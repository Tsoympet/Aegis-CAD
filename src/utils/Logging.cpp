#include "Logging.h"
#include <QDebug>
#include <QDateTime>

namespace Logging {

void init() {
    info("Logging initialized");
}

void info(const QString &msg) {
    qInfo().noquote() << QDateTime::currentDateTime().toString("HH:mm:ss") << "[INFO]" << msg;
}

void warn(const QString &msg) {
    qWarning().noquote() << QDateTime::currentDateTime().toString("HH:mm:ss") << "[WARN]" << msg;
}

} // namespace Logging

