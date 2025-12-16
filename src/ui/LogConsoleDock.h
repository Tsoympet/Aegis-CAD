#pragma once

#include "../utils/Logging.h"

#include <QDockWidget>
#include <QListWidget>

class LogConsoleDock : public QDockWidget {
    Q_OBJECT
public:
    explicit LogConsoleDock(const QString &title, QWidget *parent = nullptr);

private slots:
    void appendEntry(const Logging::Entry &entry);

private:
    void buildUi();
    void connectLogger();

    QListWidget *m_list{nullptr};
};

