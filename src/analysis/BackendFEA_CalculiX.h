#pragma once
#include <string>

class BackendFEA_CalculiX
{
public:
    bool runCase(const std::string& deckPath);
};
