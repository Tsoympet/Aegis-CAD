#include "ProjectIO.h"
#include "../cad/StepIgesIO.h"
#include "../assembly/AssemblyDocument.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <unordered_map>

ProjectIO::ProjectIO() = default;

bool ProjectIO::saveProject(const QString &filePath, const TopoDS_Shape &shape) {
    StepIgesIO io;
    return io.exportStep(filePath, shape);
}

TopoDS_Shape ProjectIO::loadProject(const QString &filePath) const {
    StepIgesIO io;
    return io.importFile(filePath);
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

