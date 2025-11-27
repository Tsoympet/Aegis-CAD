#pragma once
#include <QString>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

struct Mate
{
    QString name;
    QString partA;
    QString partB;
    gp_Pnt anchorA;
    gp_Pnt anchorB;
    gp_Dir normalA;
    gp_Dir normalB;
};
