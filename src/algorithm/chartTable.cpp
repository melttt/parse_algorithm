#include <deque>
#include <memory>
#include <utility>
#include <ranges>
#include <algorithm>
#include <format>
#include <set>
#include "graph.h"
#include "GramTable.h"
#include "chartTable.h"
#include "BTNode.h"
namespace PARSE_UTIL
{
    ChartTable::ChartEdge::ChartEdge(Node inNode, Node outNode, 
        GramTable::ProdLeft prodLeft, GramTable::ProdRightAlt prodRight, size_t dotIndex)
    :Edge(inNode, outNode, ""), prodLeft{std::move(prodLeft)}, prodRight{std::move(prodRight)}, dotIndex{dotIndex}
    {
        isComplete = dotIndex == this->prodRight.size();
        if (GramTable::isEmptyProd(this->prodRight))
        {
            isComplete = 1;
        }

        std::string prod{this->prodLeft + "->"};
        for(size_t index = 0; auto str : this->prodRight)   
        {
            if (dotIndex == index)
            {
                prod += "*";
            }
            prod += str;
            index ++;
        }
        if (dotIndex == this->prodRight.size())
        {
            prod += "*";
        }
        setData(prod);
    }

    ChartTable::ChartEdge::ChartEdge(const ChartEdgeEntity &entity)
    :ChartEdge(std::get<0>(entity), std::get<1>(entity), std::get<2>(entity), std::get<3>(entity), std::get<4>(entity))
    {

    }

    auto ChartTable::ChartEdge::operator<=>(const ChartEdge& rhs) const
    {
        return ChartEdgeEntity(inDegreeNode, outDegreeNode, prodLeft, prodRight, dotIndex, isComplete) 
            <=> ChartEdgeEntity(rhs.inDegreeNode, rhs.outDegreeNode, rhs.prodLeft, rhs.prodRight, rhs.dotIndex, rhs.isComplete);
    }
    auto ChartTable::ChartEdge::operator==(const ChartEdge& rhs) const
    {
        return ChartEdgeEntity(inDegreeNode, outDegreeNode, prodLeft, prodRight, dotIndex, isComplete) 
            == ChartEdgeEntity(rhs.inDegreeNode, rhs.outDegreeNode, rhs.prodLeft, rhs.prodRight, rhs.dotIndex, rhs.isComplete);
    }
    ChartTable::ChartEdge::operator ChartEdgeEntity() const
    {
        return {inDegreeNode, outDegreeNode, prodLeft, prodRight, dotIndex, isComplete};
    }

    bool ChartTable::ChartEdge::getCompleted() const
    {
        return isComplete;
    }

    size_t ChartTable::ChartEdge::getDotIndex() const
    {
        return dotIndex;
    }

    std::string ChartTable::ChartEdge::getProdLeft() const
    {
        return prodLeft;
    }

    std::string ChartTable::ChartEdge::getNextToken() const
    {
        if (getCompleted())
        {
            return "";
        }
        return prodRight[dotIndex];
    }
    GramTable::ProdRightAlt ChartTable::ChartEdge::getProdRight() const
    {
        return prodRight;
    }

    ChartTable::ChartTable(GramTable &gramTable, std::string inputStr, Strategy strategy): 
    gramTable(gramTable), inputStr{std::move(inputStr)}, strategy{strategy}
    {
        switch (strategy)
        {
        case Strategy::BOTTOMUP :
            rules = {
                std::make_shared<InitChartRuler>(),
                std::make_shared<EmptyPredictRule>(),
                std::make_shared<BottomUpPredictRule>(),
                std::make_shared<SingleEdgeFundamentalRule>()
            };
            break;
        
        case Strategy::LEFTCORNER :
            rules = {
                std::make_shared<InitChartRuler>(),
                std::make_shared<EmptyPredictRule>(),
                std::make_shared<BottomUpPredictCombineRule>(),
                std::make_shared<SingleEdgeFundamentalRule>()
            };
            break;
        
        case Strategy::EARLEY :
            rules = {
                std::make_shared<InitEarleyChartRuler>(),
                std::make_shared<InitTopDownChartRuler>(),
                std::make_shared<EarleyScannerRule>(),
                std::make_shared<EarleyCompleterRule>(),
                std::make_shared<EarleyPredictorRule>(),
            };
            break;
        case Strategy::TOPDOWN :
        [[fallthrough]];
        default:
            rules = {
                std::make_shared<InitChartRuler>(),
                std::make_shared<InitTopDownChartRuler>(),
                std::make_shared<TopDownPredictRule>(), 
                std::make_shared<SingleEdgeFundamentalRule>()
            };
            break;
        }
    }

