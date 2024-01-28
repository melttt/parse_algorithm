#pragma once
#include "GramTable.h"
#include <string>
#include <map>
#include <vector>
#include "BasicNode.h"
#include "BTNode.h"
namespace PARSE_UTIL
{



class StrongLLk
{
    public:
        using LLTable = std::map<std::string, std::map<std::string, std::set<size_t>>>;
        StrongLLk() = delete;
        StrongLLk(GramTable &gramTable, std::string inputStr, size_t k, bool verbose = true);
        std::shared_ptr<BasicNode> parse();
        void printLLkTable();
    private:
        void __parse(std::vector<std::shared_ptr<BasicNode>>& children, size_t &inputStrIndex);
        GramTable &gramTable;
        std::string inputStr;
        size_t lookAheadSize;
        LLTable lltable;
        bool isConflict;
        bool verbose;
};


}