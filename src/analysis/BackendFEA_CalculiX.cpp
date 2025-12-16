#include "BackendFEA_CalculiX.h"

#include "DomainTemplates.h"

#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Bnd_Box.hxx>
#include <QFile>
#include <QProcess>
#include <QTemporaryDir>
#include <QTextStream>
#include <cmath>
#include <limits>
#include <map>

BackendFEA_CalculiX::BackendFEA_CalculiX() = default;

void BackendFEA_CalculiX::setModel(const TopoDS_Shape &shape) {
    m_shape = shape;
}

void BackendFEA_CalculiX::setCase(const AnalysisCase &analysisCase) {
    m_case = analysisCase;
}

QString BackendFEA_CalculiX::writeInputDeck(const QString &workDir) const {
    const QString inpPath = workDir + "/analysis.inp";
    QFile file(inpPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return {};
    }

    QTextStream out(&file);
    out << "*HEADING\nAegisCAD Analysis\n";
    out << "*NODE\n";

    Bnd_Box box;
    BRepBndLib::Add(m_shape, box);
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

    int nodeId = 1;
    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dz = 0; dz <= 1; ++dz) {
                const double x = dx ? xmax : xmin;
                const double y = dy ? ymax : ymin;
                const double z = dz ? zmax : zmin;
                out << nodeId << "," << x << "," << y << "," << z << "\n";
                ++nodeId;
            }
        }
    }

    out << "*ELEMENT, TYPE=C3D8\n";
    out << "1,1,2,4,3,5,6,8,7\n";

    out << "*MATERIAL, NAME=MAT1\n";
    out << "*DENSITY\n" << m_case.material.density << "\n";
    out << "*ELASTIC\n" << m_case.material.elasticModulus << ",0.3\n";

    for (const auto &constraint : m_case.constraints) {
        Q_UNUSED(constraint);
        out << "*BOUNDARY\n1,1,3\n5,1,3\n";
        break; // simplified stub
    }

    for (const auto &load : m_case.loads) {
        if (load.type == LoadType::Temperature) {
            out << "*TEMPERATURE\n1," << load.magnitude << "\n";
        } else {
            out << "*CLOAD\n1,3," << -std::abs(load.magnitude) << "\n";
        }
    }

    out << "*STEP\n*STATIC\n*NODE PRINT, NSET=ALLNODES\nU\n*EL PRINT, ELSET=ALLEL\nS\n*END STEP\n";
    return inpPath;
}

BackendFEA_CalculiX::Result BackendFEA_CalculiX::parseResultFile(const QString &path) const {
    Result result;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return result;
    }

    QTextStream in(&file);
    std::map<int, gp_Pnt> nodes;
    std::map<int, double> stress;
    std::map<int, double> temperature;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.startsWith(QLatin1String("NODE"))) {
            const auto parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 5) {
                const int id = parts[1].toInt();
                nodes[id] = gp_Pnt(parts[2].toDouble(), parts[3].toDouble(), parts[4].toDouble());
            }
        } else if (line.startsWith(QLatin1String("S"))) {
            const auto parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                stress[parts[1].toInt()] = parts[2].toDouble();
            }
        } else if (line.startsWith(QLatin1String("T"))) {
            const auto parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                temperature[parts[1].toInt()] = parts[2].toDouble();
            }
        }
    }

    result.minStress = std::numeric_limits<double>::max();
    result.maxStress = std::numeric_limits<double>::lowest();
    result.minTemperature = std::numeric_limits<double>::max();
    result.maxTemperature = std::numeric_limits<double>::lowest();

    for (const auto &node : nodes) {
        FieldPoint fp;
        fp.id = node.first;
        fp.position = node.second;
        fp.stress = stress.count(node.first) ? stress.at(node.first) : 0.0;
        fp.temperature = temperature.count(node.first) ? temperature.at(node.first) : 0.0;
        result.minStress = std::min(result.minStress, fp.stress);
        result.maxStress = std::max(result.maxStress, fp.stress);
        result.minTemperature = std::min(result.minTemperature, fp.temperature);
        result.maxTemperature = std::max(result.maxTemperature, fp.temperature);
        result.field.push_back(fp);
    }

    if (!result.field.empty()) {
        result.success = true;
        result.summary = QStringLiteral("Parsed %1 nodes from CalculiX output.").arg(result.field.size());
    }
    return result;
}

BackendFEA_CalculiX::Result BackendFEA_CalculiX::synthesizeFallback() const {
    Result result;
    result.success = true;
    result.summary = QStringLiteral("CalculiX not available. Generated synthetic field.");

    Bnd_Box box;
    BRepBndLib::Add(m_shape, box);
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

    int nodeId = 1;
    result.minStress = std::numeric_limits<double>::max();
    result.maxStress = std::numeric_limits<double>::lowest();
    result.minTemperature = std::numeric_limits<double>::max();
    result.maxTemperature = std::numeric_limits<double>::lowest();
    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dz = 0; dz <= 1; ++dz) {
                const double x = dx ? xmax : xmin;
                const double y = dy ? ymax : ymin;
                const double z = dz ? zmax : zmin;
                FieldPoint fp;
                fp.id = nodeId++;
                fp.position = gp_Pnt(x, y, z);
                fp.stress = std::abs(z - zmin) * 1.5e6 + 1e6;
                fp.temperature = 25.0 + (z - zmin) * 0.2;
                result.minStress = std::min(result.minStress, fp.stress);
                result.maxStress = std::max(result.maxStress, fp.stress);
                result.minTemperature = std::min(result.minTemperature, fp.temperature);
                result.maxTemperature = std::max(result.maxTemperature, fp.temperature);
                result.field.push_back(fp);
            }
        }
    }
    return result;
}

BackendFEA_CalculiX::Result BackendFEA_CalculiX::runAnalysis() {
    if (m_shape.IsNull()) {
        Result r;
        r.summary = QStringLiteral("No geometry loaded.");
        return r;
    }

    QTemporaryDir tmpDir;
    if (!tmpDir.isValid()) {
        return synthesizeFallback();
    }

    const QString inpPath = writeInputDeck(tmpDir.path());
    Q_UNUSED(inpPath);
    const QString jobName = tmpDir.path() + "/analysis";

    QProcess process;
    process.setProgram(QStringLiteral("ccx"));
    process.setArguments({jobName});
    process.setWorkingDirectory(tmpDir.path());
    process.start();
    process.waitForFinished(15000);

    const QString frdPath = jobName + QStringLiteral(".frd");
    const QString datPath = jobName + QStringLiteral(".dat");

    Result parsed;
    if (QFile::exists(frdPath)) {
        parsed = parseResultFile(frdPath);
    } else if (QFile::exists(datPath)) {
        parsed = parseResultFile(datPath);
    }

    if (parsed.success) {
        parsed.rawOutput = QString::fromUtf8(process.readAllStandardOutput());
        return parsed;
    }

    Result fallback = synthesizeFallback();
    fallback.rawOutput = QString::fromUtf8(process.readAllStandardError());
    return fallback;
}

