//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file OrientedBoxObstacle.cpp
/// @brief Implements the OrientedBoxObstacle class.

#include "obstacles/OrientedWallObstacle.h"
// #include "testcaseio/ObstacleInitialConditions.h"

#include "util/DrawLib.h"

using namespace SteerLib;
using namespace Util;

#ifndef _WIN32
// win32 does not define "std::max", instead they define "max" as a macro.
// because of this, on unix we use "using std::max" so that the code only
// needs to use "max()" instead of "std::max()".  This way, the code
// works on both win32 and unix.
using std::max;
using std::min;
#endif


OrientedWallObstacle::OrientedWallObstacle ( Util::Point centerPosition, float lengthX, float lengthZ,
										  float ymin, float ymax, float thetaY,
										  double doorLocation, double doorRadius, float traversalCost ) :
										  OrientedBoxObstacle(centerPosition, lengthX, lengthZ,
										  ymin, ymax, thetaY, traversalCost )
{
	this->doorLocation = doorLocation;
	this->doorRadius = doorRadius;

	assert(thetaY == 0.0 && "No support for non-zero thetaY values yet");
	// std::vector<OrientedBoxObstacleInitialConditions *> obstacles;
	double oldXLength;
	oldXLength = (doorLocation+(lengthX/2.0)) - doorRadius;
	Util::Point oldPosition = centerPosition + Util::Vector(((oldXLength/2.0)-(lengthX/2.0)), 0.0, 0.0);
	OrientedBoxObstacle * obstacle = new OrientedBoxObstacle(oldPosition,
			oldXLength, lengthZ, ymin, ymax, thetaY, traversalCost );

	// obstacle->height = ymax-ymin;
	// o->lengthX = size.x;
	// obstacle->lengthX = (doorLocation+(lengthX/2.0)) - doorRadius;
	// o->lengthX = o->lengthX  * (1.0-(doorRadius/o->lengthX));
	// obstacle->lengthZ = lengthZ;
	// obstacle->position = centerPosition;
	// Util::Vector opos(((obstacle->lengthX/2.0)-(lengthX/2.0)), 0.0, 0.0);
	// obstacle->position = centerPosition + opos;
	// obstacle->position = centerPosition+Util::rotateInXZPlane(obstacle->position-centerPosition, thetaY);
	// obstacle->thetaY = thetaY;
	_wallSections.push_back(obstacle);


	double nextDoor = (lengthX) + doorRadius;
	double nextXLength = ((nextDoor - doorRadius) - ((doorLocation + (lengthX/2.0)) + 1));
	OrientedBoxObstacle *obstacle2 = new OrientedBoxObstacle(
			Util::Point(oldPosition.x + (nextXLength/2.0) + (doorRadius*2.0) + (oldXLength/2.0), oldPosition.y, oldPosition.z),
					nextXLength, lengthZ,
			  ymin, ymax, thetaY, traversalCost );
	// obstacle2->height = ymax-ymin;
	// o->lengthX = size.x;
	// obstacle2->lengthX = ((nextDoor - doorRadius) - ((doorLocation + (lengthX/2.0)) + 1));
	// o2->lengthX = o2->lengthX  * (1.0-(doorRadius/o2->lengthX));
	// obstacle2->lengthZ = lengthZ;
	// obstacle2->position = centerPosition;
	// obstacle2->position.x = obstacle->position.x + (obstacle2->lengthX/2.0) + (doorRadius*2.0) + (obstacle->lengthX/2.0);

	// obstacle2->thetaY = thetaY;

	_wallSections.push_back(obstacle2);
}

const Util::AxisAlignedBox & OrientedWallObstacle::getBounds()
{
	return this->_wallSections.at(0)->getBounds();

}

void OrientedWallObstacle::draw()
{
#ifdef ENABLE_GUI

	for (size_t i = 0; i < _wallSections.size(); i++)
	{
		_wallSections.at(i)->draw();
	}

#endif // ifdef ENABLE_GUI
}


