#include "Cache.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Cache::Cache(QObject *parent) : QObject(parent) {}

bool Cache::hasValidCache(const qint64 maxAgeInSeconds) const {
    if (const QFile file(dataPath()); !file.exists()) {
        return false;
    }

    const auto lastFetchedTime = lastFetch();
    if (!lastFetchedTime.isValid()) {
        return false;
    }

    return lastFetchedTime.secsTo(QDateTime::currentDateTimeUtc()) <= maxAgeInSeconds;
}

QByteArray Cache::readPayload() const {
    QFile file(dataPath());
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    return file.readAll();
}

void Cache::writePayload(const QByteArray &jsonPayload) {
    // Write out data
    {
        QFile file(dataPath());
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(jsonPayload);
        }
    }

    // Write out metadata
    {
        QFile file(metaDataPath());
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QJsonObject jsonObject;
            jsonObject["lastFetchedTime"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            QJsonDocument(jsonObject).toJson();
            file.write(QJsonDocument(jsonObject).toJson());
        }
    }
}

QDateTime Cache::lastFetch() const {
    QFile file(metaDataPath());
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const auto document = QJsonDocument::fromJson(file.readAll());
    const auto timeString = document.object()["lastFetchedTime"].toString();

    return QDateTime::fromString(timeString, Qt::ISODate);
}

QString Cache::dataPath() const {
    const auto directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);

    const QString cachePath = directory + "/weather_cache.json";
    // qDebug() << "[Cache] cache path: " << cachePath;

    return cachePath;
}

QString Cache::metaDataPath() const {
    const auto directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);

    const QString metadataPath = directory + "/weather_metadata.json";
    // qDebug() << "[Cache] metadata path: " << metadataPath;

    return metadataPath;
}