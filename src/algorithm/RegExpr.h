#pragma once

#include <string>
#include <tuple>
#include <stack>
#include <optional>
#include <iostream>
#include <string_view>
#include <set>

class TopDownRegExprRecognition
{
    public:
    class RegExprState
    {
        public:
            std::string_view curRegExpr;
            std::string_view curStr;
            std::size_t curRegExprIndex;
            std::size_t curStrIndex;
            RegExprState(std::string_view curRegExpr, std::string_view curStr, std::size_t curRegExprIndex, std::size_t curStrIndex)
            :curRegExpr{curRegExpr}, curStr{curStr}, curRegExprIndex{curRegExprIndex}, curStrIndex{curStrIndex}{}

            bool more() const
            {
                if (curRegExprIndex < curRegExpr.length()) return true;
                return false;
            }
            bool operator==(const RegExprState &regExprState) const
            {
                if (this->curRegExpr == regExprState.curRegExpr &&
                    this->curStr == regExprState.curStr &&
                    this->curRegExprIndex == regExprState.curRegExprIndex &&
                    this->curStrIndex == regExprState.curStrIndex) return true;
                return false;
            }
            bool operator<(const RegExprState &regExprState) const
            {
                if (this->curRegExpr < regExprState.curRegExpr) return true;
                if (this->curRegExpr == regExprState.curRegExpr && this->curStr < regExprState.curStr)  return true;
                if (this->curRegExpr == regExprState.curRegExpr && this->curStr == regExprState.curStr
                    && this->curRegExprIndex < regExprState.curRegExprIndex) return true;
                if (this->curRegExpr == regExprState.curRegExpr && this->curStr == regExprState.curStr
                    && this->curRegExprIndex == regExprState.curRegExprIndex && this->curStrIndex < regExprState.curStrIndex) return true;
                return false;
            }
            std::optional<size_t> peekRegExprStar() const
            {
                size_t tmpIndex = curRegExprIndex;
                if (curRegExpr.at(tmpIndex) != ')' && curRegExpr.at(tmpIndex) != '(' && curRegExpr.at(tmpIndex) != '*' && tmpIndex + 1 < curRegExpr.length() 
                        && curRegExpr[tmpIndex + 1] == '*')
                {
                    return tmpIndex + 2;
                }
                if (curRegExpr.at(tmpIndex) == '(')
                {
                    size_t bracketNum = 1;
                    while(++ tmpIndex < curRegExpr.length())
                    {
                        if (curRegExpr.at(tmpIndex) == '(') bracketNum ++;
                        if (curRegExpr.at(tmpIndex) == ')')
                        {
                            if (-- bracketNum == 0)
                            {        
                                break;
                            }
                        }
                    }
                    if (++ tmpIndex < curRegExpr.length() && curRegExpr.at(tmpIndex) == '*')
                    {
                        return tmpIndex + 1;
                    }
                }
                return std::nullopt;
            }
            void setCurRegExprIndex(std::size_t index)
            {
                curRegExprIndex = index;
            }
            void setCurStrIndex(std::size_t index)
            {
                curStrIndex = index;
            }
            bool eatInputToken()
            {
                if (curRegExpr[curRegExprIndex] == curStr[curStrIndex])
                {
                    curStrIndex ++;
                    curRegExprIndex ++;
                    return true;
                }
                return false;
            }
            bool eatToken(char token)
            {
                if (curRegExpr[curRegExprIndex] == token)
                {
                    curRegExprIndex ++;
                    return true;
                }
                return false;

            }
            std::optional<size_t> peekRegExprCloseParen() const
            {
                auto tmpIndex = curRegExprIndex;
                if (curRegExpr.at(tmpIndex) == '(')
                {
                    size_t bracketNum = 1;
                    while(++ tmpIndex < curRegExpr.length())
                    {
                        if (curRegExpr.at(tmpIndex) == '(') bracketNum ++;
                        if (curRegExpr.at(tmpIndex) == ')')
                        {
                            if (-- bracketNum == 0)
                            {        
                                return tmpIndex - curRegExprIndex - 1;
                            }
                        }
                    }
                }

                return std::nullopt;
            }
            std::set<RegExprState> splitAlternatives() 
            {
                std::set<RegExprState> resultStates {};
                for(size_t index = 0, startNum = 0, parenNum = 0; index <= curRegExpr.length(); index ++)
                {
                    if (index == curRegExpr.length() || (curRegExpr.at(index) == '|' && parenNum == 0))
                    {   
                        resultStates.insert({this->curRegExpr.substr(startNum, index - startNum), this->curStr, 0, this->curStrIndex});
                        startNum = index + 1;
                    }
                    else if(curRegExpr.at(index) == '(')
                    {
                        parenNum ++;
                    }
                    else if(curRegExpr.at(index) == ')')
                    {
                        parenNum --;
                    }
                }
                return resultStates;
            }
            void printStrInfo() const
            {
                
                std::cout << curStr << "  | " << curRegExpr << std::endl;
                for(decltype(inputStr.size()) i = 0 ; i <= curStr.length(); i ++)
                {
                    if (i != curStrIndex)
                    {
                        std::cout << " ";
                    }
                    else
                    {
                        std::cout << "^";
                    }
                }
                std::cout << " | ";

                for(decltype(regStr.size()) i = 0 ; i <= curRegExpr.length(); i ++)
                {
                    if (i != curRegExprIndex)
                    {
                        std::cout << " ";
                    }
                    else
                    {
                        std::cout << "^";
                    }
                }
                std::cout << std::endl;

            }
    };

