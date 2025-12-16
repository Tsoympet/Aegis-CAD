#include "JsonHelpers.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>

namespace {
std::string trim(const std::string &value) {
    const auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
    const auto begin = std::find_if(value.begin(), value.end(), notSpace);
    const auto end = std::find_if(value.rbegin(), value.rend(), notSpace).base();
    if (begin >= end) return {};
    return {begin, end};
}

std::string escapeString(const std::string &value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '"' || ch == '\\') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

std::optional<JsonHelpers::JsonValue> parseScalar(const std::string &text) {
    if (text.empty()) return std::nullopt;

    if (text.front() == '"' && text.back() == '"') {
        return text.substr(1, text.size() - 2);
    }

    if (text == "true") return true;
    if (text == "false") return false;

    int64_t intValue{};
    if (std::from_chars(text.data(), text.data() + text.size(), intValue).ec == std::errc{}) {
        return intValue;
    }

    double doubleValue{};
    std::istringstream iss(text);
    if (iss >> doubleValue && iss.rdbuf()->in_avail() == 0) {
        return doubleValue;
    }

    return std::nullopt;
}
}

namespace JsonHelpers {
JsonObject loadFromFile(const std::filesystem::path &path) {
    std::ifstream stream(path);
    if (!stream.is_open()) {
        return {};
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();
    std::string content = buffer.str();
    JsonObject object;

    const auto start = content.find('{');
    const auto end = content.rfind('}');
    if (start == std::string::npos || end == std::string::npos || start >= end) {
        return object;
    }

    const std::string body = content.substr(start + 1, end - start - 1);
    std::stringstream kvStream(body);
    std::string pair;
    while (std::getline(kvStream, pair, ',')) {
        const auto colon = pair.find(':');
        if (colon == std::string::npos) continue;

        std::string key = trim(pair.substr(0, colon));
        std::string value = trim(pair.substr(colon + 1));

        if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
            key = key.substr(1, key.size() - 2);
        }

        const auto parsed = parseScalar(value);
        if (parsed) {
            object[key] = *parsed;
        }
    }

    return object;
}

bool saveToFile(const std::filesystem::path &path, const JsonObject &object) {
    const auto parent = path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    std::ofstream stream(path);
    if (!stream.is_open()) {
        return false;
    }

    stream << "{\n";
    for (auto it = object.begin(); it != object.end(); ++it) {
        stream << "  \"" << escapeString(it->first) << "\": " << toString(it->second);
        if (std::next(it) != object.end()) {
            stream << ',';
        }
        stream << "\n";
    }
    stream << "}\n";
    return true;
}

std::string toString(const JsonValue &value) {
    return std::visit(
        [](const auto &v) -> std::string {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return '"' + escapeString(v) + '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                return v ? "true" : "false";
            } else {
                return std::to_string(v);
            }
        },
        value);
}
} // namespace JsonHelpers