    void ChartTable::ChartEdge::addChildNodes(std::optional<EdgeEntityPair> pairChildNode) const
    {
        if (pairChildNode.has_value())
        {
            childNodes.insert(pairChildNode.value());
        }
    }
    const std::set<ChartTable::ChartEdge::EdgeEntityPair>& ChartTable::ChartEdge::getChildNodes() const
    {
        return childNodes;
    }

    void ChartTable::printHead()
    {
        std::cout << "\n|";
        for(auto ch : inputStr)
        {
            std::cout << std::format("{0: ^{1}}|", ch, IDENTSIZE);
        }
        std::cout << "\n";
    }
    void ChartTable::printEdge(ChartTable::ChartEdge &edge)
    {
        auto startSpace{edge.getInDegreeNode().getId() * (IDENTSIZE+1)};
        if (edge.getInDegreeNode().getId())
        {
            std::cout << std::format("{0:{1}}|", ' ', startSpace);
        }
        else
        {
            std::cout << "|";
        }
        auto endSpace {0};
        if (edge.getInDegreeNode().getId() + 1 < edge.getOutDegreeNode().getId())
        {
           endSpace = (edge.getOutDegreeNode().getId() - edge.getInDegreeNode().getId() - 1) * (IDENTSIZE + 1);
        }

        std::cout << std::format("{0:^{1}}|\n", edge.getData(), edge.getInDegreeNode() == edge.getOutDegreeNode() ? 0 : IDENTSIZE + endSpace);
    }

    std::shared_ptr<BasicNode> ChartTable::parse()
    {
        if (strategy == Strategy::EARLEY)
        {
            std::deque<ChartEdge> agenda;
            printHead();
            for(auto rule : rules)
            {
                for(auto res : rule->apply(*this))
                {
                    agenda.push_back(res);
                    printEdge(res);
                }
            }

            for(auto endNode : getNodes())
            {
                std::deque<ChartEdge> agenda;
                for(auto &edge : edgeSet)
                {
                    if (edge.getOutDegreeNode() == endNode)
                    {
                        agenda.push_back(edge);
                    }
                }
                if (earleyInput.contains(endNode))
                {
                    for(auto &edge : earleyInput[endNode])
                    {
                        agenda.push_back(edge);
                    }
                }
                while(!agenda.empty())
                {
                    auto curEdge{agenda.back()};
                    agenda.pop_back();
                    std::vector<ChartTable::ChartEdge> tempRes;
                    for(auto rule : rules)
                    {
                        auto temp = rule->apply(*this, curEdge);
                        for(auto t : temp)
                        {
                            tempRes.push_back(t);
                        }
                    }
                    for(auto res : tempRes)
                    {
                        printEdge(res);
                        agenda.push_back(res);   
                    }
                }
            }
        }
        else
        {
            std::deque<ChartEdge> agenda;
            printHead();
            for(auto rule : rules)
            {
                for(auto res : rule->apply(*this))
                {
                    agenda.push_back(res);
                    printEdge(res);
                }
                
            }
            while(!agenda.empty())
            {
                auto curEdge{agenda.back()};
                agenda.pop_back();
                std::vector<ChartTable::ChartEdge> tempRes;
                for(auto rule : rules)
                {
                    auto temp = rule->apply(*this, curEdge);
                    for(auto t : temp)
                    {
                        tempRes.push_back(t);
                    }
                }

                for(auto res : tempRes)
                {
                    agenda.push_back(res);
                    printEdge(res);
                }
            }
        }
        return buildFirstTree();

    }

    bool ChartTable::makeNewEdge(const ChartEdge &edge, std::optional<ChartEdge::EdgeEntityPair> entityPair = std::nullopt)
    {
        if (auto it{edgeSet.find(edge)}; it != edgeSet.end())
        {
            it->addChildNodes(entityPair);
            return false;
        }
        edge.addChildNodes(entityPair);
        edgeSet.insert(edge);
        Graph::makeNewEdge(edge.getInDegreeNode(), edge.getOutDegreeNode(), edge.getData());
        return true;
    }

