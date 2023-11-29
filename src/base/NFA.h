#pragma once
#include <tuple>
#include <vector>
#include "TreeNode.h"
using NodeId = size_t;
const NodeId MaxNodeId{777777777};
class Node
{
    public:
        Node():id{MaxNodeId}{}
        Node(NodeId id):id{id}{}
        operator NodeId()
        {
            return id;
        }
        auto operator<=>(const Node&) const = default;
        bool operator==(const Node&) const = default;
        auto getId()
        {
            return id;
        }
    private:
        NodeId id;
};
static std::string dotGetNodeStr(NodeId node)
{
    return "node" + std::to_string(node);
}
class Edge
{
    public:
        static const inline std::string epsilon{"\u03B5"};
    public:
        Edge(NodeId inputId, NodeId outputId, std::string data)
        :inputId{inputId}, outputId{outputId}, data{data}{}
        NodeId getInputId() const
        {
            return inputId;
        }
        NodeId getOutputId() const
        {
            return outputId;
        }
        std::string getData() const
        {
            return *data.begin();
        }
    private:
        NodeId inputId;
        NodeId outputId;
        std::vector<std::string> data;
};
class NFA
{
    public:
        using StartEndNodes = std::tuple<Node, Node>;
        NFA()
        {
            
        }

        bool buildNFA(std::shared_ptr<TreeNode> rootNode)
        {
            try
            {
                auto [retStartNode, retEndNode] {buildCommon(rootNode)};
                finalNode = retEndNode;
                startNode = retStartNode;
            }
            catch(const char* e)
            {
                std::cerr << e << '\n';
                return false;
            }
            return true;
        }
        std::string ConvertToDotStr()
        {
            std::string res {"digraph G{\n"};
            //print Node
            for(auto node : nodes)
            {
                auto nodeId{dotGetNodeStr(node)};
                auto finalInfo {isFinalNode(node) ? ", shape=doublecircle": ", shape=circle"};
                auto nodeInfo {"[label=\"" + std::to_string(node) + "\"" + finalInfo + "]\n"};
                res += nodeId + nodeInfo;
            }

            for(auto edges : graph)
            {
                for(auto edge : edges)
                {
                    auto inputIdStr{dotGetNodeStr(edge.getInputId())};
                    auto outputIdStr{dotGetNodeStr(edge.getOutputId())};
                    auto edgeInfo{inputIdStr + "->" + outputIdStr + "[label=\"" + edge.getData() + "\"]\n"};
                    res += edgeInfo;
                }
            }
            res += "}";
            return res;
        }
        std::vector<std::vector<Edge>>& getGraph()
        {
            return graph;
        }
        Node getStartNode()
        {
            return startNode;
        }
        
        Node GetFinalNode()
        {
            return finalNode;
        }
    private:
        
        StartEndNodes buildCommon(std::shared_ptr<TreeNode> rootNode)
        {
            if (!rootNode)
                throw "nullptr node";
            if (auto curStr{rootNode->getData()}; curStr == "*")
            {
                if (rootNode->getChildSize() > 1)
                    throw "get multi child";
                auto [retStartNode, retEndNode] = buildCommon(rootNode->getChild(0));
                makeNewEdge(retEndNode, retStartNode, Edge::epsilon);
                auto startNode{makeNewNode()}, endNode{makeNewNode()};
                makeNewEdge(startNode, retStartNode, Edge::epsilon);
                makeNewEdge(retEndNode, endNode, Edge::epsilon);
                makeNewEdge(startNode,  endNode, Edge::epsilon);
                return {startNode, endNode};
            }
            else if(curStr == "|")
            {
                auto startNode{makeNewNode()}, endNode{makeNewNode()};
                for(auto child : rootNode->getChildren())
                {
                    auto [retStartNode, retEndNode] {buildCommon(child)};
                    makeNewEdge(startNode, retStartNode, Edge::epsilon);
                    makeNewEdge(retEndNode, endNode, Edge::epsilon);
                }
                return {startNode, endNode};
            }   
            else if (curStr == ".")
            {
                if (rootNode->getChildSize() != 2)
                    throw "need 2 child!";
                auto [retStartNode1, retEndNode1]{buildCommon(rootNode->getChild(0))};
                auto [retStartNode2, retEndNode2]{buildCommon(rootNode->getChild(1))};
                makeNewEdge(retEndNode1, retStartNode2, Edge::epsilon);
                return {retStartNode1, retEndNode2};
            }
            else
            {
                if (!rootNode->isLeafNode())
                {
                    throw "isNotLeatNode!";
                }
                Node startNode{makeNewNode()};
                Node endNode{makeNewNode()};
                makeNewEdge(startNode, endNode, curStr);
                return {startNode, endNode};
            }
        }

        Node& makeNewNode()
        {
            nodes.emplace_back(nodes.size());
            graph.push_back({});
            return nodes.back();
        }
        void makeNewEdge(Node startNode, Node endNode, std::string data)
        {
            graph[startNode].emplace_back(startNode, endNode, std::move(data));
        }
        bool isFinalNode(Node &node)
        {
            return node == finalNode;
        }
        
        std::vector<std::vector<Edge>> graph;
        std::vector<Node> nodes;
        Node finalNode;
        Node startNode;
};