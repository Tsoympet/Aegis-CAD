#include "LogConsoleDock.h"

#include <QAbstractItemView>
#include <QVBoxLayout>

LogConsoleDock::LogConsoleDock(const QString &title, QWidget *parent)
    : QDockWidget(title, parent) {
    buildUi();
    connectLogger();
}

void LogConsoleDock::buildUi() {
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(4, 4, 4, 4);

    m_list = new QListWidget(container);
    m_list->setSelectionMode(QAbstractItemView::NoSelection);
    m_list->setUniformItemSizes(true);
    layout->addWidget(m_list);

    container->setLayout(layout);
    setWidget(container);
}

void LogConsoleDock::connectLogger() {
    auto *logger = Logging::stream();
    for (const auto &entry : logger->entries()) {
        appendEntry(entry);
    }
    connect(logger, &Logging::LogStream::entryAdded, this, &LogConsoleDock::appendEntry);
}

void LogConsoleDock::appendEntry(const Logging::Entry &entry) {
    if (!m_list) return;
    m_list->addItem(entry.formatted());
    m_list->scrollToBottom();
}

