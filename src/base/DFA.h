#pragma once
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <optional>
#include "NFA.h"
#include "graph.h"
namespace PARSE_UTIL
{
using Node = Graph::Node;
using Edge = Graph::Edge;
using NodeSet = Graph::NodeSet;

class DFA
{
    public:
        DFA(NFA nfa);
        std::tuple<std::string, std::string> getDotTowStr();
        std::optional<std::string> isMatch(std::string inputStr);

    
    private:
        void buildMinimumDFA(const std::set<NodeSet>& partitions);
        void minimization();
        NodeSet makeImage(std::string edgeData, NodeSet nodeSet);
        std::tuple<std::set<NodeSet>, std::set<NodeSet>> initMinimazation();
        void buildInitDFA();
        std::set<std::string> getNoEpsilonEdgeData(NodeSet nodeSet);
        NodeSet epsilonClosure(Node curNode);
        NodeSet epsilonClosure(NodeSet curNodes);
        NodeSet move(Node curNode, std::string data);
        NodeSet move(NodeSet curNodes, std::string ele);
        const Graph &nfaGraph;
        Graph initDFAGraph;
        Graph minDFAGraph;
        Graph::AdjacencyList outDgreeAL;
        std::set<std::string> sigma;
};

}


