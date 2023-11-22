/*
 * MathNode.h
 *
 *  Created on: 23 ago. 2019
 *      Author: asens
 */

#ifndef BASICNODE_H_
#define BASICNODE_H_

#include <iostream>
#include <list>
#include <memory>

class BasicNode {
public:
	BasicNode();
	BasicNode(std::string data);

	virtual ~BasicNode();

	void addChildren(std::shared_ptr<BasicNode> nd);
	std::list<BasicNode *> getChildren();
	std::shared_ptr<BasicNode> getChildrenByIndex(size_t index)
	{
		auto it {children.begin()};
		while(index -- != 0)
		{
			it ++;
		}
		return *it;
	}
	std::string getData();
	void setData(std::string data);
	void clearChildren()
	{
		children.pop_back();
		_children.pop_back();
	}
protected:
	void setParent(BasicNode *nd);

	std::string _data;
	std::list<std::shared_ptr<BasicNode>> children;
	std::list<BasicNode *> _children;
	BasicNode * _parent;
};

#endif /* BASICNODE_H_ */