    public:
    TopDownRegExprRecognition(std::string inputStr, std::string regStr);
    bool recognition();
    std::set<RegExprState> parseAlternativesExpr(RegExprState regExprState);
    std::set<RegExprState> parseMultiRegularExpr(std::set<RegExprState> origStates);
    std::set<RegExprState> parseMultiCompoundExpr(RegExprState origState);
    std::set<RegExprState> parseMultRepeatExpr(RegExprState origState);
    std::set<RegExprState> parseMultSimpleExpr(RegExprState origState);
    private:
    
    std::string inputStr;
    std::string regStr;

};

#include "BasicNode.h"
class RegExprState
{
    public:
        RegExprState(std::string_view regExpr):regExpr{regExpr},regExprIndex{0}{}
        static std::set<std::string_view> splitAlternatives(std::string_view str)
        {
            std::set<std::string_view> results {};
            for(size_t index = 0, startNum = 0, parenNum = 0; index <= str.length(); index ++)
            {
                if (index == str.length() || (str.at(index) == '|' && parenNum == 0))
                {   
                    results.insert(str.substr(startNum, index - startNum));
                    startNum = index + 1;
                }
                else if(str.at(index) == '(')
                {
                    auto ret {skipCloseParen(str, index)};
                    if (ret.has_value())
                    {
                        index = ret.value() - 1;
                    }
                    else
                    {
                        return {};
                    }
                }
            }
            return results;
        }
        static std::optional<size_t> skipStar(std::string_view str, size_t curIndex)
        {
            if (str.at(curIndex) != ')' && str.at(curIndex) != '(' && str.at(curIndex) != '*' 
                    && curIndex + 1 < str.length() && str[curIndex + 1] == '*')
            {
                return curIndex + 2;
            }
            if (str.at(curIndex) == '(')
            {
                auto ret {skipCloseParen(str, curIndex)};
                if (ret.has_value())
                {
                    curIndex = ret.value();
                    if (curIndex < str.length() && str.at(curIndex) == '*')
                    {
                        return curIndex + 1;
                    }
                }
            }
            return std::nullopt;
        }
        static std::optional<size_t> skipCloseParen(std::string_view str, size_t curIndex)
        {
            if (str.at(curIndex) == '(')
            {
                size_t bracketNum = 1;
                while(++ curIndex < str.length())
                {
                    if (str.at(curIndex) == '(') bracketNum ++;
                    else if (str.at(curIndex) == ')' && -- bracketNum == 0)
                    {
                        return curIndex + 1;//tmpIndex - index - 1;
                    }
                }
            }
            return std::nullopt;
        }
        private:
        std::string_view regExpr;
        size_t regExprIndex {0};
};

class TopDownRegExprParse
{
    public:
    TopDownRegExprParse(std::string curRegExpr)
    :curRegExpr{curRegExpr}, curRegExprIndex{0}
    {

    }

    std::shared_ptr<BasicNode> parse()
    {
        auto [curNode, resIndex] {parseAlternativesExpr(curRegExpr, 0)};
        return curNode;
    }

    std::tuple<std::shared_ptr<BasicNode>, size_t> parseAlternativesExpr(std::string_view str, size_t strIndex)
    {
        auto alterSets {splitAlternatives(str)};
        auto headNode {std::make_shared<BasicNode>("head")};
        auto prevNode {headNode};
        auto ppNode{headNode};
        for(auto &alter : alterSets)
        {
            auto [curNode, resIndex] {parseMultiRegularExpr(alter, 0)};
            if (curNode != nullptr)
            {
                auto tmp {std::make_shared<BasicNode>("|")};
                prevNode->addChildren(tmp);
                ppNode = prevNode;
                prevNode = tmp;
                prevNode->addChildren(curNode);
            }
            else
            {
                std::cout << "error parseAlternativesExpr\n";
                return {nullptr, 0};
            }
        }
        if (headNode->getChildren().size() == 0)
            return {nullptr, 0};

        {
            auto lastNode {prevNode->getChildrenByIndex(0)};
            ppNode->clearChildren();
            ppNode->addChildren(lastNode);
        }
        
        return {headNode->getChildrenByIndex(0), 0};
    }

