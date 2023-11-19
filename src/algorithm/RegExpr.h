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
    std::set<RegExprState> parseAlternativesExpr(RegExprState regExprState);
    bool recognition();
    std::set<RegExprState> parseMultiRegularExpr(std::set<RegExprState> origStates);
    std::set<RegExprState> parseMultiCompoundExpr(RegExprState origState);
    std::set<RegExprState> parseMultRepeatExpr(RegExprState origState);
    std::set<RegExprState> parseMultSimpleExpr(RegExprState origState);
    private:
    
    std::string inputStr;
    std::string regStr;

};
