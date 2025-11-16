#include "GCodeExport.h"
#include <fstream>

bool GCodeExport::save(const std::string& path, const std::string& gcode)
{
    std::ofstream f(path);
    if (!f) return false;
    f << gcode;
    return true;
}