    std::tuple<std::shared_ptr<BasicNode>, size_t> parseMultiRegularExpr(std::string_view str, size_t strIndex)
    {
        auto headNode {std::make_shared<BasicNode>("head")};
        auto prevNode {headNode};
        auto ppNode{headNode};
        size_t curIndex{strIndex};
        while(curIndex < str.length())
        {
            auto [curNode, resIndex] {parseMultiCompoundExpr(str, curIndex)};
            curIndex = resIndex;
            if (curNode != nullptr)
            {
                auto tmp {std::make_shared<BasicNode>(".")};
                prevNode->addChildren(tmp);
                ppNode = prevNode;
                prevNode = tmp;
                prevNode->addChildren(curNode);
            }
            else
            {
                //throw "parseMultiRegularExpr error!";
                std::cout << "parseMultiRegularExpr exception!\n";
                break;
            }
        }
        if (headNode->getChildren().size() == 0)
            return {nullptr, curIndex};
        {
            auto lastNode {prevNode->getChildrenByIndex(0)};
            ppNode->clearChildren();
            ppNode->addChildren(lastNode);
        }
        
        return {headNode->getChildrenByIndex(0), curIndex};
    }

    std::tuple<std::shared_ptr<BasicNode>, size_t> parseMultiCompoundExpr(std::string_view str, size_t strIndex)
    {
        auto ret {peekRegExprStar(str, strIndex)};
        if(ret.has_value())
        {
            return parseMultRepeatExpr(str, strIndex, ret.value());
        }
        return parseMultSimpleExpr(str, strIndex);
    }

    std::tuple<std::shared_ptr<BasicNode>, size_t> parseMultRepeatExpr(std::string_view str, size_t strIndex, size_t afterStarIndex)
    {
        auto headNode {std::make_shared<BasicNode>("*")};
        auto prevNode {headNode};
        size_t curIndex{strIndex};
        while(curIndex < afterStarIndex - 1)
        {

            auto [curNode, resIndex] {parseMultSimpleExpr(str, strIndex)};
            curIndex = resIndex;
            if (curNode != nullptr)
            {
                prevNode->addChildren(curNode);
            }
            else
            {
                std::cout << "parseMultRepeatExpr exception!\n";
                break;
            }
        }
        if (headNode->getChildren().size() == 0)
            return {nullptr, curIndex};
        return {headNode, curIndex + 1};
    }

    std::tuple<std::shared_ptr<BasicNode>, size_t> parseMultSimpleExpr(std::string_view str, size_t strIndex)
    {
        if (str[strIndex] == ')')
        {
            return {nullptr, 0};
        }
        if (str[strIndex] != '(')
        {
            return {std::make_shared<BasicNode>(std::string(1, str[strIndex])), strIndex + 1};
        }
        auto inCloseNum {peekRegExprCloseParen(str, strIndex)};
        if (inCloseNum.has_value())
        {
            std::cout << "(*******************)" << std::endl;
            std::cout << str.substr(strIndex+ 1, inCloseNum.value()) << std::endl;
            auto [curNode, resIndex] {parseAlternativesExpr(str.substr(strIndex + 1, inCloseNum.value()), 0)};
            if (curNode != nullptr)
            {
                return {curNode, strIndex + inCloseNum.value() + 2};
            }
        }
        return {nullptr, 0};
    }
    private:
    std::set<std::string_view> splitAlternatives(std::string_view inputStr) 
    {
        std::set<std::string_view> results {};
        for(size_t index = 0, startNum = 0, parenNum = 0; index <= inputStr.length(); index ++)
        {
            if (index == inputStr.length() || (inputStr.at(index) == '|' && parenNum == 0))
            {   
                results.insert(inputStr.substr(startNum, index - startNum));
                startNum = index + 1;
            }
            else if(inputStr.at(index) == '(')
            {
                parenNum ++;
            }
            else if(inputStr.at(index) == ')')
            {
                parenNum --;
            }
        }
        return results;
    }
    std::optional<size_t> peekRegExprStar(std::string_view str, size_t strIndex) const
    {
        size_t tmpIndex = strIndex;
        if (str.at(tmpIndex) != ')' && str.at(tmpIndex) != '(' && str.at(tmpIndex) != '*' && tmpIndex + 1 < str.length() 
                && str[tmpIndex + 1] == '*')
        {
            return tmpIndex + 2;
        }
        if (str.at(tmpIndex) == '(')
        {
            size_t bracketNum = 1;
            while(++ tmpIndex < str.length())
            {
                if (str.at(tmpIndex) == '(') bracketNum ++;
                if (str.at(tmpIndex) == ')')
                {
                    if (-- bracketNum == 0)
                    {        
                        break;
                    }
                }
            }
            if (++ tmpIndex < str.length() && str.at(tmpIndex) == '*')
            {
                return tmpIndex + 1;
            }
        }
        return std::nullopt;
    }
    std::optional<size_t> peekRegExprCloseParen(std::string_view str, size_t strIndex) const
    {
        auto tmpIndex = strIndex;
        if (str.at(tmpIndex) == '(')
        {
            size_t bracketNum = 1;
            while(++ tmpIndex < str.length())
            {
                if (str.at(tmpIndex) == '(') bracketNum ++;
                if (str.at(tmpIndex) == ')')
                {
                    if (-- bracketNum == 0)
                    {        
                        return tmpIndex - strIndex - 1;
                    }
                }
            }
        }

        return std::nullopt;
    }
    std::string curRegExpr;
    size_t curRegExprIndex;
};