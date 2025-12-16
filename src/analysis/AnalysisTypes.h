#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <vector>

enum class DomainTemplateKind { Car, Ship, Aircraft, Armor };

enum class LoadType { Force, Pressure, Temperature }; 

enum class ConstraintType { Fixed, Slider, Symmetry };

struct MaterialProperty {
    QString name;
    double density{7850.0};
    double elasticModulus{2.0e11};
    double yieldStrength{250e6};
    double thermalConductivity{45.0};
};

struct LoadDefinition {
    LoadType type{LoadType::Force};
    QString targetPartId;
    gp_Vec direction{0, 0, -1};
    double magnitude{1000.0};
    QString regionHint;
};

struct ConstraintDefinition {
    ConstraintType type{ConstraintType::Fixed};
    gp_Pnt anchor{0, 0, 0};
    gp_Dir normal{0, 0, 1};
    QString regionHint;
};

struct AnalysisCase {
    QString name;
    MaterialProperty material;
    std::vector<LoadDefinition> loads;
    std::vector<ConstraintDefinition> constraints;
    DomainTemplateKind domain{DomainTemplateKind::Car};
};

