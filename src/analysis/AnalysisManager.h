#pragma once
#include <QString>
#include <QVector>
#include <gp_Pnt.hxx>

/// Enum representing supported analysis types.
enum class AnalysisTemplateKind {
    StaticStructural,
    ThermalSteady,
    Modal,
    Buckling,
    ArmorImpact
};

/// Node-level result used for visualization.
struct NodeResult {
    gp_Pnt position;
    double value = 0.0;  ///< stress, temperature, etc.
};

/// High-level analysis result metadata.
struct AnalysisResult {
    QString caseName;
    bool succeeded = false;
    double maxStress = 0.0;
    double maxDisplacement = 0.0;
    QString reportPath;
    QVector<NodeResult> nodes;  ///< per-node results for 3D visualization
};
