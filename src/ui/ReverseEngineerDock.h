#pragma once

#include <QCheckBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QPointF>
#include <TopoDS_Shape.hxx>
#include <vector>
#include <TopoDS_Shape.hxx>

class ReverseEngineerDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ReverseEngineerDock(const QString &title, QWidget *parent = nullptr);

signals:
    void modelGenerated(const TopoDS_Shape &shape);

public slots:
    void triggerGenerate();
    void exportAsPart();
    void exportAsStep();

private slots:
    void chooseBlueprint();

private slots:
    void chooseBlueprint();

private:
    TopoDS_Shape generatePlaceholder() const;
    TopoDS_Shape generateFromBlueprint() const;
    TopoDS_Shape buildShapeFromContours(const std::vector<std::vector<QPointF>> &paths, double scale) const;
    std::vector<std::vector<QPointF>> detectContours(const QString &path) const;
    QStringList extractLabels(const QString &path) const;
    double inferScale(const QStringList &labels, const std::vector<std::vector<QPointF>> &paths) const;
    void updateProgress(int value) const;

    QPlainTextEdit *m_prompt{nullptr};
    QPushButton *m_generate{nullptr};
    QPushButton *m_exportPart{nullptr};
    QPushButton *m_exportStep{nullptr};
    QLineEdit *m_blueprintPath{nullptr};
    QPushButton *m_selectBlueprint{nullptr};
    QProgressBar *m_progress{nullptr};
    QCheckBox *m_previewToggle{nullptr};
    mutable TopoDS_Shape m_lastShape;
    mutable std::vector<std::vector<QPointF>> m_lastContours;
    QPlainTextEdit *m_prompt{nullptr};
    QPushButton *m_generate{nullptr};
    QPushButton *m_exportPart{nullptr};
    QPushButton *m_exportStep{nullptr};
    QLineEdit *m_blueprintPath{nullptr};
    QPushButton *m_selectBlueprint{nullptr};
    QProgressBar *m_progress{nullptr};
    QCheckBox *m_previewToggle{nullptr};
    mutable TopoDS_Shape m_lastShape;
    mutable std::vector<std::vector<QPointF>> m_lastContours;

    QPlainTextEdit *m_prompt{nullptr};
    QPushButton *m_generate{nullptr};
    QLineEdit *m_blueprintPath{nullptr};
    QPushButton *m_selectBlueprint{nullptr};
};

