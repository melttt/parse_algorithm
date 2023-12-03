#include "NFA.h"

namespace PARSE_UTIL
{

NFA::NFA()
{
    
}

NFA::NFA(std::shared_ptr<TreeNode> rootNode)
{
    buildNFA(rootNode);
}


bool NFA::buildNFA(std::shared_ptr<TreeNode> rootNode)
{
    try
    {
        auto [retStartNode, retEndNode] {buildNode(rootNode)};
        graph.setEndNode(retEndNode);
        graph.setStartNode(retStartNode);
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
        return false;
    }
    return true;
}
std::string NFA::getDotStr()
{
    return graph.getDotStr();
}
const Graph& NFA::getNFAGraph()
{
    return graph;
}

StartEndNodes NFA::buildNode(std::shared_ptr<TreeNode> rootNode)
{
    if (!rootNode)
        throw "nullptr node";
    if (auto curStr{rootNode->getData()}; curStr == "*")
    {
        if (rootNode->getChildSize() > 1)
            throw "get multi child";
        auto [retStartNode, retEndNode] = buildNode(rootNode->getChild(0));
        
        graph.makeNewEdge(retEndNode, retStartNode, Edge::epsilon);
        auto startNode{graph.makeNewNode()}, endNode{graph.makeNewNode()};
        graph.makeNewEdge(startNode, retStartNode, Edge::epsilon);
        graph.makeNewEdge(retEndNode, endNode, Edge::epsilon);
        graph.makeNewEdge(startNode,  endNode, Edge::epsilon);
        return {startNode, endNode};
    }
    else if(curStr == "|")
    {
        auto startNode{graph.makeNewNode()}, endNode{graph.makeNewNode()};
        for(auto child : rootNode->getChildren())
        {
            auto [retStartNode, retEndNode] {buildNode(child)};
            graph.makeNewEdge(startNode, retStartNode, Edge::epsilon);
            graph.makeNewEdge(retEndNode, endNode, Edge::epsilon);
        }
        return {startNode, endNode};
    }   
    else if (curStr == ".")
    {
        if (rootNode->getChildSize() != 2)
            throw "need 2 child!";
        auto [retStartNode1, retEndNode1]{buildNode(rootNode->getChild(0))};
        auto [retStartNode2, retEndNode2]{buildNode(rootNode->getChild(1))};
        graph.makeNewEdge(retEndNode1, retStartNode2, Edge::epsilon);
        return {retStartNode1, retEndNode2};
    }
    else
    {
        if (!rootNode->isLeafNode())
        {
            throw "isNotLeatNode!";
        }
        Node startNode{graph.makeNewNode()};
        Node endNode{graph.makeNewNode()};
        graph.makeNewEdge(startNode, endNode, curStr);
        return {startNode, endNode};
    }
}

}