#include "utils/JsonHelpers.h"
#include "utils/Settings.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <variant>

namespace {
void testJsonHelpers(const std::filesystem::path &tempDir) {
    std::filesystem::create_directories(tempDir);
    const auto path = tempDir / "sample.json";

    JsonHelpers::JsonObject object{{"name", std::string("cube")}, {"size", int64_t{42}}};
    const bool saved = JsonHelpers::saveToFile(path, object);
    assert(saved && "Saving JSON should succeed");

    const auto loaded = JsonHelpers::loadFromFile(path);
    assert(loaded.at("name") == JsonHelpers::JsonValue{std::string("cube")});
    assert(std::get<int64_t>(loaded.at("size")) == 42);
}

void testSettings(const std::filesystem::path &tempDir) {
    const auto settingsPath = tempDir / "settings.json";
    Settings settings(settingsPath);
    settings.setValue("unitTest/key", "123");
    assert(settings.value("unitTest/key") == "123");
}
}

int main() {
    const auto tempDir = std::filesystem::temp_directory_path() / "aegiscad-tests";
    testJsonHelpers(tempDir);
    testSettings(tempDir);
    std::cout << "All tests passed\n";
    return 0;
}

