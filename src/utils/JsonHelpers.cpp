#include "JsonHelpers.h"

#include <QFile>

namespace JsonHelpers {

QJsonObject loadFromFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    const auto doc = QJsonDocument::fromJson(file.readAll());
    return doc.object();
}

bool saveToFile(const QString &path, const QJsonObject &object) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    QJsonDocument doc(object);
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

} // namespace JsonHelpers

