#pragma once
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include "NFA.h"

using NodeSet = std::set<Node>;
using MinDFASet = std::set<Node>;
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
        buildEndNodes();
        minimization();
    }
    void minimization()
    {
        buildOutDegreeGraphAndSigma();
        std::set<MinDFASet> partition;
        std::set<MinDFASet> workList;
        MinDFASet partition1,partition2;
        for(auto node : nodes)
        {
            if (isFinalNode(node.getNodeSet()))
            {
                partition1.insert(node.getNodeId());
            }
            else
            {
                partition2.insert(node.getNodeId());
            }
        }
        partition.insert(partition1);
        partition.insert(partition2);
        workList = partition;

        while(!workList.empty())
        {
            auto s {*workList.begin()};
            workList.erase(s);
            for(auto c : sigma)
            {
                //calc image
                auto image {makeImage(c, s)};
                auto tempPartition{partition};
                for(auto q : tempPartition)
                {
                    decltype(image) q1{},q2{};
                    std::set_intersection(image.begin(), image.end(), q.begin(), q.end(), std::inserter(q1, q1.begin()));
                    std::set_difference(q.begin(), q.end(), q1.begin(), q1.end(), std::inserter(q2, q2.begin()));

                    if (!q1.empty() && !q2.empty())
                    {
                        partition.erase(q);
                        partition.insert(q1);
                        partition.insert(q2);

                        if (workList.contains(q))
                        {
                            workList.erase(q);
                            workList.insert(q1);
                            workList.insert(q2);
                        }
                        else if(q1.size() <= q2.size())
                        {
                            workList.insert(q1);
                        }
                        else
                        {
                            workList.insert(q2);
                        }
                    }
                }

            }
        }


        for(size_t index = 0;auto part : partition)
        {
            std::cout << "min node index" << index ++ << " ";
            for(auto pa : part)
            {
                std::cout << pa.getId() << ", ";
            }
            std::cout << std::endl;
        }

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
        for(auto edges : inDdgreeGraph)
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
    MinDFASet makeImage(std::string edgeData, MinDFASet s)
    {
        MinDFASet ret;
        for(auto node : s)
        {
            for(auto edge : outDdgreeGraph[node])
            {
                if(edge.getData() == edgeData)
                {
                    ret.insert(edge.getInputId());
                }
            }
        }
        return ret;
    }
    void buildOutDegreeGraphAndSigma()
    {
        for(auto node : nodes)
        {
            outDdgreeGraph.push_back({});
        }
        for(auto edges : inDdgreeGraph)
        {
            for(auto edge : edges)
            {
                sigma.insert(edge.getData());
                outDdgreeGraph[edge.getOutputId()].emplace_back(edge);
            }
        }
    }
    void buildEndNodes()
    {
        for(size_t index = 0; auto node : nodes)
        {
            if(isFinalNode(node.getNodeSet()))
            {
                endNodesIndex.insert(index);
            }
            index ++;
        }
    }
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
        inDdgreeGraph.push_back({});
        return nodes.back();
    }
    void makeDFAEdge(DFANode inputNodes, DFANode outputNodes, std::set<std::string> edgesData)
    {
        inDdgreeGraph[inputNodes].emplace_back(inputNodes, outputNodes, edgesData);
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
    std::vector<std::vector<DFAEdge>> inDdgreeGraph;
    std::vector<std::vector<DFAEdge>> outDdgreeGraph;
    std::vector<DFANode> nodes;
    std::set<std::string> sigma;
    std::set<size_t> endNodesIndex;
};