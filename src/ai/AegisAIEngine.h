#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include <vector>
#include <map>

class TopoDS_Shape;

class AegisAIEngine : public QObject
{
    Q_OBJECT
public:
    explicit AegisAIEngine(QObject* parent = nullptr);
    ~AegisAIEngine() override;

    bool initializePython();
    QString processCommand(const QString& input);
    QString suggestOptimization(const std::shared_ptr<TopoDS_Shape>& shape);
    QString analyzeMaterial(const QString& matName, double stress, double strain);
    QString generateSummary() const;

signals:
    void logMessage(const QString& msg);
    void actionTriggered(const QString& cmd);
    void pythonReady();

private:
    bool m_pythonInitialized{false};
    std::map<QString, double> m_materialDB;
    QString runPythonReasoning(const QString& prompt);
    QString ruleBasedResponse(const QString& input);
};
