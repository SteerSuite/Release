//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#pragma once

#include <cassert>

using namespace std;

class AStarLiteNode
{
public:
	AStarLiteNode();
	AStarLiteNode(int id, int parent, float g, float h);
	~AStarLiteNode();

	class Compare
	{
	public:
		virtual ~Compare();

		bool operator()(const AStarLiteNode& node1, const AStarLiteNode& node2);
	};

	int m_id;		// node id
	int m_parent;	// parent node id
	float m_g;		// actual cost to reach this node
	float m_h;		// estimate cost from this node to end node
	float m_f;		// g + h


private:

};
