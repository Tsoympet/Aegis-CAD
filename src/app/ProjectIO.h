#pragma once
#include <QString>
#include <QJsonObject>
#include <QVector>
#include <QImage>
#include <memory>

struct PartContext;

class ProjectIO
{
public:
    static bool saveProject(const QString& filePath,
                            const QJsonObject& cadState,
                            const QJsonObject& analysisState,
                            const PartContext& aiContext,
                            const QVector<QString>& aiConsoleLog,
                            const QImage& thumbnail,
                            const QString& thumbFileName = "project_thumbnail.png");

    static bool loadProject(const QString& filePath,
                            QJsonObject& cadState,
                            QJsonObject& analysisState,
                            PartContext& aiContext,
                            QVector<QString>& aiConsoleLog,
                            QString& thumbPath);

    static QJsonObject readJsonFile(const QString& path);
    static bool writeJsonFile(const QString& path, const QJsonObject& obj);
};
