#include <QtTest/QtTest>
#include <QFile>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTextStream>

#include "scripting/ScriptRunner.h"
#include "utils/JsonHelpers.h"
#include "utils/Settings.h"

class CoreTests : public QObject {
    Q_OBJECT

private slots:
    void jsonHelpers_roundTrip();
    void settings_roundTrip();
};

class ScriptingTests : public QObject {
    Q_OBJECT

private slots:
    void bindings_are_registered();
    void bindings_validate_arguments();
    void runFile_executes_script();
};

void CoreTests::jsonHelpers_roundTrip() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "Temporary directory should be valid");

    const QString path = dir.filePath("sample.json");
    QJsonObject object;
    object["name"] = "cube";
    object["size"] = 42;

    QVERIFY2(JsonHelpers::saveToFile(path, object), "Saving JSON should succeed");

    const QJsonObject loaded = JsonHelpers::loadFromFile(path);
    QCOMPARE(loaded.value("name").toString(), QString("cube"));
    QCOMPARE(loaded.value("size").toInt(), 42);
}

void CoreTests::settings_roundTrip() {
    Settings settings;
    settings.setValue("unitTest/key", 123);
    QCOMPARE(settings.value("unitTest/key").toInt(), 123);
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

#include "CoreTests.moc"
