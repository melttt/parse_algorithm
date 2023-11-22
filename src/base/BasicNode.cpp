/*
 * MathNode.cpp
 *
 *  Created on: 23 ago. 2019
 *      Author: asens
 */

#include "BasicNode.h"

BasicNode::BasicNode() {
	_parent = nullptr;

}

BasicNode::BasicNode(std::string data) {
	_parent = nullptr;
	_data   = data;
}

BasicNode::~BasicNode() {
}

void BasicNode::addChildren(std::shared_ptr<BasicNode> nd) {
	children.push_back(nd);
	_children.push_back(nd.get());
	//nd->setParent(this);
}

std::list<BasicNode*> BasicNode::getChildren() {
	return _children;
}

std::string BasicNode::getData() {
	return _data;
}

void BasicNode::setData(std::string data) {
	_data = data;
}

void BasicNode::setParent(BasicNode *nd) {
	_parent = nd;
}