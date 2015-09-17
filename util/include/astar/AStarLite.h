//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#pragma once

#include <vector>

#include "astar/Environment.h"
#include "astar/AStarLiteNode.h"
#include "astar/AStarLiteOpen.h"
#include "astar/AStarLiteClose.h"
#include "UtilGlobals.h"

#include <cassert>


class UTIL_API AStarLite
{
public:

	AStarLite();
	~AStarLite();

	bool findPath(const Environment& env, int start, int target);

    const vector<int>& getPath() const;

private:

    static const int NO_NODE = -1;

	vector<int> m_path;

};
