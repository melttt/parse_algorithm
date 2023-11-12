#pragma once
#include <optional>
#include <string_view>
#include <iostream>
#include <memory>
#include "BasicNode.h"
#include "GramTable.h"
class CYK 
{
    
    public:
        CYK(GramTable &gramTable, std::string str);
        std::shared_ptr<BasicNode> parse();

        std::shared_ptr<BasicNode> buildTree(std::string_view curSymbol, int symbolStart, int symbolEnd);
    private:
        const static int INPUTMAXSIZE = 100;
        std::array<std::array<std::set<std::string_view>, INPUTMAXSIZE>,INPUTMAXSIZE> recogTable;
        GramTable gramTable;

        GramTable::BnfNoTermToTermMap bnfNoTermToTermMap;
        GramTable::BnfTwoTermToTermMap bnfTwoTermToTermMap;
        std::string inputStr;
        std::string_view startSymbol {"S"};
};