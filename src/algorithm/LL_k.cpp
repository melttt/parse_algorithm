#include "LL_k.h"
#include <algorithm>
#include <numeric>
#include <format>
#include <memory>
#include <vector>
#include "BasicNode.h"
#include "BTNode.h"
namespace PARSE_UTIL
{
    using namespace std::literals;
    
    
    StrongLLk::StrongLLk(GramTable &gramTable, std::string inputStr, size_t k, bool verbose)
    :gramTable(gramTable), inputStr(inputStr + std::string(k, GramTable::inputEndTerm[0])), lookAheadSize(k), isConflict(false), verbose(verbose)
    {
        auto firstKmap {gramTable.getFirstKMap(k).value()};
        auto followKmap {gramTable.getFollowKMap(k, firstKmap).value()};
        //fill LLkTable
        for(auto &[prodLeft, prodRightAlts] : gramTable.getGramMap())
        {
            size_t altIndex = 0;
            
            for(auto &prodRightAlt : prodRightAlts)
            {
                auto candidationItem {followKmap[prodLeft]};
                std::cout << std::format("{0} -> {1}\n", prodLeft, std::accumulate(std::begin(prodRightAlt), std::end(prodRightAlt), ""s));
                std::set<std::string> resFirstSet;
                // for A->BC, (1).calculate First(BC) -> CartesianProduct(B,C).
                std::find_if(std::begin(prodRightAlt), std::end(prodRightAlt), [&resFirstSet, &k, &firstKmap](auto &str)
                {
                    if (!resFirstSet.empty() && std::all_of(std::begin(resFirstSet), std::end(resFirstSet),
                     [&k](const auto &str){ return str.length() == k;}))
                    {
                        return true;
                    }
                    decltype(resFirstSet) temp;
                    CartesianProduct(std::begin(resFirstSet), std::end(resFirstSet), std::begin(firstKmap[str]), std::end(firstKmap[str]), 
                    [&k, &temp](const auto& x, const auto& y){
                        auto resStr {x + y};
                        if (resStr.size() > k)
                        {
                            resStr = resStr.substr(0, k);
                        }
                        temp.insert(resStr);
                    });
                    resFirstSet = std::move(temp);
                    return false;
                });
                // for A->BC, (2).calculate First(BC) + Follow(A) -> CartesianProduct(First(BC), Follow(A))
                decltype(candidationItem) resSet;
                CartesianProduct(std::begin(resFirstSet), std::end(resFirstSet), std::begin(candidationItem),
                std::end(candidationItem), [&k, &resSet](const auto& x, const auto& y){
                    auto resStr {x + y};
                    if (resStr.size() > k)
                    {
                        resStr = resStr.substr(0, k);
                    }
                    resSet.insert(resStr);
                });
                candidationItem = std::move(resSet);
                
                // update LLkTable from above
                for(auto item : candidationItem)
                {
                    lltable[prodLeft][item].insert(altIndex);
                    if (lltable[prodLeft][item].size() > 1)
                    {
                        isConflict = true;
                    }
                }
                altIndex ++;
            }
        }
        if (verbose) printLLkTable();
    }
    void StrongLLk::printLLkTable()
    {
        std::cout << std::format("LL{0} Table Info:\n",lookAheadSize);
        for(auto &[noTerm, otherInfo] : lltable)
        {
            for(auto &[lookAheadStr, tableNums] : otherInfo)
            {
                std::string res;
                std::for_each(std::begin(tableNums), std::end(tableNums), [&res](auto x)
                {
                    res += std::to_string(x) + " ";
                });
                std::cout << std::format("noTerm : {0}, lookAheadStr : {1}, choose alt : {2}\n", noTerm, lookAheadStr, res);
            }
        }
    }

    std::shared_ptr<BasicNode> StrongLLk::parse()
    {
        if (isConflict)
        {
            return nullptr;
        }
        auto root {std::make_shared<BasicNode>(GramTable::startNoTerm)};
        size_t startIndex = 0;
        std::vector<std::shared_ptr<BasicNode>> ve{root};
        for(size_t i = 0 ; i < lookAheadSize ; i ++) ve.emplace_back(std::make_shared<BasicNode>(GramTable::inputEndTerm));
        
        __parse(ve, startIndex);
        return root;
    }

    void StrongLLk::__parse(std::vector<std::shared_ptr<BasicNode>>& children, size_t &inputStrIndex)
    {
        for(size_t childrenIndex = 0; childrenIndex < children.size(); ++ childrenIndex)
        {
            auto curStr {std::string(1, inputStr[inputStrIndex])};
            auto curChild {children[childrenIndex]};
            auto curChildStr {curChild->getData()};
            if (verbose)
            {
                std::cout << "-----------------------------------------------\n";
                std::cout << std::format("input curStr:{0}\n", inputStr);
                std::cout << std::format("{0: >{1}}\n", "^", 14 + inputStrIndex);
                std::cout << std::format("curNodeStr:{0}\n", curChildStr);
                std::cout << "cur strage : ";
            }
            if (curChildStr == GramTable::epsilon)
            {
                if (verbose) std::cout << "skip null\n";
                continue;
            }
            if (GramTable::isTerm(curChildStr))
            {
                //match
                if (curChildStr == curStr)
                {
                    if (verbose) std::cout <<"match\n";
                    ++ inputStrIndex;
                    continue;
                }
                else
                {
                    throw "match error\n";
                }
            }
            else
            {
                //prediction
                auto curLookAheadStr {inputStr.substr(inputStrIndex, lookAheadSize)};
                
                if (lltable.contains(curChildStr) && lltable[curChildStr].contains(curLookAheadStr) &&
                    lltable[curChildStr][curLookAheadStr].size() == 1)
                {
                    auto prodRightAlts {gramTable.getGramMap()[curChildStr]};
                    auto prodRightAlt {prodRightAlts[*std::begin(lltable[curChildStr][curLookAheadStr])]};
                    //build tree
                    std::vector<std::shared_ptr<BasicNode>> ve;
                    for(auto prodItem : prodRightAlt)
                    {
                        auto xx {std::make_shared<BasicNode>(prodItem == GramTable::nullSymbol ? GramTable::epsilon : prodItem)};
                        ve.push_back(xx);
                        curChild->addChildren(xx);
                    }
                    if (verbose)
                    {
                        std::cout <<"predict[ " << std::format("{0} -> {1} ]\n",
                        curChildStr, std::accumulate(std::begin(prodRightAlt), std::end(prodRightAlt), ""s));
                    }
                    __parse(ve, inputStrIndex);
                }
                else
                {
                    throw "prediction error\n";
                }

            }
        }

    }


} 