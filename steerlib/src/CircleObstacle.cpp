//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file CircleObstacle.cpp
/// @brief Implements the CircleObstacle class.

#include "obstacles/CircleObstacle.h"
#include "util/DrawLib.h"

using namespace SteerLib;
using namespace Util;

CircleObstacle::CircleObstacle ( Point centerPosition, float radius, float ymin, float ymax, float traversalCost )
{
	_radius = radius;
	_centerPosition = centerPosition;
	_traversalCost = traversalCost;

	// calculating bounds
	_bounds.ymin = ymin;
	_bounds.ymax = ymax;
	_bounds.xmin = _centerPosition.x - _radius;
	_bounds.xmax = _centerPosition.x + _radius;
	_bounds.zmin = _centerPosition.z - _radius;
	_bounds.zmax = _centerPosition.z + _radius;

	// calculating _blocksLineOfSight
	_blocksLineOfSight = (ymax > 0.7) ? true : false;

}

std::pair<std::vector<Util::Point>,std::vector<size_t> > CircleObstacle::getStaticGeometry()
{
	std::cout << "*****CircleObstacle:: get static geometry not implemented yet" << std::endl;
	std::vector<Util::Point> ps;
	std::vector<size_t> vs;
	return std::make_pair(ps, vs);
}

/**
 * Returns a set of points that can be used to define the bounding polygon
 * of this <code>CircleObstacle</code>.
 */
std::vector<Util::Point> CircleObstacle::getCirclePoints()
{

	Util::Vector offset(1.0,this->position().y,0.0); // because we work in x,z plane
	offset = offset * this->radius();
	std::vector<Util::Point> points;
	double thetaPart = float(360) / CIRCLE_SAMPLES;

	for (int i = 0; i < CIRCLE_SAMPLES; i++)
	{
		double theta = -radians((i*thetaPart)+45.0);
		Util::Point tmp_p = this->position() + rotateInXZPlane( offset, theta);
		points.push_back(tmp_p);
	}
	return points;
}

void CircleObstacle::draw() {
#ifdef ENABLE_GUI

	DrawLib::glColor(gDarkCyan);
	DrawLib::drawCylinder(_centerPosition,_radius,_bounds.ymin,_bounds.ymax);
	DrawLib::glColor(gBlack);
	DrawLib::drawBoxWireFrame(_bounds.xmin,_bounds.xmax,_bounds.ymin,_bounds.ymax,_bounds.zmin,_bounds.zmax);
#endif // ifdef ENABLE_GUI
}
