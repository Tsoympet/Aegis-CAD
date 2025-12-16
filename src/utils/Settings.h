#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

class Settings {
public:
    explicit Settings(std::filesystem::path storagePath = {});

    std::string value(const std::string &key, const std::string &def = {}) const;
    void setValue(const std::string &key, const std::string &value);
    void flush() const;
    std::filesystem::path storagePath() const { return m_storagePath; }

private:
    std::filesystem::path m_storagePath;
    std::unordered_map<std::string, std::string> m_values;
};

