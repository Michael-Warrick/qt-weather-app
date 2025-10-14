#ifndef WEATHER_CLIENT_HPP
#define WEATHER_CLIENT_HPP

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>

class WeatherClient final : public QObject {
    Q_OBJECT
public:
    explicit WeatherClient(QObject *parent=nullptr);
    ~WeatherClient() override = default;

    void setLocation(double latitude, double longitude);
    void fetchWeather();

signals:
    void fetchSucceeded(QJsonObject data);
    void fetchFailed(QString error);

private:
    void handleReply(class QNetworkReply *reply);

    QNetworkAccessManager m_networkAccessManager;

    // Default to Brighton
    double m_latitude = 52.2297;
    double m_longitude = 21.0122;
};

#endif // WEATHER_CLIENT_HPP