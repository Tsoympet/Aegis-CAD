#pragma once

#include <TopoDS_Shape.hxx>
#include <QString>
#include <QJsonObject>
#include <vector>

class PartRegistry {
public:
    struct Entry {
        QString id;
        QString name;
        TopoDS_Shape shape;
        bool visible{true};
        QString material{QStringLiteral("Aluminum 6061")};
        double density{2700.0};
        double yieldStrength{250e6};
    };

    PartRegistry();

    QString addPart(const QString &name, const TopoDS_Shape &shape);
    void updatePart(const QString &id, const TopoDS_Shape &shape);
    void setMaterial(const QString &id, const QString &material, double density = 2700.0, double yieldStrength = 250e6);
    TopoDS_Shape activeShape() const;
    QString activeId() const { return m_activeId; }
    std::vector<Entry> parts() const { return m_parts; }

    bool exportToJson(const QString &path) const;
    bool importFromJson(const QString &path);

    TopoDS_Shape synthesizeFromPrompt(const QString &prompt);

private:
    static QString serializeShape(const TopoDS_Shape &shape);
    static TopoDS_Shape deserializeShape(const QString &encoded);

    std::vector<Entry> m_parts;
    QString m_activeId;
};

