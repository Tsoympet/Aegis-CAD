#pragma once

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <TopoDS_Shape.hxx>

class ReverseEngineerDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ReverseEngineerDock(const QString &title, QWidget *parent = nullptr);

signals:
    void modelGenerated(const TopoDS_Shape &shape);

public slots:
    void triggerGenerate();

private slots:
    void chooseBlueprint();

private:
    TopoDS_Shape generatePlaceholder() const;

    QPlainTextEdit *m_prompt{nullptr};
    QPushButton *m_generate{nullptr};
    QLineEdit *m_blueprintPath{nullptr};
    QPushButton *m_selectBlueprint{nullptr};
};

