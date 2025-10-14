#include "Window.hpp"

#include <QGuiApplication>
#include <QStyleHints>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include <QJsonDocument>

Window::Window(QWidget *parent) : QMainWindow(parent) {
    m_weatherClient = new WeatherClient(this);
    m_cache = new Cache(this);

    auto *center = new QWidget(this);
    auto *layout = new QVBoxLayout(center);

    m_cityLabel = new QLabel(this);
    m_cityLabel->setStyleSheet("font-size: 36px; font-weight: 400");
    m_cityLabel->setAlignment(Qt::AlignHCenter);

    m_temperatureLabel = new QLabel("— °C", this);
    m_temperatureLabel->setStyleSheet("font-size: 90px; font-weight: 600;");
    m_temperatureLabel->setAlignment(Qt::AlignHCenter);

    m_detailsLabel = new QLabel(this);
    m_detailsLabel->setStyleSheet("font-size: 24px; font-weight: 300");
    m_detailsLabel->setAlignment(Qt::AlignHCenter);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: gray;");
    m_statusLabel->setAlignment(Qt::AlignLeading);

    m_dataSourceLabel = new QLabel(this);
    m_dataSourceLabel->setStyleSheet("color: gray;");
    m_dataSourceLabel->setAlignment(Qt::AlignLeading);

    auto applyThemeToLabels = [this]{
        const Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
        const QPalette appPal = qApp->palette();

        const QColor subtle       = appPal.color(QPalette::PlaceholderText); // for status/source
        const QColor detailsColor = appPal.color(QPalette::WindowText);
        const QColor tempColor    = (scheme == Qt::ColorScheme::Dark)
                                    ? appPal.color(QPalette::BrightText)
                                    : appPal.color(QPalette::WindowText);

        auto tint = [](QLabel* L, const QColor& c){
            if (!L) return; // extra guard
            QPalette p = L->palette();
            p.setColor(QPalette::WindowText, c);
            L->setPalette(p);
        };

        tint(m_cityLabel, detailsColor);
        tint(m_temperatureLabel, tempColor);
        tint(m_detailsLabel,     detailsColor);
        tint(m_statusLabel,      subtle);
        tint(m_dataSourceLabel,  subtle);
    };

    applyThemeToLabels();
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                     this, [applyThemeToLabels](Qt::ColorScheme){ applyThemeToLabels(); });
#endif

    layout->addWidget(m_cityLabel);
    layout->addWidget(m_temperatureLabel);
    layout->addWidget(m_detailsLabel);
    layout->addStretch();
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_dataSourceLabel);

    setCentralWidget(center);
    setWindowTitle("Qt Weather");

    // Wire WeatherClient
    connect(m_weatherClient, &WeatherClient::fetchSucceeded, this, [this](const QJsonObject &jsonObject) {
       // Cache pretty-printed JSON for ease of use
        m_cache->writePayload(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
        updateUIFromJson(jsonObject);
        m_statusLabel->setText("Last updated at: " + QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate));
        scheduleNextRefresh();
    });

    connect(m_weatherClient, &WeatherClient::fetchFailed, this, [this](const QString &error) {
        m_statusLabel->setText("Fetch failed: " + error);

        // Re-attempt in 5 minutes
        m_refreshTimer.stop();
        m_refreshTimer.setSingleShot(true);
        m_refreshTimer.start(5 * 60 * 1000);
    });

    // Timer handler
    connect(&m_refreshTimer, &QTimer::timeout, [this] { startFetch(); });

    // Initial: use cache if still fresh or else fetch new data
    if (m_cache->hasValidCache(kCacheRefreshTimeoutInSeconds)) {
        auto bytes = m_cache->readPayload();
        auto document = QJsonDocument::fromJson(bytes);
        if (document.isObject()) {
            updateUIFromJson(document.object());
            auto lastFetchedTime = m_cache->lastFetch().toLocalTime();
            m_statusLabel->setText("Cached (from " + lastFetchedTime.toString() + ")");
            scheduleNextRefresh();

            return;
        }
    }

    startFetch();
}


void Window::startFetch() {
    m_statusLabel->setText("Fetching…");
    m_weatherClient->fetchWeather();
}

void Window::scheduleNextRefresh() {
    m_refreshTimer.stop();
    auto lastFetchedTime = m_cache->lastFetch();
    qint64 remaining = lastFetchedTime.isValid()
        ? kCacheRefreshTimeoutInSeconds - lastFetchedTime.secsTo(QDateTime::currentDateTimeUtc())
        : kCacheRefreshTimeoutInSeconds;
    if (remaining < 5) remaining = 5;
    m_refreshTimer.setSingleShot(true);
    m_refreshTimer.start(static_cast<int>(remaining) * 1000);
}

QString Window::weatherCodeToText(int weatherCode) {
    switch (weatherCode) {
        case 0: return "Clear";
        case 1: case 2: case 3: return "Partly cloudy";
        case 45: case 48: return "Fog";
        case 51: case 53: case 55: return "Drizzle";
        case 61: case 63: case 65: return "Rain";
        case 71: case 73: case 75: return "Snow";
        case 80: case 81: case 82: return "Showers";
        case 95: return "Thunderstorm";
        default: return "—";
    }
}

void Window::updateUIFromJson(const QJsonObject &jsonObject) {
    // Open-Meteo current weather lives under "current"
    auto currentWeather = jsonObject["current"].toObject();
    const double temperature = currentWeather["temperature_2m"].toDouble(std::numeric_limits<double>::quiet_NaN());
    const int weatherCode = currentWeather["weather_code"].toInt(-1);
    const double windSpeed = currentWeather["wind_speed_10m"].toDouble();

    if (std::isnan(temperature)) {
        m_temperatureLabel->setText("—°C");
        m_detailsLabel->setText("No data");

        return;
    }

    m_cityLabel->setText("Warsaw");
    m_temperatureLabel->setText(QString::number(static_cast<int>(std::round(temperature))) + "°C");
    m_detailsLabel->setText(
        QString("%1")
        .arg(weatherCodeToText(weatherCode))
    );
    m_dataSourceLabel->setText("Weather data provided by Open-Meteo.");
}