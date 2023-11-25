#pragma once

#include <string>
#include <tuple>
#include <stack>
#include <optional>
#include <iostream>
#include <string_view>
#include <set>
#include "TreeNode.h"
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

class RegExprState
{
    public:
        RegExprState(std::string_view regExpr, size_t regExprIndex = 0):regExpr{regExpr},regExprIndex{regExprIndex}{}
        static std::vector<std::string_view> splitAlternatives(std::string_view str)
        {
            std::vector<std::string_view> results {};
            for(size_t index = 0, startNum = 0, parenNum = 0; index <= str.length(); index ++)
            {
                if (index == str.length() || (str.at(index) == '|' && parenNum == 0))
                {   
                    results.emplace_back(str.substr(startNum, index - startNum));
                    startNum = index + 1;
                }
                else if(str.at(index) == '(')
                {
                    auto ret {skipCloseParen({str, index})};
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
        static std::optional<size_t> skipStar(RegExprState regExprState)
        {
            std::string_view str {regExprState.getRegExprStr()};
            size_t curIndex {regExprState.getRegExprIndex()};
            if (str.at(curIndex) != ')' && str.at(curIndex) != '(' && str.at(curIndex) != '*' 
                    && curIndex + 1 < str.length() && str[curIndex + 1] == '*')
            {
                return curIndex + 2;
            }
            if (str.at(curIndex) == '(')
            {
                
                auto ret {skipCloseParen({str, curIndex})};
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
        static std::optional<size_t> skipCloseParen(RegExprState regExprState)
        {
            std::string_view str{regExprState.getRegExprStr()};
            size_t curIndex {regExprState.getRegExprIndex()};
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
        std::string_view getRegExprStr() const
        {
            return regExpr;
        }
        size_t getRegExprIndex() const
        {
            return regExprIndex;
        }
        RegExprState& setRegExprIndex(size_t index)
        {
            regExprIndex = index;
            return *this;
        }
        bool more() const
        {
            return regExprIndex < regExpr.length();
        }
        char getCurToken() const
        {
            return regExpr.at(regExprIndex);
        }
        private:
        std::string regExpr;
        size_t regExprIndex {0};
};

class TopDownRegExprParse
{
    public:
        TopDownRegExprParse(std::string curRegExpr)
        :inputState{curRegExpr}
        {
            
        }

        std::optional<std::shared_ptr<TreeNode>> parse()
        {
            try
            {
                auto [curNode, resIndex] {parseAlternativesExpr(inputState)};
                return curNode;
            }
            catch(const char* e)
            {
                std::cerr << e << '\n';
            }
            return std::nullopt;
        }

        std::tuple<std::shared_ptr<TreeNode>, size_t> parseAlternativesExpr(RegExprState origState)
        {
            auto alterSets {RegExprState::splitAlternatives(origState.getRegExprStr())};
            auto headNode {std::make_shared<TreeNode>("headNode")};
            auto prevNode {headNode};
            auto prevPrevNode {headNode};
            for(size_t index = 0; auto &alter : alterSets)
            {
                auto [curNode, resIndex] {parseMultiRegularExpr({alter})};
                if (curNode)
                {
                    prevPrevNode = prevNode;
                    prevNode = prevNode->addChild(std::make_shared<TreeNode>("|"));
                    prevNode->addChild(std::move(curNode));
                }
            }
            prevPrevNode->mergeLastGrandson();
            return {
                headNode->getChildSize() ? headNode->getChild(0) : nullptr,
                0
            };
        }

        std::tuple<std::shared_ptr<TreeNode>, size_t> parseMultiRegularExpr(RegExprState origState)
        {
            auto headNode {std::make_shared<TreeNode>("head")};
            auto prevNode {headNode};
            auto prevPrevNode {headNode};
            auto curState{origState};
            while(curState.more())
            {
                auto [curNode, resIndex] {parseMultiCompoundExpr(curState)};
                curState.setRegExprIndex(resIndex);
                if (curNode)
                {
                    prevPrevNode = prevNode;
                    prevNode = prevNode->addChild(std::make_shared<TreeNode>("."));
                    prevNode->addChild(std::move(curNode));
                }
            }
            prevPrevNode->mergeLastGrandson();
            return {
                headNode->getChildSize() ? headNode->getChild(0) : nullptr,
                curState.getRegExprIndex()
            };

        }

        std::tuple<std::shared_ptr<TreeNode>, size_t> parseMultiCompoundExpr(RegExprState origState)
        {
            auto afterStarIndex {RegExprState::skipStar(origState)};
            if(afterStarIndex.has_value())
            {
                return parseMultRepeatExpr(origState, afterStarIndex.value());
            }
            return parseMultSimpleExpr(origState);
        }

        std::tuple<std::shared_ptr<TreeNode>, size_t> parseMultRepeatExpr(RegExprState origState, size_t afterStarIndex)
        {
            auto headNode {std::make_shared<TreeNode>("*")};
            auto prevNode {headNode};
            auto curState{origState};

            auto [curNode, resIndex] {parseMultSimpleExpr(curState)};
            curState.setRegExprIndex(resIndex);
            if (curNode)
            {
                prevNode->addChild(std::move(curNode));
            }
            else
            {
                return {nullptr, afterStarIndex};
            }
            return {headNode, afterStarIndex};
        }

        std::tuple<std::shared_ptr<TreeNode>, size_t> parseMultSimpleExpr(RegExprState origState)
        {
            if (origState.getCurToken() == ')')
            {
                throw "parseMultSimpleExpr unmatch char :\')\'";
            }
            if (origState.getCurToken() != '(')
            {
                return {std::make_shared<TreeNode>(std::string(1, origState.getCurToken())), origState.getRegExprIndex() + 1};
            }
            auto afterCloseParenIndex {origState.skipCloseParen(origState)};
            if (afterCloseParenIndex.has_value())
            {
                size_t inCloseParenNum {afterCloseParenIndex.value() - origState.getRegExprIndex() - 2};
                decltype(origState) curState{origState.getRegExprStr().substr(origState.getRegExprIndex() + 1, inCloseParenNum)};
                auto [curNode, resIndex] {parseAlternativesExpr(curState)};
                return {curNode, afterCloseParenIndex.value()};
            }
            throw "parseMultSimpleExpr unreachable";
            return {nullptr, 0};
        }
    private:
        RegExprState inputState;
};