#pragma once

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QPushButton>

class ReverseEngineerDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ReverseEngineerDock(const QString &title, QWidget *parent = nullptr);

    QString promptText() const;

signals:
    void generateShapeRequested(const QString &prompt);

public slots:
    void triggerGenerate();

private:
    QPlainTextEdit *m_prompt{nullptr};
    QPushButton *m_generate{nullptr};
};

