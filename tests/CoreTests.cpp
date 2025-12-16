#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonObject>
#include <QRegularExpression>
#include <QDir>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include "utils/JsonHelpers.h"
#include "utils/Settings.h"
#include "cad/StepIgesIO.h"
#include "cad/GltfExporter.h"
#include "cad/FeatureOps.h"

class CoreTests : public QObject {
    Q_OBJECT

private slots:
    void jsonHelpers_roundTrip();
    void settings_roundTrip();
    void step_roundTrip();
    void iges_roundTrip();
    void gltf_export();
    void io_failure_logging();
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
