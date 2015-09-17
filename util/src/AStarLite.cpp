//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "astar/AStarLite.h"
#include <iostream>

AStarLite::AStarLite()
{
	m_path.clear();
}

AStarLite::~AStarLite()
{
}

bool AStarLite::findPath(const Environment& env, int start, int target)
{
	// input sanitize
	assert(env.isValidNodeId(start));
	assert(env.isValidNodeId(target));

	// open and close lists
	AStarLiteOpen openList;
	AStarLiteClose closedList;

	// initialize search
	openList.clear();
	closedList.clear();
	float heuristic = env.getHeuristic(start, target);
	vector<Environment::Successor> successors;

	// create start node
	AStarLiteNode startNode(start, NO_NODE, 0, heuristic);

	// start search
	openList.insert(startNode);

	AStarLiteNode bestHeuristicNode = startNode;

	while(! openList.isEmpty())
	{
		// get the best node from the open list
		// AStarLiteOpen automatically puts that node on top
		AStarLiteNode node = openList.pop();

		// if we found the target, wrap up search and leave
		if (node.m_id == target)
		{
			closedList.insert(node);
			m_path = closedList.constructPath(start, target);
			break;
		}

		// else, get and search down the node's successors
		env.getSuccessors(node.m_id, NO_NODE, successors);

		for (vector<Environment::Successor>::const_iterator i = successors.begin(); i != successors.end(); i++)
		{
			// successorg: the cost to go from this node to the sucessor i
			// successorId: the successor's id
			float successorg = node.m_g + i->m_cost;
			int successorId = i->m_target;

			// check if successor is in the open or closed lists
			// if it is, and the new successor g is better than the old one, remove it from the open or closed list
			if (openList.hasNode(successorId))
			{
				if (openList.search(successorId)->m_g > successorg)
					openList.remove(successorId);
				else
					continue;
			}
			else if (closedList.hasNode(successorId))
			{
				if (closedList.search(successorId)->m_g > successorg)
					closedList.remove(successorId);
				else
					continue;
			}


			//std::cerr << " - COST to reach node " << successorId << " is " << successorg << "\n";

			// the successor is either new or has a better parent path, so add it to the open list
			float successorHeuristic = env.getHeuristic(successorId, target);

			//std::cerr << " - F + G (for " << successorId << ") is " << successorg + successorHeuristic << "\n\n";

			AStarLiteNode successorNode(successorId, node.m_id, successorg, successorHeuristic);

			openList.insert(successorNode);
		}

		// done expanding node; add to closed list
		closedList.insert(node);

		// if open list is empty at this point, we failed to find a proper solution, so construct whatever is possible.
		if (node.m_f < bestHeuristicNode.m_f) {
			bestHeuristicNode = node;
		}
		if (openList.isEmpty()) {
			m_path = closedList.constructPath(start, bestHeuristicNode.m_id);
			break;
		}
	}
	return true;
}


const vector<int>& AStarLite::getPath() const
{
	return m_path;
}
