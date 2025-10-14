#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QJsonObject>

#include "WeatherClient.hpp"
#include "Cache.hpp"

class QLabel;

class Window final : public QMainWindow {
    Q_OBJECT
public:
    explicit Window(QWidget *parent=nullptr);
    ~Window() override = default;

private:
    void updateUIFromJson(const QJsonObject &jsonObject);
    void scheduleNextRefresh();
    void startFetch();

    static QString weatherCodeToText(int weatherCode);

    static constexpr qint64 kCacheRefreshTimeoutInSeconds = 30 * 60 * 1000; // 1800 seconds (30 minutes)

    WeatherClient *m_weatherClient;
    Cache *m_cache;
    QTimer m_refreshTimer;

    // UI bits
    QLabel *m_cityLabel;
    QLabel *m_temperatureLabel;
    QLabel *m_detailsLabel;
    QLabel *m_statusLabel;
    QLabel *m_dataSourceLabel;
};

#endif // WINDOW_HPP