//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "astar/AStarLiteClose.h"

AStarLiteClose::AStarLiteClose()
{
	clear();
}

AStarLiteClose::~AStarLiteClose()
{
}

void AStarLiteClose::clear()
{
	m_nodeMap.clear();
}

bool AStarLiteClose::insert(const AStarLiteNode& node)
{
	int nodeId = node.m_id;

	// check for duplicate insertion
	if(hasNode(nodeId))
		return false;

	// insert node
	m_nodeMap[nodeId] = node;
	return true;
}

bool AStarLiteClose::isEmpty() const
{
	return (size() == 0);
}

int AStarLiteClose::size() const
{
	return (int)m_nodeMap.size();
}

bool AStarLiteClose::hasNode(int nodeId)
{
	return (m_nodeMap.find(nodeId) != m_nodeMap.end());
}

bool AStarLiteClose::remove(int nodeId)
{
	// get node
	NodeMapIter nodeInfo = m_nodeMap.find(nodeId);

	// check if node exists
	if(nodeInfo == m_nodeMap.end())
		return false;

	// remove the node
	m_nodeMap.erase(nodeInfo);

	return true;
}

const AStarLiteNode* AStarLiteClose::search(int nodeId)
{
	// get node
	NodeMapIter nodeInfo = m_nodeMap.find(nodeId);

	// check if node exists
	if(nodeInfo == m_nodeMap.end())
		return NULL;

	// return node
	return &(nodeInfo->second);
}

vector<int> AStarLiteClose::constructPath(int start, int target)
{
	vector<int> path;
	path.clear();

	int nodeId = target;

	while(true)
	{
		path.push_back(nodeId);

		if(nodeId == start)
			break;

		const AStarLiteNode* node = search(nodeId);

		assert(node != NULL);
		nodeId = node->m_parent;
	}

	assert(path.size() > 0);
	//assert(*(path.begin()) == start);
	//assert(*(path.end() - 1) == target);
	assert(*(path.begin()) == target);
	assert(*(path.end() - 1) == start);

	return path;
}
