//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file BoxObstacle.cpp
/// @brief Implements the BoxObstacle class.

#include "obstacles/BoxObstacle.h"
#include "util/DrawLib.h"

using namespace SteerLib;
using namespace Util;

void BoxObstacle::draw() {
#ifdef ENABLE_GUI
	DrawLib::glColor(Util::gBlack);
	DrawLib::drawBoxWireFrame(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);
	DrawLib::glColor(Color(0.178f, 0.2896f, 0.3339));
	DrawLib::drawBox(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);
#endif // ifdef ENABLE_GUI
}

/**
 * Returns the points in the geometry and the indicies for the faces in the geometry
 */
std::pair<std::vector<Util::Point>,std::vector<size_t> > BoxObstacle::getStaticGeometry()
{
	std::vector<size_t> triVerts;

	std::vector<Util::Point> verts;
	Point topLeft(_bounds.xmin, _bounds.ymin-0.01f, _bounds.zmin);
	verts.push_back(topLeft); //0
	Point topRight(_bounds.xmax, _bounds.ymin-0.01f, _bounds.zmin);
	verts.push_back(topRight); //1
	Point botLeft(_bounds.xmin, _bounds.ymin-0.01f, _bounds.zmax);
	verts.push_back(botLeft); //2
	Point botRight(_bounds.xmax, _bounds.ymin-0.01f, _bounds.zmax);
	verts.push_back(botRight); //3

	Point topLefth(_bounds.xmin, _bounds.ymax, _bounds.zmin);
	verts.push_back(topLefth); //4
	Point topRighth(_bounds.xmax, _bounds.ymax, _bounds.zmin);
	verts.push_back(topRighth); //5
	Point botLefth(_bounds.xmin, _bounds.ymax, _bounds.zmax);
	verts.push_back(botLefth); //6
	Point botRighth(_bounds.xmax, _bounds.ymax, _bounds.zmax);
	verts.push_back(botRighth); //7



	// upper/lower plane
	DrawLib::drawQuad(botLefth, botRighth, topRighth, topLefth);
	triVerts.push_back(6);
	triVerts.push_back(7);
	triVerts.push_back(5);

	triVerts.push_back(6);
	triVerts.push_back(5);
	triVerts.push_back(4);
	DrawLib::drawQuad(topLeft, topRight, botRight, botLeft);
	triVerts.push_back(0);
	triVerts.push_back(1);
	triVerts.push_back(3);

	triVerts.push_back(0);
	triVerts.push_back(3);
	triVerts.push_back(2);

	// top/bot sides
	DrawLib::drawQuad(topLeft, topLefth, topRighth, topRight);
	triVerts.push_back(0);
	triVerts.push_back(4);
	triVerts.push_back(5);

	triVerts.push_back(0);
	triVerts.push_back(5);
	triVerts.push_back(1);

	DrawLib::drawQuad(botRight, botRighth, botLefth, botLeft);
	triVerts.push_back(3);
	triVerts.push_back(7);
	triVerts.push_back(6);

	triVerts.push_back(3);
	triVerts.push_back(6);
	triVerts.push_back(2);

	// left/right sides
	DrawLib::drawQuad(botLeft, botLefth, topLefth, topLeft);
	triVerts.push_back(2);
	triVerts.push_back(6);
	triVerts.push_back(4);

	triVerts.push_back(2);
	triVerts.push_back(4);
	triVerts.push_back(0);

	DrawLib::drawQuad(topRight, topRighth, botRighth, botRight);
	triVerts.push_back(1);
	triVerts.push_back(5);
	triVerts.push_back(7);

	triVerts.push_back(1);
	triVerts.push_back(7);
	triVerts.push_back(3);

	return std::make_pair(verts, triVerts);

}
