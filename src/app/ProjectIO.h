#pragma once
#include <QString>

class ProjectIO
{
public:
    static bool save(const QString& path);
    static bool load(const QString& path);
};
