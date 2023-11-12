#include "GramTable.h"
GramTable::GramTable(std::vector<std::string> gramLines)
{
    for(const auto& line : gramLines)
    {
        parseGramLine(line);
    }

    for(auto &[prodLeft, prodRightAlts] : grammerMap)
    {
        for(auto &prodRightAlt : prodRightAlts)
        {
            for(auto &rightItem : prodRightAlt)
            {
                if (isTerm(rightItem))
                {
                    termSet.insert(rightItem);
                }
                else
                {
                    noTermSet.insert(rightItem);
                }
                
            }
            
        }
    }

    
}       
std::optional<std::tuple<GramTable::BnfNoTermToTermMap, GramTable::BnfTwoTermToTermMap>> GramTable::getBNFInfo()
{
    BnfNoTermToTermMap bnfNoTermToTermMap;
    BnfTwoTermToTermMap bnfTwoTermToTermMap;
    //if BNF
    for(auto &[prodLeft, prodRightAlts] : grammerMap)
    {
        for(auto &prodRightAlt : prodRightAlts)
        {

            
            if (prodRightAlt.size() == 1 && isTerm(prodRightAlt[0]) && !prodRightAlt[0].empty())
            {
                bnfNoTermToTermMap[prodRightAlt[0]].insert(prodLeft);
            }
            else if(prodRightAlt.size() == 2 && !isTerm(prodRightAlt[0]) && !isTerm(prodRightAlt[1]))
            {
                std::tuple<std::string_view, std::string_view> curRight{prodRightAlt[0], prodRightAlt[1]};
                bnfTwoTermToTermMap[curRight].insert(prodLeft);
            }
            else
            {
                return std::nullopt;
            }
        }
    }

    for (auto [key, value] : bnfTwoTermToTermMap)
    {
        std::cout << "key: {" <<std::get<0>(key) <<" " << std::get<1>(key) <<" } : ";
        for(auto val : value)
        {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    return {{bnfNoTermToTermMap, bnfTwoTermToTermMap}};
}
void GramTable::print() const
{
    for(auto &[left, rightAlts] : grammerMap)
    {
        std::cout << left << ":";
        auto first = true;
        for(const auto &rightAlt : rightAlts)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                std::cout << "|";
            }
            for(const auto& rightAltItem : rightAlt)
            {
                if (rightAltItem.empty())
                {
                    std::cout << '#';
                }
                else
                {
                    std::cout << rightAltItem;
                }
            }
            
        }
        std::cout << std::endl;
    }
}
void GramTable::parseGramLine(std::string_view line)
{
    auto colonIdx {line.find(":")};
    std::string prodLeft {line.substr(0, colonIdx)};
    auto prodRight {line.substr(colonIdx + 1, line.length())};
    auto verticalIdx {prodRight.find("|")};
    auto getProdRight = [](std::string_view str){
        std::vector<std::string> res;
        if (str.empty())
        {
            res.push_back("");
        }
        else
        {
            for(const auto &c : str)
            {
                res.push_back(std::string(1,c));
            }
        }
        return res;
    };
    while(verticalIdx != std::string_view::npos)
    {
        grammerMap[prodLeft].push_back(getProdRight(prodRight.substr(0, verticalIdx)));   
        prodRight = prodRight.substr(verticalIdx + 1, prodRight.length());
        verticalIdx = prodRight.find("|");
    }
    grammerMap[prodLeft].push_back(getProdRight(prodRight));
}
bool GramTable::isTerm(std::string_view str)
{
    if (str.length() == 0 ||
        (str.length() == 1 && (str[0] < 'A' || str[0] > 'Z')))
        return true;
    return false;
}

bool GramTable::calcMinMap(MinMap &minMap)
{
    bool change = false; 
    for(auto &[prodLeft, prodRightAlts] : grammerMap)
    {
        for(auto &prodRightAlt : prodRightAlts)
        {
            auto rightMinsum = 0;
            for(auto &rightItem : prodRightAlt)
            {
                if (isTerm(rightItem))
                {
                    rightMinsum += rightItem.length();
                    if (minMap.find(rightItem) == minMap.end())
                    {
                        minMap[rightItem] = rightItem.length();
                        change = true;
                    }
                }
                else
                {
                    rightMinsum += minMap[rightItem];
                }
            }
            if (rightMinsum < minMap[prodLeft])
            {
                change = true;
                minMap[prodLeft] = rightMinsum;
            }
        }
    }
    return change;
}
GramTable::MinMap GramTable::getMinMap(int defaultMaxSymbol)
{
    MinMap res;
    for(auto &[left, right] : grammerMap)
    {
        res[left] = defaultMaxSymbol;
    }

    while(calcMinMap(res));
    for(auto &[left, right] : res)
    {
        std::cout << "noTerm : " << left << " min_num : " << right << std::endl;
    }
    return  res;
}

