#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <variant>

namespace JsonHelpers {
using JsonValue = std::variant<std::string, int64_t, double, bool>;
using JsonObject = std::map<std::string, JsonValue>;

JsonObject loadFromFile(const std::filesystem::path &path);
bool saveToFile(const std::filesystem::path &path, const JsonObject &object);

std::string toString(const JsonValue &value);
}

