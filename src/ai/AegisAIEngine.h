#pragma once
#include <QString>

struct AegisContext {
    QString summary;
};

struct AegisReply {
    QString text;
};

class AegisAIEngine
{
public:
    AegisReply respond(const AegisContext& ctx, const QString& userText);
};
