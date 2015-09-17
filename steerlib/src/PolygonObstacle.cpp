//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * PolygonObstacle.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: gberseth
 */

#include "obstacles/PolygonObstacle.h"
#include "util/DrawLib.h"

PolygonObstacle::PolygonObstacle(std::vector<Util::Point> points, float traversalCost)
{
	// TODO Auto-generated constructor stub
	_traversalCost = traversalCost;
	_points = points;

	_centerPosition = Util::Point(0,0,0);
	_radius = 1.0;
	float ymin = 0.0;
	float ymax = 1.0;
	_bounds.ymin = ymin;
	_bounds.ymax = ymax;
	_bounds.xmin = _centerPosition.x - _radius;
	_bounds.xmax = _centerPosition.x + _radius;
	_bounds.zmin = _centerPosition.z - _radius;
	_bounds.zmax = _centerPosition.z + _radius;

}

PolygonObstacle::~PolygonObstacle()
{
	// TODO Auto-generated destructor stub
}

void PolygonObstacle::draw()
{

	/*
	for (size_t _vert=0; _vert < this->_points.size()-1; _vert++)
	{
		Util::DrawLib::drawLine(this->_points.at(_vert),
				this->_points.at(_vert+1), Util::gDarkMagenta, 3.0);
	}
	Util::DrawLib::drawLine(this->_points.at(0),
			this->_points.at(this->_points.size()-1), Util::gDarkMagenta, 3.0);
			*/
}

std::pair<std::vector<Util::Point>,std::vector<size_t> > PolygonObstacle::getStaticGeometry()
{
	std::cout << "*****PolygonObstacle:: get static geometry not implemented yet" << std::endl;
	std::vector<Util::Point> ps;
	std::vector<size_t> vs;
	return std::make_pair(ps, vs);
}
