#include "ProjectIO.h"
#include "ai/AegisAIEngine.h"
#include <QFile>
#include <QJsonDocument>
#include <QDateTime>
#include <QDebug>

bool ProjectIO::saveProject(const QString& filePath,
                            const QJsonObject& cadState,
                            const QJsonObject& analysisState,
                            const PartContext& aiContext,
                            const QVector<QString>& aiConsoleLog)
{
    QJsonObject root;
    root["project_name"] = cadState.value("name").toString("Untitled");
    root["version"] = "1.0";
    root["last_saved"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["cad_state"] = cadState;
    root["analysis_state"] = analysisState;
    root["current_context"] = aiContext.toJson();

    QJsonArray logArray;
    for (const QString& line : aiConsoleLog)
        logArray.append(line);
    root["ai_console_log"] = logArray;

    return writeJsonFile(filePath, root);
}

bool ProjectIO::loadProject(const QString& filePath,
                            QJsonObject& cadState,
                            QJsonObject& analysisState,
                            PartContext& aiContext,
                            QVector<QString>& aiConsoleLog)
{
    QJsonObject root = readJsonFile(filePath);
    if (root.isEmpty()) return false;

    cadState = root["cad_state"].toObject();
    analysisState = root["analysis_state"].toObject();

    if (root.contains("current_context"))
        aiContext = PartContext::fromJson(root["current_context"].toObject());

    aiConsoleLog.clear();
    if (root.contains("ai_console_log")) {
        QJsonArray arr = root["ai_console_log"].toArray();
        for (const auto& v : arr)
            aiConsoleLog.append(v.toString());
    }

    qDebug() << "Loaded project:" << filePath
             << "| AI log lines:" << aiConsoleLog.size()
             << "| Context:" << aiContext.name;
    return true;
}

QJsonObject ProjectIO::readJsonFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << path;
        return {};
    }
    const QByteArray data = file.readAll();
    file.close();
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << err.errorString();
        return {};
    }
    return doc.object();
}

bool ProjectIO::writeJsonFile(const QString& path, const QJsonObject& obj)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Failed to write" << path;
        return false;
    }
    const QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
