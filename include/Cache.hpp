#ifndef CACHE_HPP
#define CACHE_HPP

#include <QObject>
#include <QDateTime>
#include <QByteArray>

class Cache final : public QObject {
    Q_OBJECT
public:
    explicit Cache(QObject *parent=nullptr);
    ~Cache() override = default;

    [[nodiscard]] bool hasValidCache(qint64 maxAgeInSeconds) const;
    [[nodiscard]] QByteArray readPayload() const;
    void writePayload(const QByteArray &jsonPayload);

    [[nodiscard]] QDateTime lastFetch() const;

private:
    [[nodiscard]] QString dataPath() const;
    [[nodiscard]] QString metaDataPath() const;
};

#endif // CACHE_HPP
