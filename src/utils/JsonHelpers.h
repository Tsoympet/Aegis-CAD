#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace JsonHelpers {
QJsonObject loadFromFile(const QString &path);
bool saveToFile(const QString &path, const QJsonObject &object);
}

