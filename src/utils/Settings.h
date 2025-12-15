#pragma once

#include <QSettings>
#include <QString>

class Settings {
public:
    Settings();

    QVariant value(const QString &key, const QVariant &def = {}) const;
    void setValue(const QString &key, const QVariant &value);

private:
    QSettings m_settings;
};

