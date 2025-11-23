#include "AegisAssistantDock.h"
#include "ai/AegisAIEngine.h"
#include "ai/AegisReverseEngine.h"

#include <QHBoxLayout>
#include <QScrollBar>

AegisAssistantDock::AegisAssistantDock(QWidget* parent)
    : QDockWidget("Aegis Assistant", parent),
      m_ai(std::make_unique<AegisAIEngine>()),
      m_rev(std::make_unique<AegisReverseEngine>())
{
    auto* mainWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(mainWidget);
    m_console = new QTextEdit(mainWidget);
    m_console->setReadOnly(true);
    m_input = new QLineEdit(mainWidget);
    m_sendBtn = new QPushButton("Send", mainWidget);
    m_contextLabel = new QLabel("No selection", mainWidget);

    auto* inputRow = new QHBoxLayout();
    inputRow->addWidget(m_input);
    inputRow->addWidget(m_sendBtn);

    layout->addWidget(new QLabel("AI Assistant Console"));
    layout->addWidget(m_contextLabel);
    layout->addWidget(m_console);
    layout->addLayout(inputRow);
    mainWidget->setLayout(layout);
    setWidget(mainWidget);

    connectSignals();
}

AegisAssistantDock::~AegisAssistantDock() = default;

void AegisAssistantDock::connectSignals()
{
    connect(m_sendBtn, &QPushButton::clicked, this, &AegisAssistantDock::onSendClicked);
    connect(m_ai.get(), &AegisAIEngine::logMessage, this, &AegisAssistantDock::handleLog);
    connect(m_rev.get(), &AegisReverseEngine::logMessage, this, &AegisAssistantDock::handleLog);
}

void AegisAssistantDock::updateContext(const QString& name, const QString& material, const QString& type)
{
    m_currentContext.name = name;
    m_currentContext.material = material;
    m_currentContext.type = type;
    m_currentContext.mass = 0.0;
    m_currentContext.volume = 0.0;
    m_currentContext.stress = 0.0;
    m_currentContext.fos = 0.0;

    m_contextLabel->setText(QString("🧩 Selected: %1 | %2 | %3")
                            .arg(name, material, type));
    m_console->append(QString("[Context] Active part: %1").arg(name));
    m_ai->setCurrentPart(m_currentContext);
}

void AegisAssistantDock::onSendClicked()
{
    const QString cmd = m_input->text().trimmed();
    if (cmd.isEmpty()) return;
    m_console->append("> " + cmd);
    m_input->clear();

    QString result;
    if (cmd.startsWith("reverse"))
    {
        const QString img = cmd.section(' ', 1, 1);
        const QString res = m_rev->analyzeImage(img);
        const QString textHint = m_rev->runPythonFusion("auto", cv::Mat());
        result = "🔍 Reverse Result: " + res + "\n" + textHint;
    }
    else
    {
        // Forward part context for explicit part-related commands
        bool refersToPart = cmd.contains("selected", Qt::CaseInsensitive)
                            || cmd.contains("this part", Qt::CaseInsensitive)
                            || cmd.contains("current part", Qt::CaseInsensitive);

        if (refersToPart && m_currentContext.isValid())
            m_ai->setCurrentPart(m_currentContext);

        result = m_ai->processCommand(cmd);
    }

    m_console->append("🤖 " + result);
    m_console->verticalScrollBar()->setValue(m_console->verticalScrollBar()->maximum());
}

void AegisAssistantDock::handleLog(const QString& msg)
{
    m_console->append("[Log] " + msg);
}
