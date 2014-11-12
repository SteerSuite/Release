//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file OrientedBoxObstacle.cpp
/// @brief Implements the OrientedBoxObstacle class.

#include "obstacles/OrientedBoxObstacle.h"
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


OrientedBoxObstacle::OrientedBoxObstacle ( Util::Point centerPosition, float lengthX, float lengthZ, 
										  float ymin, float ymax, float thetaY, float traversalCost )
{
	_centerPosition = centerPosition;
	_lengthX = lengthX;
	_lengthZ = lengthZ;
	_thetaY = thetaY * M_PI / 180.0f; // radians 

	// calculating a,b,c,d
	// a ..... b 
	//
	// d ..... c


	_a.x = - _lengthX/2.0f;
	_a.y = 0.0f;
	_a.z = - _lengthZ/2.0f;

	_b.x = _lengthX/2.0f;
	_b.y = 0.0f;
	_b.z = - _lengthZ/2.0f;

	_c.x = _lengthX/2.0f;
	_c.y = 0.0f;
	_c.z =  _lengthZ/2.0f;

	_d.x = - _lengthX/2.0f;
	_d.y = 0.0f;
	_d.z = _lengthZ/2.0f;



	_a = Util::rotateInXZPlane ( _a, _thetaY );
	_b = Util::rotateInXZPlane ( _b, _thetaY );
	_c = Util::rotateInXZPlane ( _c, _thetaY );
	_d = Util::rotateInXZPlane ( _d, _thetaY );

	_a.x = _a.x + centerPosition.x;
	_a.z = _a.z + centerPosition.z;
	_b.x = _b.x + centerPosition.x;
	_b.z = _b.z + centerPosition.z;
	_c.x = _c.x + centerPosition.x;
	_c.z = _c.z + centerPosition.z;
	_d.x = _d.x + centerPosition.x;
	_d.z = _d.z + centerPosition.z;

	// 

	// calculating bounds. 
	// MUBBASIR TODO 
	_bounds.ymax = ymax;
	_bounds.ymin = ymin;

	_bounds.xmax = max ( max (_a.x, _b.x), max(_c.x, _d.x) );
	_bounds.xmin = min ( min (_a.x, _b.x), min(_c.x, _d.x) );
	_bounds.zmax = max ( max (_a.z, _b.z), max(_c.z, _d.z) );
	_bounds.zmin = min ( min (_a.z, _b.z), min(_c.z, _d.z) );


	// calculating _blocksLineOfSight
	_blocksLineOfSight = (ymax > 0.7) ? true : false;


	// calculating dummy bounds -- assuming origin is shifted to _centerPosition and axes are rotated by _thetaY radians
	_dummyBounds.ymax = ymax;
	_dummyBounds.ymin = ymin;

	_dummyBounds.xmax = lengthX / 2.0f;
	_dummyBounds.xmin = -lengthX / 2.0f;
	_dummyBounds.zmax =  lengthZ / 2.0f;
	_dummyBounds.zmin =  -lengthZ / 2.0f;

	_traversalCost = traversalCost;
}

void OrientedBoxObstacle::draw() {
#ifdef ENABLE_GUI
	DrawLib::glColor(gBlack);
	DrawLib::drawBoxWireFrame(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);

	Point topLeft(_a.x, _bounds.ymin-0.01f, _a.z);
	Point topRight(_b.x, _bounds.ymin-0.01f, _b.z);
	Point botLeft(_d.x, _bounds.ymin-0.01f, _d.z);
	Point botRight(_c.x, _bounds.ymin-0.01f, _c.z);

	Point topLefth(_a.x, _bounds.ymax, _a.z);
	Point topRighth(_b.x, _bounds.ymax, _b.z);
	Point botLefth(_d.x, _bounds.ymax, _d.z);
	Point botRighth(_c.x, _bounds.ymax, _c.z);

	// add a 3-d wireframe to it just to be interesting
	DrawLib::glColor(gBlack);
	DrawLib::drawLine(topRight,topRighth);
	DrawLib::drawLine(topLeft,topLefth);
	DrawLib::drawLine(botLeft,botLefth);
	DrawLib::drawLine(botRight,botRighth);
	DrawLib::drawLine(topRighth, topLefth);
	DrawLib::drawLine(topLefth, botLefth);
	DrawLib::drawLine(botLefth, botRighth);
	DrawLib::drawLine(botRighth, topRighth);

	DrawLib::glColor(gDarkCyan);
	// upper plane
	DrawLib::drawQuad(botLefth, botRighth, topRighth, topLefth);
	// top/bot sides
	DrawLib::drawQuad(topLeft, topLefth, topRighth, topRight);
	DrawLib::drawQuad(botRight, botRighth, botLefth, botLeft);
	// left/right sides
	DrawLib::drawQuad(botLeft, botLefth, topLefth, topLeft);
	DrawLib::drawQuad(topRight, topRighth, botRighth, botRight);

#endif // ifdef ENABLE_GUI
}
