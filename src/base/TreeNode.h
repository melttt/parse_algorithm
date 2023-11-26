#pragma once
#include <vector>
#include <string>
#include <utility>
#include <memory>
#include "BasicNode.h"

class TreeNode
{
    public:
    
        TreeNode(std::string data):data(std::move(data)){};
        std::shared_ptr<TreeNode>& addChild(std::shared_ptr<TreeNode> child)
        {
            children.emplace_back(std::move(child));
            return children.back();
        }
        std::shared_ptr<TreeNode>& getChild(size_t index)
        {
            if (index >= children.size())
            {
                throw "error index!";
            }
            return children.at(index);
        }
        size_t getChildSize() const
        {
            return children.size();
        }
        std::vector<std::shared_ptr<TreeNode>>& getChildren()
        {
            return children;
        }
        void clearChildren()
        {
            children.clear();
        }

        void mergeLastGrandson()
        {
            if (auto childSize = getChildSize(); childSize > 0)
            {
                if (auto lastSon {children[childSize - 1]}; lastSon->getChildSize() == 1)
                {
                    auto lastGrandSon {lastSon->getChild(0)};
                    lastSon->clearChildren();
                    children.pop_back();
                    addChild(lastGrandSon);
                }
            }
        }
        std::string getData() const
        {
            return data;
        }
        bool isLeafNode()
        {
            return children.size() == 0;
        }
    private:
        std::string data;
        std::vector<std::shared_ptr<TreeNode>> children;
};

static std::shared_ptr<BasicNode> TreeNodeToBasicNode(std::shared_ptr<TreeNode> head)
{
    if (head)
    {
        auto curHead{std::make_shared<BasicNode>(head->getData())};
        for(auto child : head->getChildren())
        {
            curHead->addChildren(TreeNodeToBasicNode(child));
        }
        return curHead;
    }
    return nullptr;
}
static std::string totalEdgeInfo {};
static std::string totalNodeInfo {};


static std::string TreeNodeToDotStr(std::shared_ptr<TreeNode> head)
{
    static int nodeNum = 0;
    if (head)
    {
        std::string nodeId{"node" + std::to_string(nodeNum ++)};    
        std::string nodeInfo{nodeId + "[label=\"" + head->getData() + "\"]\n"};
        totalNodeInfo += nodeInfo;
        std::string edgeInfo1{nodeId + "->"};
        for(auto child : head->getChildren())
        {
            auto childNodeId = TreeNodeToDotStr(child);
            auto edgeInfo {edgeInfo1 + childNodeId + "\n"};
            totalEdgeInfo += edgeInfo;
        }
        return nodeId;
    }
    return {""};
}

static std::string ConvertDotStr(std::shared_ptr<TreeNode> head)
{
    std::string res {"digraph G{\n"};
    if (head)
    {
        TreeNodeToDotStr(head);
        res += totalNodeInfo;
        res += totalEdgeInfo;
    }
    res += "}";
    return res;
}