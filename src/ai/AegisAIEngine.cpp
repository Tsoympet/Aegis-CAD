#include "AegisAIEngine.h"

AegisReply AegisAIEngine::respond(const AegisContext& ctx, const QString& userText)
{
    AegisReply r;
    r.text = "[AI stub] Context: " + ctx.summary + " | User: " + userText;
    return r;
}
