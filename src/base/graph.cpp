#include "graph.h"

using namespace PARSE_UTIL;

Graph::Node::Node(NodeId id):id{id}{}
Graph::Node::operator NodeId()
{
    return id;
}
NodeId Graph::Node::getId()
{
    return id;
}

Graph::Node::operator std::string()
{
    return "node" + std::to_string(id);
}

Graph::Edge::Edge(Node inDegreeNode, Node outDegreeNode, std::string data)
:inDegreeNode(inDegreeNode), outDegreeNode{outDegreeNode}, data{data}{}

Graph::Edge::Edge(Node inDegreeNode, Node outDegreeNode, std::vector<std::string> data)
:inDegreeNode(inDegreeNode), outDegreeNode{outDegreeNode}, data{data}{}

Graph::Node Graph::Edge::getInDegreeNode() const
{
    return inDegreeNode;
}

Graph::Node Graph::Edge::getOutDegreeNode() const
{
    return outDegreeNode;
}
std::string Graph::Edge::getData() const
{
    return *data.begin();
}

const Graph::AdjacencyList& Graph::getAdjacencyList() const
{
    return adjacencyList;
}
const Graph::Node& Graph::makeNewNode()
{
    nodes.emplace_back(nodes.size());
    adjacencyList.push_back({});
    return nodes.back();
}
void Graph::makeNewEdge(Node startNode, Node endNode, std::string data)
{
    adjacencyList.at(startNode).emplace_back(startNode, endNode, std::move(data));
}

bool Graph::isEndNode(Node node) const
{
    return endNode.contains(node);
}
bool Graph::isStartNode(Node node) const
{
    return startNode.contains(node);
}
void Graph::setEndNode(Node node)
{
    endNode.insert(node);
}
void Graph::setStartNode(Node node)
{
    startNode.insert(node);
}
const Graph::NodeSet& Graph::getStartNode() const
{
    return startNode;
}
const Graph::NodeSet& Graph::getEndNode() const
{
    return endNode;
}
const std::vector<Graph::Node>& Graph::getNodes() const
{
    return nodes;
}

std::string Graph::getDotStr()
{
    std::string res {"digraph G{\n"};
    for(auto node : nodes)
    {
        decltype(res) nodeId{node};
        auto finalInfo {isEndNode(node) ? ", shape=doublecircle": ", shape=circle"};
        auto nodeInfo {"[label=\"" + std::to_string(node) + "\"" + finalInfo + "]\n"};
        res += nodeId + nodeInfo;
    }

    for(auto edges : adjacencyList)
    {
        for(Graph::Edge edge : edges)
        {
            decltype(res) inputIdStr{edge.getInDegreeNode()};
            decltype(res) outputIdStr{edge.getOutDegreeNode()};
            auto edgeInfo{inputIdStr + "->" + outputIdStr + "[label=\"" + edge.getData() + "\"]\n"};
            res += edgeInfo;
        }
    }
    res += "}";
    return res;
}