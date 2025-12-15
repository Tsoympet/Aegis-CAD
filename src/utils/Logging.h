#pragma once

#include <QString>

namespace Logging {
void init();
void info(const QString &msg);
void warn(const QString &msg);
}

