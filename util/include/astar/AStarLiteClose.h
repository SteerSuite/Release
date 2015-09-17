//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#pragma once

#include "astar/AStarLiteNode.h"

#include <map>
#include <vector>
#include <cassert>
#include <stdlib.h> // For definition of NULL

class AStarLiteClose
{
public:
	AStarLiteClose();
	~AStarLiteClose();

	void clear();
	bool insert(const AStarLiteNode& node);
	bool isEmpty() const;
	int size() const;
	bool hasNode(int nodeId);
	bool remove(int nodeId);
	const AStarLiteNode* search(int nodeId);
	std::vector<int> constructPath(int start, int target);

private:
	typedef std::map<int,AStarLiteNode> NodeMap;
	typedef NodeMap::iterator NodeMapIter;

	NodeMap m_nodeMap;
};
