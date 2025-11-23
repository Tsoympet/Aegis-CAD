#pragma once
#include <QString>
#include <QJsonObject>
#include <QVector>
#include <memory>

struct PartContext;

class ProjectIO
{
public:
    static bool saveProject(const QString& filePath,
                            const QJsonObject& cadState,
                            const QJsonObject& analysisState,
                            const PartContext& aiContext,
                            const QVector<QString>& aiConsoleLog);

    static bool loadProject(const QString& filePath,
                            QJsonObject& cadState,
                            QJsonObject& analysisState,
                            PartContext& aiContext,
                            QVector<QString>& aiConsoleLog);

    static QJsonObject readJsonFile(const QString& path);
    static bool writeJsonFile(const QString& path, const QJsonObject& obj);
};
