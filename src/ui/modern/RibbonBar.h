#pragma once
#include <QTabWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMap>
#include <QWidget>
#include <QIcon>
#include <QString>
#include <QPointer>

// Professional SolidWorks-style Ribbon Bar with contextual tabs
class RibbonBar : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonBar(QWidget* parent = nullptr);
    ~RibbonBar() override = default;

    // Contextual tab management
    void showContextTab(const QString& name);
    void hideContextTab(const QString& name);

signals:
    void actionTriggered(const QString& actionName);

private:
    QTabWidget* m_tabWidget = nullptr;
    QMap<QString, QWidget*> m_contextTabs;

    QWidget* createSketchTab();
    QWidget* createModelTab();
    QWidget* createInspectTab();
    QWidget* createAITab();
    QWidget* createAnalysisTab();

    QToolButton* makeButton(const QString& iconPath, const QString& text, const QString& tooltip);
};
