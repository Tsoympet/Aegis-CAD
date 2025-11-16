#pragma once
#include <vector>
#include <string>

struct ToolDef {
    std::string name;
};

class ToolLibrary
{
public:
    void addTool(const ToolDef& t) { m_tools.push_back(t); }
private:
    std::vector<ToolDef> m_tools;
};