    std::shared_ptr<BasicNode> ChartTable::buildFirstTree()
    {

        for(auto edge : edgeSet)
        {
            if (edge.getProdLeft() == gramTable.getStartNoTerm() && edge.getCompleted() 
                && edge.getInDegreeNode() == *nodes.begin()  && edge.getOutDegreeNode() == *nodes.rbegin())
            {
                auto res {_buildFirstTree(edge)};
                if (res.size() > 0)
                {
                    return res[0];
                }
                return nullptr;
            }
        }
        return nullptr;
    }
    std::vector<std::shared_ptr<BasicNode>> ChartTable::_buildFirstTree(const ChartEdge &edge)
    {
        if (edge.getDotIndex() == 0)
        {
            return {};
        }

        if (edge.getCompleted() && edge.getDotIndex() == 1 && GramTable::isTerm(edge.getProdRight().at(0)))
        {
            auto root {std::make_shared<BasicNode>(edge.getProdLeft())}; 
            auto child {std::make_shared<BasicNode>(edge.getProdRight().at(0))};
            root->addChildren(child);
            return {root};
        }

        auto [leftEntity, rightEntity] {*(edge.getChildNodes().begin())};
        auto leftEdge{*edgeSet.find(leftEntity)};
        auto rightEdge{*edgeSet.find(rightEntity)};

        if (edge.getCompleted())
        {
            auto Node {std::make_shared<BasicNode>(edge.getProdLeft())};
            for(auto basicNode : _buildFirstTree(leftEdge))
            {
                Node->addChildren(basicNode);
            }
            for(auto basicNode : _buildFirstTree(rightEdge))
            {
                Node->addChildren(basicNode);
            }
            return {Node};
        }
        
        std::vector<std::shared_ptr<BasicNode>> res;
        for(auto basicNode : _buildFirstTree(leftEdge))
        {
            res.push_back(basicNode);
        }
        for(auto basicNode : _buildFirstTree(rightEdge))
        {
            res.push_back(basicNode);
        }
        return res;
    }
    
    std::set<ChartTable::ChartEdge> ChartTable::getEdgeSet()
    {
        return edgeSet;
    }
    std::vector<ChartTable::ChartEdge> EmptyPredictRule::apply(ChartTable &chartTable)
    {
        std::vector<ChartTable::ChartEdge> res;
        for(auto &[prodLeft, prodRightAlts] : chartTable.gramTable.getGramMap())
        {
            for(auto &prodRightAlt : prodRightAlts)
            {
                if (GramTable::isEmptyProd(prodRightAlt))
                {
                    for(const auto &curNode : chartTable.getNodes())
                    {
                        ChartTable::ChartEdge newEdge {curNode, curNode, prodLeft, prodRightAlt, 0};
                        if (chartTable.makeNewEdge(newEdge))
                        {
                            res.emplace_back(newEdge);
                        }
                    }
                }
            }
        }
        return res;
    }

    std::vector<ChartTable::ChartEdge> InitChartRuler::apply(ChartTable &chartTable)
    {
        std::vector<ChartTable::ChartEdge> res; 
        auto termToProdLefts {chartTable.gramTable.getProdsWithOnlyTerms()};
        if (!termToProdLefts.has_value())
        {
            throw "wrong chart table gramTable";
        }
        auto prevNode {chartTable.makeNewNode()};

        for(auto ch : chartTable.inputStr)
        {
            auto curNode {chartTable.makeNewNode()};
            std::string curToken{std::string(1, ch)};
            
            if (auto &tmpMap {termToProdLefts.value()}; tmpMap.contains(curToken))
            {
                for(auto leftProd : tmpMap[curToken])
                {
                    auto newEdge {ChartTable::ChartEdge(prevNode, curNode, leftProd, {curToken}, 1)};
                    if (chartTable.makeNewEdge(newEdge))
                    {
                        res.emplace_back(newEdge);
                    }
                }
            }
            prevNode = curNode;
        }
        std::ranges::reverse(res);
        return res;
    }

