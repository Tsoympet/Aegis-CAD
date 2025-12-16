#include "utils/JsonHelpers.h"
#include "utils/Logging.h"
#include "utils/Settings.h"

#include <filesystem>
#include <iostream>
#include <variant>

int main() {
    Logging::init();

    Settings settings;
    Logging::info("Settings file: " + settings.storagePath().string());

    const std::string lastProject = settings.value("session/lastProject", "none");
    Logging::info("Last project: " + lastProject);

    settings.setValue("session/lastProject", "demo-project");

    JsonHelpers::JsonObject demoJson{
        {"name", std::string("AegisCAD")},
        {"version", int64_t{1}},
        {"headless", true},
    };

    const auto outputPath = std::filesystem::current_path() / "sample_project.json";
    if (JsonHelpers::saveToFile(outputPath, demoJson)) {
        Logging::info("Wrote sample JSON to " + outputPath.string());
    } else {
        Logging::warn("Could not write sample JSON to " + outputPath.string());
    }

    const auto loaded = JsonHelpers::loadFromFile(outputPath);
    auto nameIt = loaded.find("name");
    if (nameIt != loaded.end()) {
        std::cout << "Loaded project name: " << std::get<std::string>(nameIt->second) << "\n";
    }

    return 0;
}

