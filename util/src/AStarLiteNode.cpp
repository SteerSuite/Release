//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "astar/AStarLiteNode.h"

AStarLiteNode::AStarLiteNode()
{
}

AStarLiteNode::AStarLiteNode(int id, int parent, float g, float h)
	: m_id(id),
	  m_parent(parent),
	  m_g(g),
	  m_h(h),
	  m_f(g+h)
{
}

AStarLiteNode::~AStarLiteNode()
{
}




AStarLiteNode::Compare::~Compare()
{
}

bool AStarLiteNode::Compare::operator()(const AStarLiteNode& node1, const AStarLiteNode& node2)
{
	float f1 = node1.m_f;
    float f2 = node2.m_f;
    if (f1 != f2)
        return (f1 < f2);
    float g1 = node1.m_g;
    float g2 = node2.m_g;
    return (g1 > g2);
}

