#pragma once
#include "TreeNode.h"
class NFA
{
    class Node
    {
        public:
            int id;
    };
    class Edge
    {
        public:
            int inputId;
            int outputId;
            std::string data;
    };
    public:

        std::vector<std::vector<Edge>> graph;
    private:
};