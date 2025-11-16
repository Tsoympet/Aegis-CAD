#include "ProjectIO.h"
#include <QFile>
#include <QTextStream>

bool ProjectIO::save(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f);
    out << "AegisCAD Project v1.0";
    return true;
}

bool ProjectIO::load(const QString& path)
{
    QFile f(path);
    return f.exists();
}
