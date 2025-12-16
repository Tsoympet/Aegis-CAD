#include <QtTest/QtTest>
#include <QFile>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTextStream>
#include <QRegularExpression>
#include <QDir>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include "scripting/ScriptRunner.h"
#include "utils/JsonHelpers.h"
#include "utils/Settings.h"
#include "cad/StepIgesIO.h"
#include "cad/GltfExporter.h"
#include "cad/FeatureOps.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <variant>
class CoreTests : public QObject {
    Q_OBJECT

private slots:
    void jsonHelpers_roundTrip();
    void settings_roundTrip();
    void jsonHelpers_benchmark();
    void step_roundTrip();
    void iges_roundTrip();
    void gltf_export();
    void io_failure_logging();
};

class ScriptingTests : public QObject {
    Q_OBJECT

private slots:
    void bindings_are_registered();
    void bindings_validate_arguments();
    void runFile_executes_script();
};

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

void CoreTests::jsonHelpers_benchmark() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "Temporary directory should be valid");

    const QString path = dir.filePath("sample.json");
    QJsonObject object;
    object["name"] = "cube";
    object["size"] = 42;

    QBENCHMARK {
        JsonHelpers::saveToFile(path, object);
        JsonHelpers::loadFromFile(path);
    }
void ScriptingTests::bindings_are_registered() {
    ScriptRunner runner;
    const QString result = runner.runSnippet(R"(import aegiscad
expected = [
    "make_box",
    "make_cylinder",
    "extrude",
    "revolve",
    "fillet",
]
for name in expected:
    assert hasattr(aegiscad, name), name
assert aegiscad.make_box.__doc__
)");

    QCOMPARE(result, QStringLiteral("[ok] Script executed"));
}

void ScriptingTests::bindings_validate_arguments() {
    ScriptRunner runner;
    const QString result = runner.runSnippet(R"(import aegiscad
try:
    aegiscad.make_box(0)
except ValueError:
    pass
else:
    raise AssertionError("size should be validated")

shape = aegiscad.Shape()
try:
    aegiscad.fillet(shape, 1)
except ValueError:
    pass
else:
    raise AssertionError("shape should be validated")

try:
    aegiscad.extrude(aegiscad.make_box(1), 1, (0, 0, 0))
except ValueError:
    pass
else:
    raise AssertionError("direction should be validated")
)");

    QCOMPARE(result, QStringLiteral("[ok] Script executed"));
}

void ScriptingTests::runFile_executes_script() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "Temporary directory should be valid");

    const QString scriptPath = dir.filePath("sample.py");
    QFile file(scriptPath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream << "import aegiscad\n";
    stream << "shape = aegiscad.make_cylinder(1.5, 3.0)\n";
    stream << "assert shape is not None\n";
    file.close();

    ScriptRunner runner;
    const QString result = runner.runFile(scriptPath);
    QCOMPARE(result, QStringLiteral("[ok] Script executed"));
}

int main(int argc, char **argv) {
    int status = 0;
    {
        CoreTests core;
        status += QTest::qExec(&core, argc, argv);
    }
    {
        ScriptingTests scripting;
        status += QTest::qExec(&scripting, argc, argv);
    }
    return status;
}

void CoreTests::step_roundTrip() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("roundtrip.step");
    StepIgesIO io;
    const TopoDS_Shape original = FeatureOps::makeBox(10.0);
    QVERIFY(io.exportStep(path, original));

    const TopoDS_Shape imported = io.importFile(path);
    QVERIFY(!imported.IsNull());

    GProp_GProps propsOriginal;
    BRepGProp::VolumeProperties(original, propsOriginal);
    GProp_GProps propsImported;
    BRepGProp::VolumeProperties(imported, propsImported);
    QCOMPARE_WITH_SIGNED_TOLERANCE(propsOriginal.Mass(), propsImported.Mass(), 1e-6);
}

void CoreTests::iges_roundTrip() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("roundtrip.iges");
    StepIgesIO io;
    const TopoDS_Shape original = FeatureOps::makeCylinder(5.0, 20.0);
    QVERIFY(io.exportIges(path, original));

    const TopoDS_Shape imported = io.importFile(path);
    QVERIFY(!imported.IsNull());

    GProp_GProps propsOriginal;
    BRepGProp::VolumeProperties(original, propsOriginal);
    GProp_GProps propsImported;
    BRepGProp::VolumeProperties(imported, propsImported);
    QCOMPARE_WITH_SIGNED_TOLERANCE(propsOriginal.Mass(), propsImported.Mass(), 1e-6);
}

void CoreTests::gltf_export() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("simple.gltf");
    GltfExporter exporter;
    const TopoDS_Shape box = FeatureOps::makeBox(4.0);
    QVERIFY(exporter.exportShape(path, box));

    QFile file(path);
    QVERIFY(file.exists());
    QVERIFY(file.size() > 0);
}

void CoreTests::io_failure_logging() {
    StepIgesIO io;
    const QString missing = QDir::temp().filePath("does_not_exist.step");
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*Failed to read STEP file:.*does_not_exist\\.step"));
    QVERIFY(io.importFile(missing).IsNull());

    GltfExporter exporter;
    const QString gltfPath = QDir::temp().filePath("empty.gltf");
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*Cannot export glTF: no geometry.*empty\\.gltf"));
    QVERIFY(!exporter.exportShape(gltfPath, TopoDS_Shape()));
}

QTEST_APPLESS_MAIN(CoreTests)
#include "CoreTests.moc"
