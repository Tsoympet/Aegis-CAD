#pragma once

#include <TopoDS_Shape.hxx>
#include <QString>
#include <vector>

class PartRegistry {
public:
    struct Entry {
        QString name;
        TopoDS_Shape shape;
    };

    PartRegistry();

    void addPart(const QString &name, const TopoDS_Shape &shape);
    TopoDS_Shape activeShape() const;
    std::vector<Entry> parts() const { return m_parts; }
    TopoDS_Shape synthesizeFromPrompt(const QString &prompt);

private:
    std::vector<Entry> m_parts;
};

