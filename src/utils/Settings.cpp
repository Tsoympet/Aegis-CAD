#include "Settings.h"

Settings::Settings() : m_settings("AegisCAD", "AegisCAD") {}

QVariant Settings::value(const QString &key, const QVariant &def) const {
    return m_settings.value(key, def);
}

void Settings::setValue(const QString &key, const QVariant &value) {
    m_settings.setValue(key, value);
}