bool GramTable::calcFirstMap(FirstMap &firstMap)
{
    bool change = false; 
    for(auto &[prodLeft, prodRightAlts] : grammerMap)
    {
        auto &leftSet = firstMap[prodLeft];
        for(auto &prodRightAlt : prodRightAlts)
        {
            auto hasNullSymbol = false;
            for(auto &rightItem : prodRightAlt)
            {
                if (isTerm(rightItem))
                {
                    if (!leftSet.contains(rightItem))
                    {
                        change = true;
                        leftSet.insert(rightItem);
                    }

                    hasNullSymbol = false;
                }
                else
                {
                    auto itemSet = firstMap[rightItem];
                    hasNullSymbol = itemSet.find(nullSymbol) != itemSet.end();
                    std::remove_reference<decltype(itemSet)>::type unionSet {leftSet};
                    itemSet.erase(nullSymbol);
                    unionSet.merge(itemSet);
                    if (unionSet != leftSet)
                    {
                        change = true;
                        leftSet = unionSet;
                    }
                }
                if (!hasNullSymbol)
                {
                    break;
                }
            }
            if (hasNullSymbol && leftSet.find(nullSymbol) == leftSet.end())
            {
                change = true;
                leftSet.insert(nullSymbol);
            }
        }
    }
    return change;
}
GramTable::FirstMap GramTable::getFirstMap()
{
    FirstMap firstMap;
    for(auto term : termSet)
    {
        firstMap[std::string(term)].insert(std::string(term));
    }
    while(calcFirstMap(firstMap));
    for(auto &[left, right] : firstMap)
    {
        std::cout << "noTerm (" << left <<") : ";
        for(auto &subRight : right)
        {
            std::cout << (subRight.empty() ? "#" : subRight) << " ";
        }
        std::cout << std::endl;
    }
    return firstMap;
}
void GramTable::calcPartition(const std::string_view &str, int num, PartitionTable &res, PartitionTableItem &resItem,
        const std::vector<int> &minVec, const std::vector<std::set<std::string>> &firstVec)
{
    
    if (num == 0)
    {
        if (str.empty())
        {
            res.push_back(resItem);
        }
        return;
    }
 
    for(int i = minVec.size() ? minVec[num - 1] : 0; i <= str.length() ; i ++)
    {
        auto curStr = str.substr(0, i);
        if (firstVec.size())
        {
            if (!firstVec[num - 1].contains(firstCharToString(curStr)))
            {
                return;
            }
        }
        resItem.push_back(curStr);
        calcPartition(str.substr(i, str.length()), num - 1, res, resItem, minVec, firstVec);
        resItem.pop_back();
    }

}

GramTable::PartitionTable GramTable::partition(std::string_view str, int num, const PartitionOption &option)
{
    PartitionTable res;
    PartitionTableItem resItem;
    std::vector<int> minVec;
    std::vector<std::set<std::string>> firstVec;
    if (option.has_value())
    {
        auto prodRightAlt = std::get<0>(option.value());
        auto optionMaps = std::get<1>(option.value());
        auto optionMinMaps = std::get<0>(optionMaps);
        auto optionFirstMaps = std::get<1>(optionMaps);
        for(auto &prodRightAltItem : prodRightAlt)
        {
            if (optionMinMaps.has_value())
            {
                minVec.insert(minVec.begin(), optionMinMaps.value()[prodRightAltItem]);
            }

            if (optionFirstMaps.has_value())
            {
                firstVec.insert(firstVec.begin(), optionFirstMaps.value()[prodRightAltItem]);
            }
        }
    }
    calcPartition(str, num, res, resItem, minVec, firstVec);
    return res;
}

