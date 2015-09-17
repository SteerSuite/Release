//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "astar/AStarLiteOpen.h"

AStarLiteOpen::AStarLiteOpen()
{
	clear();
}

AStarLiteOpen::~AStarLiteOpen()
{
}

void AStarLiteOpen::clear()
{
	m_openSet.clear();
	m_nodeMap.clear();
}

bool AStarLiteOpen::insert(const AStarLiteNode& node)
{
	int nodeId = node.m_id;

	// can't have multiple nodes with same id
	if(hasNode(nodeId))
		return false;

	// insert
	NodeSetIter nodeIter = m_openSet.insert(node);
	m_nodeMap[nodeId] = nodeIter;

	return true;
}

bool AStarLiteOpen::isEmpty() const
{
	return (size() == 0);
}

int AStarLiteOpen::size() const
{
	return (int)m_openSet.size();
}

AStarLiteNode AStarLiteOpen::pop()
{
	// can't pop if there's nothing to pop
	assert(!isEmpty());

	// pop top of stack
	AStarLiteNode node = *(m_openSet.begin());
	m_openSet.erase(m_openSet.begin());
	m_nodeMap.erase(node.m_id);

	return node;
}

bool AStarLiteOpen::hasNode(int nodeId)
{
	return (m_nodeMap.find(nodeId) != m_nodeMap.end());
}

bool AStarLiteOpen::remove(int nodeId)
{
	// find node
	NodeMapIter nodeInfo = m_nodeMap.find(nodeId);

	// if node not found, return false
	if(nodeInfo == m_nodeMap.end())
		return false;

	// clear out node
	m_openSet.erase(nodeInfo->second);
	m_nodeMap.erase(nodeInfo);

	return true;
}

const AStarLiteNode* AStarLiteOpen::search(int nodeId)
{
	// find node
	NodeMapIter nodeInfo = m_nodeMap.find(nodeId);

	// if node not found, return false
	if(nodeInfo == m_nodeMap.end())
		return false;

	// return node
	return &(*(nodeInfo->second));
}
