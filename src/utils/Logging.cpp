#include "Logging.h"
#include <QDebug>

void Logging::info(const QString& msg)
{
    qInfo() << msg;
}
