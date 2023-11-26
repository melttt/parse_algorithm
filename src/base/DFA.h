#pragma once
#include <set>
#include <map>
#include <queue>
#include "NFA.h"
using NodeSet = std::set<Node>;

class DFANode
{
    public:
        DFANode(){}
        DFANode(NodeSet nodeSet, NodeId nodeId):nodeSet{std::move(nodeSet)}, nodeId{nodeId}{}
        operator NodeId()
        {
            return nodeId;
        }
        NodeSet& getNodeSet()
        {
            return nodeSet;
        }
        NodeId getNodeId()
        {
            return nodeId;
        }
    private:
        NodeId nodeId;
        NodeSet nodeSet;
};

class DFAEdge
{
    public:
        DFAEdge(DFANode inputNode, DFANode outputNode, std::set<std::string> edgedata)
        :inputNode{inputNode}, outputNode{outputNode}, edgedata{edgedata}{}
        NodeId getInputId()
        {
            return inputNode.getNodeId();
        }
        NodeId getOutputId()
        {
            return outputNode.getNodeId();
        }
        std::string getData()
        {
            return *edgedata.begin();
        }
    private:
        DFANode inputNode;
        DFANode outputNode;
        std::set<std::string> edgedata;
};
class DFA
{
    public:
    DFA(NFA nfa):nfa{nfa}
    {
        buildDFA();
    }
    std::string ConvertToDotStr()
    {
        std::string res {"digraph G{\n"};
        for(auto node : nodes)
        {
            auto nodeId{dotGetNodeStr(node)};
            auto finalInfo {isFinalNode(node.getNodeSet()) ? ", shape=doublecircle": ", shape=circle"};
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

    private:
    bool isFinalNode(const NodeSet &nodeSet)
    {
        for(auto node : nodeSet)
        {
            if (node == nfa.GetFinalNode())
            {
                return true;
            }
        }
        return false;
    }
    
    void buildDFA()
    {
        auto startNodeSet {epsilonClosure(nfa.getStartNode())};
        std::queue<NodeSet> que;
        std::map<NodeSet, NodeId> visited;
        auto startDFANode {makeDFANode(startNodeSet)};
        visited[startDFANode.getNodeSet()] = startDFANode.getNodeId();
        que.push(startNodeSet);
        while(!que.empty())
        {
            auto tempNodeset {que.front()};
            que.pop();
            DFANode inputDFANode{nodes[visited[tempNodeset]]};
            for(auto curEdgeStr : getNoEpsilonEdgeData(tempNodeset))
            {
                
                auto res {epsilonClosure(move(tempNodeset, curEdgeStr))};
                if (res.size() == 0)
                    continue;
                DFANode outputDFANode{};
                if (!visited.contains(res))
                {
                    outputDFANode = makeDFANode(res);
                    visited[outputDFANode.getNodeSet()] = outputDFANode.getNodeId();
                    que.push(res);
                }
                else
                {
                    outputDFANode = nodes[visited[res]];
                }
                
                makeDFAEdge(inputDFANode, outputDFANode, {curEdgeStr});
            }
        }

    }
    
    DFANode& makeDFANode(NodeSet nodeSet)
    {
        nodes.emplace_back(std::move(nodeSet), nodes.size());
        graph.push_back({});
        return nodes.back();
    }
    void makeDFAEdge(DFANode inputNodes, DFANode outputNodes, std::set<std::string> edgesData)
    {
        graph[inputNodes].emplace_back(inputNodes, outputNodes, edgesData);
    }

    std::set<std::string> getNoEpsilonEdgeData(NodeSet nodeSet)
    {
        std::set<std::string> res;
        for(auto node : nodeSet)
        {
            for(auto edge : nfa.getGraph()[node])
            {
                auto str {edge.getData()};
                if (str != Edge::epsilon)
                    res.insert(str);
            }
        }
        return res;
    }
    NodeSet epsilonClosure(Node curNode)
    {
        NodeSet visited;
        std::queue<Node> que;
        que.push(curNode);
        visited.insert(curNode);
        while(!que.empty())
        {
            auto tempNode {que.front()};
            que.pop();
            
            for(const auto &edge : nfa.getGraph()[tempNode])
            {
                auto outputNodeId {edge.getOutputId()};
                if (edge.getData() == Edge::epsilon && !visited.contains(outputNodeId))
                {
                    visited.insert(outputNodeId);
                    que.push(outputNodeId);
                }
            }
        }
        return visited;
    }
    NodeSet epsilonClosure(NodeSet curNodes)
    {
        NodeSet res{};
        for(auto curNode : curNodes)
        {
            res.merge(epsilonClosure(curNode));
        }
        return res;
    }

    NodeSet move(Node curNode, std::string ele)
    {
        NodeSet res{};
        for(const auto &edge : nfa.getGraph()[curNode])
        {
            if (edge.getData() == ele)
            {
                res.insert(edge.getOutputId());
            }
        }
        return res;
    }

    NodeSet move(NodeSet curNodes, std::string ele)
    {
        NodeSet res{};
        for(auto curNode : curNodes)
        {
            res.merge(move(curNode, ele));
        }
        return res;
    }

    NFA &nfa;
    std::vector<std::vector<DFAEdge>> graph;
    std::vector<DFANode> nodes;
};