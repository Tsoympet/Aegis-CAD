#include "Settings.h"

#include "JsonHelpers.h"

#include <cstdlib>
#include <filesystem>
#include <variant>

namespace {
std::filesystem::path defaultSettingsPath() {
    if (const char *home = std::getenv("HOME")) {
        return std::filesystem::path(home) / ".config" / "aegiscad" / "settings.json";
    }
    return std::filesystem::path("settings.json");
}
}

Settings::Settings(std::filesystem::path storagePath)
    : m_storagePath(storagePath.empty() ? defaultSettingsPath() : std::move(storagePath)) {
    const auto existing = JsonHelpers::loadFromFile(m_storagePath);
    for (const auto &entry : existing) {
        m_values[entry.first] = std::visit(
            [](const auto &value) -> std::string {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    return value ? "true" : "false";
                } else {
                    return std::to_string(value);
                }
            },
            entry.second);
    }
}

std::string Settings::value(const std::string &key, const std::string &def) const {
    const auto it = m_values.find(key);
    if (it == m_values.end()) {
        return def;
    }
    return it->second;
}

void Settings::setValue(const std::string &key, const std::string &value) {
    m_values[key] = value;
    flush();
}

void Settings::flush() const {
    JsonHelpers::JsonObject object;
    for (const auto &pair : m_values) {
        object[pair.first] = pair.second;
    }
    JsonHelpers::saveToFile(m_storagePath, object);
}