    std::vector<ChartTable::ChartEdge> InitEarleyChartRuler::apply(ChartTable &chartTable)
    {
        std::vector<ChartTable::ChartEdge> res; 
        auto termToProdLefts {chartTable.gramTable.getProdsWithOnlyTerms()};
        if (!termToProdLefts.has_value())
        {
            throw "wrong chart table gramTable";
        }
        auto prevNode {chartTable.makeNewNode()};
        for(auto ch : chartTable.inputStr)
        {
            auto curNode {chartTable.makeNewNode()};
            std::string curToken{std::string(1, ch)};
            
            if (auto &tmpMap {termToProdLefts.value()}; tmpMap.contains(curToken))
            {
                chartTable.earleyInput[curNode] = {};
                for(auto leftProd : tmpMap[curToken])
                {
                    auto newEdge {ChartTable::ChartEdge(prevNode, curNode, leftProd, {curToken}, 1)};
                    chartTable.earleyInput[curNode].insert(newEdge);
                }
            }
            prevNode = curNode;
        }
        return res;
    }

    std::vector<ChartTable::ChartEdge> InitTopDownChartRuler::apply(ChartTable &chartTable)
    {
        std::vector<ChartTable::ChartEdge> res;
        for(auto &[prodLeft, prodRightAlts] : chartTable.gramTable.getGramMap())
        {
            if (prodLeft == chartTable.gramTable.getStartNoTerm())
            {
                for(auto & prodRightAlt : prodRightAlts)
                {
                    ChartTable::ChartEdge newEdge {0, 0, prodLeft, prodRightAlt, 0};
                    if (chartTable.makeNewEdge(newEdge))
                    {
                        res.emplace_back(newEdge);
                    }
                }
            }
        }
        return res;
    }

    std::vector<ChartTable::ChartEdge> TopDownPredictRule::apply(ChartTable &chartTable, ChartTable::ChartEdge &edge)
    { 
        std::vector<ChartTable::ChartEdge> res;
        if (edge.getCompleted())
        {
            return res;
        }
        auto edgeNextToken{edge.getNextToken()};

        for(auto &[prodLeft, prodRightAlts] : chartTable.gramTable.getGramMap())
        {
            if (prodLeft == edgeNextToken)
            {
                
                for(auto & prodRightAlt : prodRightAlts)
                {
                    
                    ChartTable::ChartEdge newEdge {ChartTable::Node{edge.getOutDegreeNode()}, 
                                ChartTable::Node{edge.getOutDegreeNode()}, prodLeft, prodRightAlt, 0};
                    if (chartTable.makeNewEdge(newEdge))
                    {
                        res.push_back(newEdge);
                    }
                }
            }
        }
        return res;
    }

    std::vector<ChartTable::ChartEdge> BottomUpPredictRule::apply(ChartTable &chartTable, ChartTable::ChartEdge &edge)
    {
        std::vector<ChartTable::ChartEdge> res;
        if (!edge.getCompleted())
        {
            return {};
        }
        for(auto &[prodLeft, prodRightAlts] : chartTable.gramTable.getGramMap())
        {
            for(auto &prodRightAlt : prodRightAlts)
            {
                if (prodRightAlt.size() > 0 && prodRightAlt[0] == edge.getProdLeft())
                {
                    ChartTable::ChartEdge newEdge {edge.getInDegreeNode(), 
                    edge.getInDegreeNode(), prodLeft, prodRightAlt, 0};
                    chartTable.makeNewEdge(newEdge);
                    res.push_back(newEdge);
                }
            }
        }
        return res;
    }

    std::vector<ChartTable::ChartEdge> BottomUpPredictCombineRule::apply(ChartTable &chartTable, ChartTable::ChartEdge &rightEdge)
    {
        std::vector<ChartTable::ChartEdge> res;
        if (!rightEdge.getCompleted())
        {
            return {};
        }
        for(auto &[prodLeft, prodRightAlts] : chartTable.gramTable.getGramMap())
        {
            for(auto &prodRightAlt : prodRightAlts)
            {
                if (prodRightAlt.size() > 0 && prodRightAlt[0] == rightEdge.getProdLeft())
                {
                    ChartTable::ChartEdge leftEdge {rightEdge.getInDegreeNode(), 
                    rightEdge.getInDegreeNode(), prodLeft, prodRightAlt, 0};
                    chartTable.makeNewEdge(leftEdge);
                    ChartTable::ChartEdge newEdge {rightEdge.getInDegreeNode(), 
                    rightEdge.getOutDegreeNode(), prodLeft, prodRightAlt, 1};
                    chartTable.makeNewEdge(newEdge, std::make_tuple(leftEdge, rightEdge));
                    res.push_back(newEdge);
                }
            }
        }
        return res;
    }


