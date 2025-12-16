#include "Logging.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
std::string timestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

void logWithLevel(const std::string &level, const std::string &msg) {
    std::cout << timestamp() << " [" << level << "] " << msg << std::endl;
}
}

namespace Logging {
void init() {
    info("Logging initialized");
}

void info(const std::string &msg) {
    logWithLevel("INFO", msg);
}

void warn(const std::string &msg) {
    logWithLevel("WARN", msg);
}
} // namespace Logging

