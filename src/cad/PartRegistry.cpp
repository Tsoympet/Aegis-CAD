#include "PartRegistry.h"
#include "FeatureOps.h"

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <QBuffer>
#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QtGlobal>
#include <sstream>

PartRegistry::PartRegistry() = default;

QString PartRegistry::addPart(const QString &name, const TopoDS_Shape &shape) {
    QString id = QString::number(QRandomGenerator::global()->generate64(), 16);
    m_parts.push_back({id, name, shape, true});
    m_activeId = id;
    return id;
}

void PartRegistry::updatePart(const QString &id, const TopoDS_Shape &shape) {
    for (auto &entry : m_parts) {
        if (entry.id == id) {
            entry.shape = shape;
            m_activeId = id;
            return;
        }
    }
    addPart(id, shape);
}

void PartRegistry::setMaterial(const QString &id, const QString &material, double density, double yieldStrength) {
    for (auto &entry : m_parts) {
        if (entry.id == id) {
            entry.material = material;
            entry.density = density;
            entry.yieldStrength = yieldStrength;
            return;
        }
    }
}

TopoDS_Shape PartRegistry::activeShape() const {
    for (const auto &entry : m_parts) {
        if (entry.id == m_activeId) {
            return entry.shape;
        }
    }
    if (!m_parts.empty()) {
        return m_parts.back().shape;
    }
    return TopoDS_Shape();
}

QString PartRegistry::serializeShape(const TopoDS_Shape &shape) {
    if (shape.IsNull()) return {};

    std::ostringstream stream;
    BRepTools::Write(shape, stream);
    const std::string data = stream.str();
    QByteArray bytes(data.data(), static_cast<int>(data.size()));
    return QString::fromLatin1(bytes.toBase64());
}

TopoDS_Shape PartRegistry::deserializeShape(const QString &encoded) {
    if (encoded.isEmpty()) return TopoDS_Shape();
    QByteArray bytes = QByteArray::fromBase64(encoded.toLatin1());
    std::string buffer(bytes.constData(), bytes.size());
    std::istringstream stream(buffer);
    TopoDS_Shape shape;
    BRep_Builder builder;
    if (BRepTools::Read(shape, stream, builder)) {
        return shape;
    }
    return TopoDS_Shape();
}

bool PartRegistry::exportToJson(const QString &path) const {
    QJsonArray array;
    for (const auto &entry : m_parts) {
        QJsonObject obj;
        obj["id"] = entry.id;
        obj["name"] = entry.name;
        obj["visible"] = entry.visible;
        obj["material"] = entry.material;
        obj["density"] = entry.density;
        obj["yieldStrength"] = entry.yieldStrength;
        obj["brep"] = serializeShape(entry.shape);
        array.push_back(obj);
    }

    QJsonDocument doc(array);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool PartRegistry::importFromJson(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;
    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return false;

    m_parts.clear();
    for (const auto &value : doc.array()) {
        const auto obj = value.toObject();
        Entry entry;
        entry.id = obj["id"].toString();
        entry.name = obj["name"].toString();
        entry.visible = obj["visible"].toBool(true);
        entry.material = obj.value("material").toString(entry.material);
        entry.density = obj.value("density").toDouble(entry.density);
        entry.yieldStrength = obj.value("yieldStrength").toDouble(entry.yieldStrength);
        entry.shape = deserializeShape(obj["brep"].toString());
        m_parts.push_back(entry);
    }
    if (!m_parts.empty()) {
        m_activeId = m_parts.front().id;
    }
    return true;
}

TopoDS_Shape PartRegistry::synthesizeFromPrompt(const QString &prompt) {
    QString lower = prompt.toLower();
    if (lower.contains("cylinder")) {
        auto shape = FeatureOps::makeCylinder(20.0, 40.0);
        addPart("PromptCylinder", shape);
        return shape;
    }
    auto shape = FeatureOps::makeBox(50.0);
    addPart("PromptBox", shape);
    return shape;
}