    std::vector<ChartTable::ChartEdge> SingleEdgeFundamentalRule::apply(ChartTable &chartTable, ChartTable::ChartEdge &edge)
    {
        if (edge.getCompleted())
        {
            return applyComplete(chartTable, edge);
        }
        return applyInComplete(chartTable, edge);
    }

    std::vector<ChartTable::ChartEdge> SingleEdgeFundamentalRule::applyComplete(ChartTable &chartTable, ChartTable::ChartEdge &rightEdge)
    {
        std::vector<ChartTable::ChartEdge> res;
        
        for(auto leftEdge : chartTable.getEdgeSet())
        {
                std::string prodright;
                for(auto str : rightEdge.getProdRight())
                {
                    prodright += str;
                }
                if (!leftEdge.getCompleted() && leftEdge.getNextToken() == rightEdge.getProdLeft() 
                    && leftEdge.getOutDegreeNode() == rightEdge.getInDegreeNode())
                {
                    ChartTable::ChartEdge newEdge {leftEdge.getInDegreeNode(), rightEdge.getOutDegreeNode(), 
                                                    leftEdge.getProdLeft(), leftEdge.getProdRight(), leftEdge.getDotIndex() + 1};
                    if (chartTable.makeNewEdge(newEdge, std::make_tuple(leftEdge, rightEdge)))
                    {
                        res.emplace_back(newEdge);
                    }
                }
            
        }
        return res;
    }

    std::vector<ChartTable::ChartEdge> SingleEdgeFundamentalRule::applyInComplete(ChartTable &chartTable, ChartTable::ChartEdge &leftEdge)
    {
        std::vector<ChartTable::ChartEdge> res;
        for(auto rightEdge : chartTable.getEdgeSet())
        {
            if (rightEdge.getCompleted() && leftEdge.getNextToken() == rightEdge.getProdLeft()
                && leftEdge.getOutDegreeNode() == rightEdge.getInDegreeNode())
            {
                ChartTable::ChartEdge newEdge {leftEdge.getInDegreeNode(), rightEdge.getOutDegreeNode(), 
                                                leftEdge.getProdLeft(), leftEdge.getProdRight(), leftEdge.getDotIndex() + 1};
                if (chartTable.makeNewEdge(newEdge, std::make_tuple(leftEdge, rightEdge)))
                {
                    res.emplace_back(newEdge);
                }
            }
        }

        return res;
    }


    std::vector<ChartTable::ChartEdge> CompleteFundamentalRule::applyInComplete(ChartTable &chartTable, ChartTable::ChartEdge &leftEdge)
    {
        std::vector<ChartTable::ChartEdge> res;
        for(auto rightEdge : chartTable.getEdgeSet())
        {
            if (rightEdge.getCompleted() && leftEdge.getNextToken() == rightEdge.getProdLeft()
                && leftEdge.getOutDegreeNode() == rightEdge.getInDegreeNode()
                && rightEdge.getOutDegreeNode() == leftEdge.getOutDegreeNode())
            {
                ChartTable::ChartEdge newEdge {leftEdge.getInDegreeNode(), rightEdge.getOutDegreeNode(), 
                                                leftEdge.getProdLeft(), leftEdge.getProdRight(), leftEdge.getDotIndex() + 1};
                if (chartTable.makeNewEdge(newEdge, std::make_tuple(leftEdge, rightEdge)))
                {
                    res.emplace_back(newEdge);
                }
            }
        }
        return res;
    }


    std::vector<ChartTable::ChartEdge> EarleyScannerRule::apply(ChartTable &chartTable, ChartTable::ChartEdge &edge)
    {
        std::vector<ChartTable::ChartEdge> res;
        if (chartTable.earleyInput.contains(edge.getOutDegreeNode()) &&
            chartTable.earleyInput[edge.getOutDegreeNode()].contains(edge))
        {
            if (chartTable.makeNewEdge(edge))
            {
                res.emplace_back(edge);
            }
        }
        return res;
    }


    std::vector<ChartTable::ChartEdge> EarleyCompleterRule::apply(ChartTable &chartTable, ChartTable::ChartEdge &edge)
    {
        return CompleteFundamentalRule::apply(chartTable, edge);
    }
    

}



