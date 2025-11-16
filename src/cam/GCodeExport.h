#pragma once
#include <string>

class GCodeExport
{
public:
    bool save(const std::string& path, const std::string& gcode);
};
