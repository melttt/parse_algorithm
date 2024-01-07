#pragma once

#include <deque>
#include <memory>
#include <utility>
#include <ranges>
#include <algorithm>
#include <format>
#include "graph.h"
#include "GramTable.h"
#include "BasicNode.h"
namespace PARSE_UTIL
{
class ChartRuler;
class ChartTable :public Graph
{
    public:
        friend class InitChartRuler;
        friend class InitTopDownChartRuler;
        friend class EmptyPredictRule;
        friend class TopDownPredictRule;
        friend class SingleEdgeFundamentalRule;
        friend class BottomUpPredictRule;
        friend class BottomUpPredictCombineRule;
        friend class CompleteFundamentalRule;
        friend class InitEarleyChartRuler;
        friend class EarleyScannerRule;
        friend class EarleyCompleterRule;
        friend class EarleyPredictorRule;
        enum class Strategy
        {
            TOPDOWN,
            BOTTOMUP,
            LEFTCORNER,
            EARLEY
        };
        class ChartEdge : public Edge
        {
            public:
                using ChartEdgeEntity = std::tuple<Node, Node, GramTable::ProdLeft, GramTable::ProdRightAlt, size_t, bool>;
                using EdgeEntityPair = std::tuple<ChartEdgeEntity, ChartEdgeEntity>;
                ChartEdge() = default;
                ChartEdge(Node inNode, Node outNode, GramTable::ProdLeft prodLeft,
                        GramTable::ProdRightAlt prodRight, size_t dotIndex);
                ChartEdge(const ChartEdgeEntity&);
                auto operator<=>(const ChartEdge&) const;
                auto operator==(const ChartEdge&) const;
                operator ChartEdgeEntity() const;
                bool getCompleted() const;
                size_t getDotIndex() const;
                std::string getProdLeft() const;
                std::string getNextToken() const;
                GramTable::ProdRightAlt getProdRight() const;
                void addChildNodes(std::optional<EdgeEntityPair> pairChildNode) const;
                const std::set<EdgeEntityPair>& getChildNodes() const;
            protected:
                GramTable::ProdLeft prodLeft;
                GramTable::ProdRightAlt prodRight;
                size_t dotIndex;
                bool isComplete;
            private:
                mutable std::set<EdgeEntityPair> childNodes;
        };

        ChartTable(GramTable &gramTable, std::string inputStr, Strategy strategy = Strategy::EARLEY);
        void printHead();
        void printEdge(ChartEdge &edge);
        std::shared_ptr<BasicNode> parse();
        bool makeNewEdge(const ChartEdge &edge, std::optional<ChartEdge::EdgeEntityPair> entityPair);
        std::set<ChartEdge> getEdgeSet();
    protected:
        GramTable &gramTable;
        std::string inputStr;
        std::set<ChartEdge> edgeSet;
        std::vector<std::shared_ptr<ChartRuler>> rules;
        static const inline size_t IDENTSIZE = 17;
    private:
        std::shared_ptr<BasicNode> buildFirstTree();
        std::vector<std::shared_ptr<BasicNode>> _buildFirstTree(const ChartEdge &edge);
        std::map<Graph::Node ,std::set<ChartTable::ChartEdge>> earleyInput;
        Strategy strategy {Strategy::TOPDOWN};

};

class ChartRuler
{
    public:
        virtual std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable){ return {};}
        virtual std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge){ return {};}
};
class InitChartRuler : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable) override;
};

class InitEarleyChartRuler : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable) override;
};

class InitTopDownChartRuler : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable) override;
};

class EmptyPredictRule : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable) override;
};

class TopDownPredictRule : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge) override;
    private:
        std::set<std::tuple<ChartTable::Node, std::string>> cacheEdges;
};

class BottomUpPredictRule : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge) override;
};

class BottomUpPredictCombineRule : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge) override;
};

class SingleEdgeFundamentalRule : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge) override;
    protected:
        virtual std::vector<ChartTable::ChartEdge> applyComplete(ChartTable &chartTable, ChartTable::ChartEdge &rightEdge);
        virtual std::vector<ChartTable::ChartEdge> applyInComplete(ChartTable &chartTable, ChartTable::ChartEdge &leftEdge);
};


class CompleteFundamentalRule : public SingleEdgeFundamentalRule
{
    protected:
        std::vector<ChartTable::ChartEdge> applyInComplete(ChartTable &chartTable, ChartTable::ChartEdge &leftEdge) override;
};

class EarleyScannerRule : public ChartRuler
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge) override;
};

class EarleyCompleterRule : public CompleteFundamentalRule
{
    public:
        std::vector<ChartTable::ChartEdge> apply(ChartTable &chartTable, ChartTable::ChartEdge &edge) override;
};

class EarleyPredictorRule : public TopDownPredictRule
{

};
}
