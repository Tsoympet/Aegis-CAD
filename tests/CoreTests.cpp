#include <QtTest/QtTest>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTextStream>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <cmath>

#include "cad/FeatureOps.h"
#include "cad/GltfExporter.h"
#include "cad/StepIgesIO.h"
#include "scripting/ScriptRunner.h"
#include "utils/JsonHelpers.h"
#include "utils/Settings.h"

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
QString tempFile(const QTemporaryDir &dir, const QString &name) {
    return dir.filePath(name);
}

#define VERIFY_WITH_TOLERANCE(actual, expected, tolerance)                                                                 \
    do {                                                                                                                   \
        const double delta = std::abs((actual) - (expected));                                                             \
        QVERIFY2(delta <= (tolerance),                                                                                    \
                 qPrintable(QStringLiteral("Expected %1 within %2 of %3 (|delta|=%4)")                                   \
                               .arg(actual)                                                                              \
                               .arg(tolerance)                                                                           \
                               .arg(expected)                                                                            \
                               .arg(delta)));                                                                            \
    } while (false)
}

void CoreTests::jsonHelpers_roundTrip() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "Temporary directory should be valid");

    const auto path = tempFile(dir, QStringLiteral("sample.json")).toStdString();
    JsonHelpers::JsonObject object{{"name", std::string("cube")}, {"size", int64_t{42}}};

    QVERIFY(JsonHelpers::saveToFile(path, object));

    const auto loaded = JsonHelpers::loadFromFile(path);
    QCOMPARE(std::get<std::string>(loaded.at("name")), std::string("cube"));
    QCOMPARE(std::get<int64_t>(loaded.at("size")), int64_t{42});
}

void CoreTests::settings_roundTrip() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "Temporary directory should be valid");

    const auto path = tempFile(dir, QStringLiteral("settings.json")).toStdString();
    Settings settings(path);
    settings.setValue("unitTest/key", "123");
    QCOMPARE(settings.value("unitTest/key"), std::string("123"));
}

void CoreTests::jsonHelpers_benchmark() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "Temporary directory should be valid");

    const QString path = tempFile(dir, QStringLiteral("bench.json"));
    QJsonObject object;
    object["name"] = "cube";
    object["size"] = 42;

    QBENCHMARK {
        JsonHelpers::saveToFile(path, object);
        JsonHelpers::loadFromFile(path);
    }
}

void CoreTests::step_roundTrip() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = tempFile(dir, QStringLiteral("roundtrip.step"));
    StepIgesIO io;
    const TopoDS_Shape original = FeatureOps::makeBox(10.0);
    QVERIFY(io.exportStep(path, original));

    const TopoDS_Shape imported = io.importFile(path);
    QVERIFY(!imported.IsNull());

    GProp_GProps propsOriginal;
    BRepGProp::VolumeProperties(original, propsOriginal);
    GProp_GProps propsImported;
    BRepGProp::VolumeProperties(imported, propsImported);
    VERIFY_WITH_TOLERANCE(propsOriginal.Mass(), propsImported.Mass(), 1e-6);
}

void CoreTests::iges_roundTrip() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = tempFile(dir, QStringLiteral("roundtrip.iges"));
    StepIgesIO io;
    const TopoDS_Shape original = FeatureOps::makeCylinder(5.0, 20.0);
    QVERIFY(io.exportIges(path, original));

    const TopoDS_Shape imported = io.importFile(path);
    QVERIFY(!imported.IsNull());

    GProp_GProps propsOriginal;
    BRepGProp::VolumeProperties(original, propsOriginal);
    GProp_GProps propsImported;
    BRepGProp::VolumeProperties(imported, propsImported);
    VERIFY_WITH_TOLERANCE(propsOriginal.Mass(), propsImported.Mass(), 1e-6);
}

void CoreTests::gltf_export() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = tempFile(dir, QStringLiteral("simple.gltf"));
    GltfExporter exporter;
    const TopoDS_Shape box = FeatureOps::makeBox(4.0);
    QVERIFY(exporter.exportShape(path, box));

    QFile file(path);
    QVERIFY(file.exists());
    QVERIFY(file.size() > 0);
}

void CoreTests::io_failure_logging() {
    StepIgesIO io;
    const QString missing = QDir::temp().filePath(QStringLiteral("does_not_exist.step"));
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(".*Failed to read STEP file:.*does_not_exist\\.step")));
    QVERIFY(io.importFile(missing).IsNull());

    GltfExporter exporter;
    const QString gltfPath = QDir::temp().filePath(QStringLiteral("empty.gltf"));
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(".*Cannot export glTF: no geometry.*empty\\.gltf")));
    QVERIFY(!exporter.exportShape(gltfPath, TopoDS_Shape()));
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

    const QString scriptPath = tempFile(dir, QStringLiteral("sample.py"));
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

#include "CoreTests.moc"
