#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonObject>

#include "utils/JsonHelpers.h"
#include "utils/Settings.h"

class CoreTests : public QObject {
    Q_OBJECT

private slots:
    void jsonHelpers_roundTrip();
    void settings_roundTrip();
    void jsonHelpers_benchmark();
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
}

QTEST_APPLESS_MAIN(CoreTests)
#include "CoreTests.moc"
