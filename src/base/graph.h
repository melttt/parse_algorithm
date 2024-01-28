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
                //todo change to const
                operator NodeId();
                auto operator<=>(const Node&) const = default;
                bool operator==(const Node&) const = default;
                NodeId getId();
                operator std::string();
            protected:
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
                auto operator<=>(const Edge&) const = default;
                bool operator==(const Edge&) const = default;
                Graph::Node getInDegreeNode() const;
                Graph::Node getOutDegreeNode() const;
                std::string getData() const;
                void setData(std::string tmp)
                {
                    if (data.size() > 0)
                    {
                        data[0] = tmp;
                    }
                    else
                    {
                        data.push_back(tmp);
                    }
                }
            protected:
                Node inDegreeNode, outDegreeNode;
                std::vector<std::string> data;
        };

        // add virtual dtor
        public:
            using NodeSet = std::set<Graph::Node>;
            using AdjacencyList = std::vector<std::vector<Graph::Edge>>;
            const Node& makeNewNode();
            void makeNewEdge(Node startNode, Node endNode, std::string data);
            std::string getDotStr();
            void setEndNode(Node node);
            void setStartNode(Node node);
            const NodeSet& getStartNode() const; 
            const NodeSet& getEndNode() const;
            bool isEndNode(Node node) const;
            bool isStartNode(Node node) const;
            const std::vector<Node>& getNodes() const;
            const AdjacencyList& getAdjacencyList() const;
        protected:
            std::vector<Node> nodes;
            NodeSet startNode, endNode;
            AdjacencyList adjacencyList;
    };
    







};