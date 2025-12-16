#pragma once

#include <string>

namespace Logging {
void init();
void info(const std::string &msg);
void warn(const std::string &msg);
}

