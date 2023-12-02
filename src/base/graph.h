#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <set>
namespace PARSE_UTIL
{
    using NodeId = size_t;
    class Graph
    {
        public:
        class Node
        {
            public:
                Node() = default;
                Node(NodeId id);
                operator NodeId();
                auto operator<=>(const Node&) const = default;
                bool operator==(const Node&) const = default;
                NodeId getId();
                Graph::Node::operator std::string();
            private:
                NodeId id{static_cast<NodeId>(-1ul)};
        };
        class Edge
        {
            public:
                static const inline std::string epsilon{"\u03B5"};
            public:
                Edge() = default;
                Edge(Node inDegreeNode, Node outDegreeNode, std::string data);
                Edge(Node inDegreeNode, Node outDegreeNode, std::vector<std::string> data);

                Graph::Node Graph::Edge::getInDegreeNode() const;
                Graph::Node Graph::Edge::getOutDegreeNode() const;
                std::string getData() const;
            private:
                Node inDegreeNode, outDegreeNode;
                std::vector<std::string> data;
        };


        public:
            using NodeSet = std::set<Graph::Node>;
            using AdjacencyList = std::vector<std::vector<Graph::Edge>>;
            const Node& makeNewNode();
            void makeNewEdge(Node startNode, Node endNode, std::string data);
            std::string getDotStr();
            inline void setEndNode(Node node);
            inline void setStartNode(Node node);
            inline const NodeSet& getStartNode() const; 
            inline const NodeSet& getEndNode() const;
            inline bool isEndNode(Node node) const;
            inline bool isStartNode(Node node) const;
            const std::vector<Node>& getNodes() const;
            const AdjacencyList& getAdjacencyList() const;
        private:
            

            std::vector<Node> nodes;
            NodeSet startNode, endNode;
            AdjacencyList adjacencyList;
    };
    







};