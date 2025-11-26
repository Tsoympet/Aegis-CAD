#include "BackendFEA_CalculiX.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QRegularExpression>
#include <gp_Pnt.hxx>
#include <random>

BackendFEA_CalculiX::BackendFEA_CalculiX(QObject* parent)
    : QObject(parent)
{}

AnalysisResult BackendFEA_CalculiX::runAnalysis(
    const QString& projectPath,
    AnalysisTemplateKind kind,
    std::function<void(int)> progressCallback)
{
    AnalysisResult result;
    result.caseName = "Case_" + QString::number(static_cast<int>(kind));

    progressCallback(10);

    QString inpFile = generateInputFile(projectPath, kind);
    QString workDir = QFileInfo(inpFile).absolutePath();
    QString exe = "ccx";  // Ensure CalculiX is in PATH

    QProcess process;
    process.setWorkingDirectory(workDir);
    process.start(exe, { QFileInfo(inpFile).baseName() });
    process.waitForFinished(-1);

    progressCallback(70);

    QString datPath = workDir + "/" + QFileInfo(inpFile).baseName() + ".dat";
    result = parseResults(projectPath, datPath);
    result.reportPath = datPath;

    progressCallback(90);

    result.succeeded = QFile::exists(datPath);
    return result;
}

QString BackendFEA_CalculiX::generateInputFile(const QString& projectPath, AnalysisTemplateKind kind)
{
    QString inpDir = QDir(projectPath).absolutePath() + "/analysis/";
    QDir().mkpath(inpDir);
    QString inpFile = inpDir + "aegis_case.inp";

    QFile f(inpFile);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&f);
        ts << "*HEADING\nAegisCAD Simulation Input\n";

        switch (kind) {
            case AnalysisTemplateKind::StaticStructural:
                ts << "*STEP\n*STATIC\n";
                ts << "*BOUNDARY\nNall,1,3,0.0\n";
                ts << "*CLOAD\nN1,3,-1000.\n";
                break;
            case AnalysisTemplateKind::ThermalSteady:
                ts << "*STEP\n*HEAT TRANSFER, STEADY STATE\n";
                ts << "*BOUNDARY\nN1,11,11,373.\n";
                ts << "*CFLUX\nN2,11,100.\n";
                break;
            case AnalysisTemplateKind::Modal:
                ts << "*FREQUENCY, EIGENSOLVER=LANCZOS\n5\n";
                break;
            case AnalysisTemplateKind::Buckling:
                ts << "*BUCKLE\n5\n";
                break;
            case AnalysisTemplateKind::ArmorImpact:
                ts << "*DYNAMIC, EXPLICIT\n0.1,1.0\n";
                ts << "*INITIAL CONDITIONS, TYPE=VELOCITY\nN10,3,-50.\n";
                break;
        }

        ts << "*END STEP\n";
        f.close();
    }

    return inpFile;
}

AnalysisResult BackendFEA_CalculiX::parseResults(const QString& projectPath, const QString& datFile)
{
    AnalysisResult result;
    result.caseName = QFileInfo(datFile).baseName();

    QFile f(datFile);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return result;

    QTextStream ts(&f);
    QString text = ts.readAll();
    f.close();

    QRegularExpression reStress("MAXIMUM STRESS:\\s+(\\d+\\.\\d+)");
    QRegularExpression reDisp("MAXIMUM DISPLACEMENT:\\s+(\\d+\\.\\d+)");

    auto sm = reStress.match(text);
    auto dm = reDisp.match(text);
    if (sm.hasMatch()) result.maxStress = sm.captured(1).toDouble();
    if (dm.hasMatch()) result.maxDisplacement = dm.captured(1).toDouble();

    // Mock node results for visualization
    std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<double> dist(-50, 50);
    std::uniform_real_distribution<double> val(0, result.maxStress);

    for (int i = 0; i < 80; ++i) {
        NodeResult n;
        n.position = gp_Pnt(dist(rng), dist(rng), dist(rng));
        n.value = val(rng);
        result.nodes.append(n);
    }

    return result;
}
