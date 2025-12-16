#include "ProjectIO.h"
#include "../assembly/AssemblyDocument.h"

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <unordered_map>
#include <sstream>

ProjectIO::ProjectIO() = default;

static QString serializeShape(const TopoDS_Shape &shape) {
    if (shape.IsNull()) return {};

    std::ostringstream stream;
    BRepTools::Write(shape, stream);
    const std::string data = stream.str();
    QByteArray bytes(data.data(), static_cast<int>(data.size()));
    return QString::fromLatin1(bytes.toBase64());
}

static TopoDS_Shape deserializeShape(const QString &encoded) {
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

bool ProjectIO::saveProject(const QString &filePath, const ProjectSnapshot &snapshot) {
    QJsonObject root;
    root["shape"] = serializeShape(snapshot.shape);
    QJsonArray history;
    for (const auto &line : snapshot.chatHistory) {
        history.append(line);
    }
    root["chatHistory"] = history;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

ProjectSnapshot ProjectIO::loadProject(const QString &filePath) const {
    ProjectSnapshot snapshot;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return snapshot;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return snapshot;
    }
    QJsonObject obj = doc.object();
    snapshot.shape = deserializeShape(obj.value("shape").toString());
    QJsonArray history = obj.value("chatHistory").toArray();
    for (const auto &line : history) {
        snapshot.chatHistory.push_back(line.toString());
    }
    return snapshot;
}

bool ProjectIO::saveAssembly(const QString &filePath, const AssemblyDocument &assembly) const {
    QJsonObject root;
    QJsonArray nodes;
    for (const auto &kv : assembly.nodes()) {
        const AssemblyNode &node = kv.second;
        QJsonObject obj;
        obj["id"] = node.id;
        obj["parent"] = node.parentId;
        obj["partPath"] = node.partPath;
        obj["isReferenceAssembly"] = node.isReferenceAssembly;
        QJsonArray trsf;
        for (int r = 1; r <= 3; ++r) {
            for (int c = 1; c <= 4; ++c) {
                trsf.append(node.localTransform.Value(r, c));
            }
        }
        obj["trsf"] = trsf;
        nodes.append(obj);
    }
    root["nodes"] = nodes;

    QJsonArray mates;
    for (const auto &mate : assembly.mates()) {
        QJsonObject obj;
        obj["id"] = mate.id;
        obj["a"] = mate.a;
        obj["b"] = mate.b;
        obj["type"] = jointTypeToString(mate.type);
        obj["suppressed"] = mate.suppressed;
        obj["limitMin"] = mate.limitMin;
        obj["limitMax"] = mate.limitMax;
        mates.append(obj);
    }
    root["mates"] = mates;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(root).toJson());
    return true;
}

std::shared_ptr<AssemblyDocument> ProjectIO::loadAssembly(const QString &filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }
    auto doc = std::make_shared<AssemblyDocument>();
    QJsonDocument json = QJsonDocument::fromJson(file.readAll());
    if (!json.isObject()) {
        return doc;
    }
    QJsonObject root = json.object();
    std::unordered_map<QString, AssemblyNode> nodes;
    QJsonArray nodeArray = root.value("nodes").toArray();
    for (const auto &value : nodeArray) {
        QJsonObject obj = value.toObject();
        AssemblyNode node;
        node.id = obj.value("id").toString();
        node.parentId = obj.value("parent").toString();
        node.partPath = obj.value("partPath").toString();
        node.isReferenceAssembly = obj.value("isReferenceAssembly").toBool();
        QJsonArray trsf = obj.value("trsf").toArray();
        if (trsf.size() >= 12) {
            gp_Trsf t;
            int idx = 0;
            for (int r = 1; r <= 3; ++r) {
                for (int c = 1; c <= 4; ++c) {
                    t.SetValue(r, c, trsf[idx++].toDouble());
                }
            }
            node.localTransform = t;
        }
        nodes[node.id] = node;
    }

    std::vector<MateConstraint> mates;
    QJsonArray mateArray = root.value("mates").toArray();
    for (const auto &value : mateArray) {
        QJsonObject obj = value.toObject();
        MateConstraint mate;
        mate.id = obj.value("id").toString();
        mate.a = obj.value("a").toString();
        mate.b = obj.value("b").toString();
        mate.type = jointTypeFromString(obj.value("type").toString());
        mate.suppressed = obj.value("suppressed").toBool();
        mate.limitMin = obj.value("limitMin").toDouble();
        mate.limitMax = obj.value("limitMax").toDouble();
        mates.push_back(mate);
    }
    doc->reset(nodes, mates);
    return doc;
}

