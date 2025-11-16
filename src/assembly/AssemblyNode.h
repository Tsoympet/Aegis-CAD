#pragma once
#include <string>

struct AssemblyNode
{
    int         id {0};
    int         parentId {0};
    std::string partId;
};
