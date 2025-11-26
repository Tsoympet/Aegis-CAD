#pragma once
#include <QString>
#include <QVector>
#include <gp_Pnt.hxx>

enum class AnalysisTemplateKind {
    StaticStructural,
    ThermalSteady,
    Modal,
    Buckling,
    ArmorImpact
};

struct NodeResult {
    gp_Pnt position;
    double value;  // stress or displacement magnitude
};

struct AnalysisResult {
    QString caseName;
    bool succeeded = false;
    double maxStress = 0.0;
    double maxDisplacement = 0.0;
    QString reportPath;
    QVector<NodeResult> nodes;  // for 3D visualization
};
