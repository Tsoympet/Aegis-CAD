#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <memory>
#include <map>

class TopoDS_Shape;

// Context for currently selected part (live or serialized in .aegisproj)
struct PartContext {
    QString name;
    QString material;
    QString type;
    double mass{0.0};
    double volume{0.0};
    double stress{0.0};
    double fos{0.0};

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["material"] = material;
        obj["type"] = type;
        obj["mass"] = mass;
        obj["volume"] = volume;
        obj["stress"] = stress;
        obj["fos"] = fos;
        return obj;
    }

    static PartContext fromJson(const QJsonObject& obj) {
        PartContext c;
        c.name = obj["name"].toString();
        c.material = obj["material"].toString();
        c.type = obj["type"].toString();
        c.mass = obj["mass"].toDouble();
        c.volume = obj["volume"].toDouble();
        c.stress = obj["stress"].toDouble();
        c.fos = obj["fos"].toDouble();
        return c;
    }

    bool isValid() const { return !name.isEmpty(); }
};

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

    void setCurrentPart(const PartContext& ctx);
    const PartContext& currentPart() const { return m_currentPart; }
    QJsonObject contextToJson() const { return m_currentPart.toJson(); }
    void loadContextFromJson(const QJsonObject& obj) { m_currentPart = PartContext::fromJson(obj); }

signals:
    void logMessage(const QString& msg);
    void actionTriggered(const QString& cmd);
    void pythonReady();

private:
    bool m_pythonInitialized{false};
    std::map<QString, double> m_materialDB;
    PartContext m_currentPart;

    QString runPythonReasoning(const QString& prompt);
    QString ruleBasedResponse(const QString& input);
    QString contextualResponse(const QString& base, bool conversational) const;
};
