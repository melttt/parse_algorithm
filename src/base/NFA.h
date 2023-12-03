#pragma once
#include <tuple>
#include <vector>
#include "TreeNode.h"
#include "graph.h"
namespace PARSE_UTIL
{
using Node = Graph::Node;
using Edge = Graph::Edge;
using StartEndNodes = std::tuple<Node, Node>;
class NFA
{
    public:
        
        
        NFA();
        NFA(std::shared_ptr<TreeNode> rootNode);
        bool buildNFA(std::shared_ptr<TreeNode> rootNode);
        std::string getDotStr();
        const Graph& getNFAGraph();
    private:
        StartEndNodes buildNode(std::shared_ptr<TreeNode> rootNode);
        Graph graph;
};
}
