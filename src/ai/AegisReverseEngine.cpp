#include "AegisReverseEngine.h"

ReverseResult AegisReverseEngine::runReverse(const ReverseSpec& spec)
{
    ReverseResult r;
    r.summary = QString("[reverse stub] %1 in domain %2 with %3 images")
        .arg(spec.objectName)
        .arg(spec.domain)
        .arg(spec.imageCount);
    return r;
}
