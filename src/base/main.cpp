#include <iostream>
#include <fstream>
#include <chrono>
#include "BasicNode.h"
#include "BTNode.h"
#include "GramTable.h"
#include "unger.h"
#include "CYK.h"
#include "RegExpr.h"
#include "NFA.h"
#include "DFA.h"
void argsHelpPrint()
{
    std::cout << "Usage: gen_parse_tree algorithm grammer file" << std::endl;
}
void printInputInfo(std::string curAlgorithm, std::string inputStr)
{
    std::cout << "cur parse algorithm : " << curAlgorithm << std::endl;
    std::cout << "cur input string: " << inputStr <<std::endl; 
}

std::tuple<std::string, std::vector<std::string>, std::string> 
initArgs(int argc, char *argv[])
{
    if (argc < 4 || argv[1] == nullptr || argv[2] == nullptr
        || argv[3] == nullptr)
    {
        argsHelpPrint();
        exit(1);
    }
    std::vector<std::string> inputGrammer;
    std::string inputStr;
    std::string curAlgorithm;
    curAlgorithm = argv[1];
    std::fstream gramFile;
    std::string line;
    gramFile.open(argv[2], std::ios::in);
    if (gramFile.is_open())
    {
        while(getline(gramFile, line))
        {
            inputGrammer.push_back(line);
        }
    }
    std::fstream inputFile;
    inputFile.open(argv[3], std::ios::in);
    if(inputFile.is_open())
    {
        getline(inputFile, inputStr);
    }
    return {curAlgorithm, inputGrammer, inputStr};
}

int main(int argc, char *argv[])
{
    std::cout.setf( std::ios_base::unitbuf );    
    auto [curAlgorithm, inputGrammer, inputStr] = initArgs(argc, argv);
    printInputInfo(curAlgorithm, inputStr);

    GramTable gramTable{inputGrammer};
    gramTable.print();
    if (curAlgorithm == "unger")
    {
        auto t1 = std::chrono::steady_clock::now();
        unger tmp(gramTable, inputStr);

        auto ret = tmp.parse();
        if (ret == nullptr)
        {
            std::cout << "error parser!" << std::endl;
            return 0;
        }
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "cost time : " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
        BTTree<BasicNode> printer(ret.get(), &BasicNode::getChildren, &BasicNode::getData);
        printer.print();
    }
    else if (curAlgorithm == "CYK")
    {
        auto t1 = std::chrono::steady_clock::now();
        CYK tmp(gramTable, inputStr);

        auto ret = tmp.parse();
        if (ret == nullptr)
        {
            std::cout << "error parser!" << std::endl;
            return 0;
        }
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "cost time : " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
        BTTree<BasicNode> printer(ret.get(), &BasicNode::getChildren, &BasicNode::getData);
        printer.print();
    } 
    else if (curAlgorithm == "REG")
    {
        TopDownRegExprRecognition tmp(inputStr, inputGrammer[0]);
        std::cout << tmp.recognition() << std::endl;
    }
    else if (curAlgorithm == "RegParse")
    {
        TopDownRegExprParse tmp(inputStr);
        auto ret = tmp.parse();
        if (ret == std::nullopt)
        {
            std::cout << "error parser!" << std::endl;
            return 0;
        }
        
        auto ret2 = TreeNodeToBasicNode(ret.value());
        std::cout << ConvertDotStr(ret.value()) << std::endl;
        
        if (ret.value())
        {
            BTTree<BasicNode> printer(ret2.get(), &BasicNode::getChildren, &BasicNode::getData);
            printer.print();

            NFA nfa{};
            if (nfa.buildNFA(ret.value()))
            {
                std::cout << "******************" << std::endl;
                std::cout << nfa.ConvertToDotStr() << std::endl;
                std::cout << "******************" << std::endl;
                DFA dfa(nfa);
                std::cout << dfa.ConvertToDotStr() << std::endl;
            }
            
            
        }
        else
        {
            std::cout << "empty tree\n";
        }
        
    }
    else
    {
        std::cout << "unsupport algorithm : " << curAlgorithm << std::endl;
    }

    return 0;
}