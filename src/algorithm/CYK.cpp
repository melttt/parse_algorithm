#include "CYK.h"
CYK::CYK(GramTable &gramTable, std::string str):
gramTable(gramTable), inputStr(str)
{
    if (inputStr.length() > INPUTMAXSIZE)
    {
        throw "unger can't handle the strings exceeding 100 in length!";
    }
}

std::shared_ptr<BasicNode> CYK::parse()
{
    auto bnfInfo {gramTable.getBNFInfo()};
    if (!bnfInfo.has_value())
    {
        return nullptr;
    }
    auto &[tbnfNoTermToTermMap, tbnfTwoTermToTermMap] {bnfInfo.value()};
    bnfNoTermToTermMap = tbnfNoTermToTermMap;
    bnfTwoTermToTermMap = tbnfTwoTermToTermMap;

    for(decltype(inputStr.length()) i = 0 ; i < inputStr.length() ; i ++)
    {
        auto curStr {std::string(1, inputStr[i])};
        if(bnfNoTermToTermMap.contains(curStr))
        {
            auto mergedTmp{bnfNoTermToTermMap[curStr]};
            recogTable[i][i].merge(mergedTmp);
        }
        else
        {
            return nullptr;
        }
    }
    for (decltype(inputStr.length()) subStrLen = 1 ;            
                            subStrLen < inputStr.length() ; subStrLen ++)
    {
        for(decltype(inputStr.length()) subleftStrStart = 0 ; 
                            subleftStrStart < inputStr.length() - subStrLen ; subleftStrStart ++)
        {
            for(decltype(inputStr.length()) subleftStrLen = 0; 
                            subleftStrLen < subStrLen; subleftStrLen ++)
            {
                const auto &leftTermSet = recogTable[subleftStrStart][subleftStrStart + subleftStrLen];
                const auto &rightTermSet = recogTable[subleftStrStart + subleftStrLen + 1][subleftStrStart + subStrLen];
                for(const auto &leftTerm : leftTermSet)
                {
                    for(const auto &rightTerm : rightTermSet)
                    {
                        if (bnfTwoTermToTermMap.contains({leftTerm, rightTerm}))
                        {   
                            auto mergedTmp{bnfTwoTermToTermMap[{leftTerm, rightTerm}]};
                            recogTable[subleftStrStart][subleftStrStart + subStrLen].merge(mergedTmp);
                        }
                    }
                }
            }
        }
    }
    
    if (!recogTable[0][inputStr.length() - 1].contains(startSymbol))
    {
        return nullptr;
    }
    return buildTree(startSymbol, 0, inputStr.length() - 1);
}

std::shared_ptr<BasicNode> CYK::buildTree(std::string_view curSymbol, int symbolStart, int symbolEnd)
{
    std::shared_ptr<BasicNode> res {std::make_shared<BasicNode>(std::string(curSymbol))};
    if (symbolStart == symbolEnd)
    {
        return res;
    }
    for(int splitIndex = symbolStart; splitIndex < symbolEnd ; splitIndex ++)
    {
        for(auto leftSymbol : recogTable[symbolStart][splitIndex])
        {
            for(auto rightSymbol : recogTable[splitIndex + 1][symbolEnd])
            {
                if (bnfTwoTermToTermMap.contains({leftSymbol,rightSymbol}))
                {
                    std::shared_ptr<BasicNode> leftRes = buildTree(leftSymbol, symbolStart, splitIndex);
                    std::shared_ptr<BasicNode> rightRes = buildTree(rightSymbol, splitIndex + 1, symbolEnd);
                    if (leftRes && rightRes)
                    {
                        res->addChildren(leftRes);
                        res->addChildren(rightRes);
                        return res;
                    }
                }
            }
        }
    }
    return nullptr;
}