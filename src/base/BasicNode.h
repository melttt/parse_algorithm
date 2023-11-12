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

	std::string getData();
	void setData(std::string data);

protected:
	void setParent(BasicNode *nd);

	std::string _data;
	std::list<std::shared_ptr<BasicNode>> children;
	std::list<BasicNode *> _children;
	BasicNode * _parent;
};

#endif /* BASICNODE_H_ */