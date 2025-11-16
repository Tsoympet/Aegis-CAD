#pragma once
#include <QString>

struct ReverseSpec {
    QString objectName;
    QString domain;
    QString textInfo;
    int     imageCount {0};
};

struct ReverseResult {
    QString summary;
};

class AegisReverseEngine
{
public:
    ReverseResult runReverse(const ReverseSpec& spec);
};
