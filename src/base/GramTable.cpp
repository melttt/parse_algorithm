#include "GramTable.h"
#include <ranges>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <format>
#include <iterator>
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
            {
                using namespace std::literals;
                prodsPairs.emplace_back(prodLeft, std::accumulate(std::begin(prodRightAlt), std::end(prodRightAlt), ""s));
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

std::set<std::string> updateKMap(const std::set<std::string> &prevCandidationMap, const std::set<std::string>& newSet, size_t k)
{
    std::set<std::string> res;
    for(const auto &newEle : newSet)
    {
        for(const auto &prevEle : prevCandidationMap)
        {
            auto newRes {newEle + prevEle};
            if (newRes.size() > k)
            {
                newRes = newRes.substr(0, k);
            }
            res.insert(newRes);
        }
    }
    return res;
}
bool GramTable::calcFirstKMap(FirstKMap &firstKMap, size_t k)
{
    bool change = false;
    for(const auto &[prodLeft, prodRight] : getProdsPair())
    {
        auto &leftSet {firstKMap[prodLeft]};
        std::remove_reference<decltype(leftSet)>::type candidationSet;
        std::find_if(std::begin(prodRight), std::end(prodRight), [&candidationSet, &firstKMap, &k](const auto& ch)
        {
            if (!candidationSet.empty() && std::all_of(std::begin(candidationSet), std::end(candidationSet), 
                [&k](const auto &str) {return str.length() == k;}))
            {
                return true;
            }

            auto& curTokenFirstSet{firstKMap[std::string(1, ch)]};
            if (curTokenFirstSet.size() == 0)
            {
                std::erase_if(candidationSet, [&k](auto const& x){
                    return x.size() != k;
                });
                return true;
            }

            decltype(candidationSet) resSet;
            CartesianProduct(std::begin(candidationSet), std::end(candidationSet), std::begin(curTokenFirstSet), std::end(curTokenFirstSet), 
            [&k, &resSet](const auto& x, const auto& y){
                auto resStr {x + y};
                if (resStr.size() > k)
                {
                    resStr = resStr.substr(0, k);
                }
                resSet.insert(resStr);
            });
            candidationSet = std::move(resSet);
            return false;
        });

        auto tempResSet {leftSet};
        tempResSet.merge(candidationSet);
        if (tempResSet != leftSet)
        {
            change = true;
            leftSet = std::move(tempResSet);
        }
    }
    return change;
}
std::optional<GramTable::FirstKMap> GramTable::getFirstKMap(size_t k)
{
    FirstKMap firstKMap;
    for(auto term : termSet)
    {
        firstKMap[std::string(term)].insert(std::string(term));
    }
    for(const auto &[prodLeft, prodRight] : getProdsPair())
    {
        if (prodRight == GramTable::nullSymbol)
        {
            firstKMap[prodLeft].insert(prodRight);
        }
    }
    while(calcFirstKMap(firstKMap, k));

    std::cout << "--------------\n";
    for(const auto &[k, v] : firstKMap)
    {
        std::cout << (k == GramTable::nullSymbol ? "\u03B5" : k )<< " : ";
        for(const auto & vv : v)
        {
            std::cout << std::format("{0} ", vv == GramTable::nullSymbol ? "\u03B5" : vv);
        }
        std::cout << std::endl;
    }
    std::cout << "--------------\n";
    return firstKMap;
}
bool GramTable::calcFollowKMap(FollowKMap &followKMap, size_t k, FirstKMap &firstKMap)
{
    bool change = false;
    for(const auto &[prodLeft, prodRight] : getProdsPair())
    {
        auto candidationSet {followKMap[prodLeft]};
        std::for_each(prodRight.rbegin(), prodRight.rend(), [&candidationSet, &followKMap, &firstKMap, k, &change](auto ch)
        {
            auto curStr {std::string(1, ch)};
            auto &curFollow {followKMap[curStr]};
            auto &curFirst {firstKMap[curStr]};
            if (!GramTable::isTerm(curStr))
            {
                for(auto &candidationItem : candidationSet)
                {
                    if (candidationItem.length() == k &&
                        !curFollow.contains(candidationItem))
                    {
                        change = true;
                        curFollow.insert(candidationItem);
                    }
                }
            }

            std::remove_reference<decltype(curFollow)>::type resSet;
            CartesianProduct(std::begin(candidationSet), std::end(candidationSet), std::begin(curFirst), std::end(curFirst), 
            [&k, &resSet](const auto& x, const auto& y){
                auto resStr {y + x};
                if (resStr.size() > k)
                {
                    resStr = resStr.substr(0, k);
                }
                resSet.insert(resStr);
            });

            candidationSet = std::move(resSet);
        });
    }
    return change;
}

std::optional<GramTable::FollowKMap> GramTable::getFollowKMap(size_t k, FirstMap &firstKMap)
{
    FollowKMap followKMap;
    //start symbol S adds k '#'
    followKMap[startNoTerm].insert(std::string(k, inputEndTerm[0]));
    while(calcFollowKMap(followKMap, k, firstKMap));
    
    std::cout << "--------------\n";
    for(const auto &[k, v] : followKMap)
    {
        if (GramTable::isTerm(k)) continue;
        std::cout << (k == GramTable::nullSymbol ? "\u03B5" : k )<< " : ";
        for(const auto & vv : v)
        {
            std::cout << std::format("{0} ", vv == GramTable::nullSymbol ? "\u03B5" : vv);
        }
        std::cout << std::endl;
    }
    std::cout << "--------------\n";
    return followKMap;
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

std::optional<std::map<std::string, std::vector<std::string>>>
GramTable::getProdsWithOnlyTerms()
{
    std::map<std::string, std::vector<std::string>> res;
    for(auto &[prodLeft, prodRightAlts] : grammerMap)
    {
        for(auto &prodRightAlt : prodRightAlts)
        {
            bool hasOnlyTerm = false;
            std::string rightToken;
            for(auto &rightItem : prodRightAlt)
            {
                if (GramTable::isTerm(rightItem))
                {
                    hasOnlyTerm = true;
                    rightToken += rightItem;
                }
                else
                {
                    if (hasOnlyTerm)
                    {
                        return std::nullopt;
                    }
                }
            }
            if (hasOnlyTerm)
            {
                res[rightToken].push_back(prodLeft);
            }
        }
    }
    return res;
}