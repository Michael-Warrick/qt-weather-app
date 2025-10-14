#include "WeatherClient.hpp"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

WeatherClient::WeatherClient(QObject *parent) : QObject(parent) {}

void WeatherClient::setLocation(double latitude, double longitude) {
    m_latitude = latitude;
    m_longitude = longitude;
}

void WeatherClient::fetchWeather() {
    // Open-Meteo
    QUrl url("https://api.open-meteo.com/v1/forecast");

    QUrlQuery query;
    query.addQueryItem("latitude", QString::number(m_latitude, 'f', 4));
    query.addQueryItem("longitude", QString::number(m_longitude, 'f', 4));
    query.addQueryItem("current", "temperature_2m,relative_humidity_2m,apparent_temperature,weather_code,wind_speed_10m");

    url.setQuery(query);

    auto *reply = m_networkAccessManager.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]{ handleReply(reply); });
}

void WeatherClient::handleReply(QNetworkReply *reply) {
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit fetchFailed(reply->errorString());
        return;
    }

    auto bytes = reply->readAll();
    auto document = QJsonDocument::fromJson(bytes);
    if (!document.isObject()) {
        emit fetchFailed("Invalid JSON");
    }

    emit fetchSucceeded(document.object());
}