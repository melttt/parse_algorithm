#include "unger.h"
#include <memory>
unger::unger(GramTable &gramTable ,std::string str):
gramTable(gramTable), inputStr(str), gramMap(gramTable.getGramMap())
{
    if (inputStr.length() > INPUTMAXSIZE)
    {
        throw "unger can't handle the strings exceeding 100 in length!";
    }
    minMap = gramTable.getMinMap();
    firstMap = gramTable.getFirstMap();
}

std::shared_ptr<BasicNode> unger::parse()
{
    return _parse("S", inputStr, {0, inputStr.length()});
}

std::shared_ptr<BasicNode> unger::_parse(std::string_view prodLeft, std::string_view curStr, MemoryIndex memoryIndex)
{
    auto historyNode {std::make_tuple(prodLeft, curStr)};
    auto bNode {std::make_shared<BasicNode>(prodLeft.empty() ? std::string("#") : std::string(prodLeft))};
    auto &memoryMap {memoryRes[std::get<0>(memoryIndex)][std::get<1>(memoryIndex)]};
    if (memoryMap.contains(prodLeft))
    {
        return memoryMap[prodLeft];
    }
    
    if (historyNodeSet.contains(historyNode))
    {
        return memoryMap[prodLeft] = nullptr;
    }
    
    if (GramTable::isTerm(prodLeft) && prodLeft == curStr)
    {
        return memoryMap[prodLeft] = bNode;
    }
    
    if (!gramMap.contains(prodLeft.data()))
    {
        return memoryMap[prodLeft] = nullptr;
    }
    
    historyNodeSet.insert(historyNode);
    for(auto &prod : gramMap[prodLeft.data()])
    {
        for(auto &part : GramTable::partition(curStr, prod.size(),  {{prod, {minMap, firstMap}}}))
        {
            auto isSuccess {true};
            std::vector<std::shared_ptr<BasicNode>> tmpNodeVe;
            for(decltype(prod.size()) i = 0, curSt = std::get<0>(memoryIndex) ; i < prod.size(); curSt += part[i].length(), i ++)
            {
                auto tmpNode = _parse(prod[i], part[i], {curSt, curSt + part[i].length()});
                if (tmpNode != nullptr)
                {
                    tmpNodeVe.push_back(tmpNode);
                }
                else
                {
                    isSuccess = false;
                    break;
                }
            }
            if (isSuccess)
            {
                for(auto tmpNode : tmpNodeVe)
                {
                    bNode->addChildren(tmpNode);
                }
                historyNodeSet.erase(historyNode);
                return memoryMap[prodLeft] = bNode;
            }
        }
    }
    
    historyNodeSet.erase(historyNode);
    return memoryMap[prodLeft] = nullptr;
}
