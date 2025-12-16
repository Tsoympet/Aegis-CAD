#include "Logging.h"

#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>

namespace {
using Logging::Entry;
using Logging::Level;
using Logging::LogStream;

QString levelToString(Level level) {
    switch (level) {
    case Level::Warning:
        return QStringLiteral("WARN");
    case Level::Error:
        return QStringLiteral("ERROR");
    case Level::Info:
    default:
        return QStringLiteral("INFO");
    }
}

Level typeToLevel(QtMsgType type) {
    switch (type) {
    case QtWarningMsg:
        return Level::Warning;
    case QtCriticalMsg:
    case QtFatalMsg:
        return Level::Error;
    case QtInfoMsg:
    case QtDebugMsg:
    default:
        return Level::Info;
    }
}

QMutex &streamMutex() {
    static QMutex mutex;
    return mutex;
}

LogStream *&globalStream() {
    static LogStream *stream = nullptr;
    return stream;
}

void appendEntry(const Entry &entry) {
    QMutexLocker locker(&streamMutex());
    if (!globalStream()) {
        globalStream() = new LogStream();
    }
    globalStream()->append(entry);

    QTextStream out(entry.level == Level::Error ? stderr : stdout);
    out << entry.formatted() << Qt::endl;
}

void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg) {
    Entry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = typeToLevel(type);
    entry.message = msg.trimmed();
    appendEntry(entry);
}
} // namespace

namespace Logging {

QString Entry::levelName() const { return levelToString(level); }

QString Entry::formatted() const { return QStringLiteral("%1 [%2] %3").arg(timestamp.toString("HH:mm:ss"), levelName(), message); }

LogStream::LogStream(QObject *parent) : QObject(parent) {}

void LogStream::append(const Entry &entry) {
    m_entries.append(entry);
    emit entryAdded(entry);
}

void init() {
    qInstallMessageHandler(messageHandler);
    info(QStringLiteral("Logging initialized"));
}

void info(const QString &msg) { qInfo().noquote() << msg; }

void warn(const QString &msg) { qWarning().noquote() << msg; }

void error(const QString &msg) { qCritical().noquote() << msg; }

LogStream *stream() {
    QMutexLocker locker(&streamMutex());
    if (!globalStream()) {
        globalStream() = new LogStream();
    }
    return globalStream();
}

QVector<Entry> recentEntries() {
    QMutexLocker locker(&streamMutex());
    if (!globalStream()) {
        return {};
    }
    return globalStream()->entries();
}

} // namespace Logging

