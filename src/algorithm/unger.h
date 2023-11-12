#pragma once
#include <optional>
#include <string_view>
#include <iostream>
#include "BasicNode.h"
#include "GramTable.h"
class unger 
{
    private:
        const static int INPUTMAXSIZE = 100;
        using HistoryNode = std::tuple<std::string_view, std::string_view>; 
        using MemoryRes = std::array<std::array<std::map<std::string_view, std::shared_ptr<BasicNode>> ,INPUTMAXSIZE>, INPUTMAXSIZE>;
        using MemoryIndex = std::tuple<int, int>;
    public:
        unger(GramTable &gramTable, std::string str);
        std::shared_ptr<BasicNode> parse();
    private:
        std::shared_ptr<BasicNode> _parse(std::string_view prodLeft, std::string_view curStr, MemoryIndex memoryIndex);
        GramTable::MinMap minMap;
        GramTable::FirstMap firstMap;
        MemoryRes memoryRes;
        std::set<HistoryNode> historyNodeSet;

        GramTable gramTable;
        std::map<std::string, GramTable::ProdRightAlts> &gramMap;
        std::string inputStr;
        std::string_view startSymbol {"S"};
};