#pragma once

#include <string>
#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>

namespace Logging {

enum class Level { Info, Warning, Error };

struct Entry {
    QDateTime timestamp;
    Level level{Level::Info};
    QString message;

    QString levelName() const;
    QString formatted() const;
};

class LogStream : public QObject {
    Q_OBJECT
public:
    explicit LogStream(QObject *parent = nullptr);

    const QVector<Entry> &entries() const { return m_entries; }
    void append(const Entry &entry);

signals:
    void entryAdded(const Entry &entry);

private:
    QVector<Entry> m_entries;
};

void init();
void info(const std::string &msg);
void warn(const std::string &msg);
void info(const QString &msg);
void warn(const QString &msg);
void error(const QString &msg);

LogStream *stream();
QVector<Entry> recentEntries();
}

