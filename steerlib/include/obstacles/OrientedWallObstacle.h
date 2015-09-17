//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_ORIENTED_WALL_OBSTACLE_H__
#define __STEERLIB_ORIENTED_WALL_OBSTACLE_H__

/// @file OrientedBoxObstacle.h
/// @brief Declares the OrientedBoxObstacle class
/// @todo
///   - remove the hard-coded arbitrary value for getTraversalCost


#include "obstacles/OrientedBoxObstacle.h"
#include "Globals.h"


namespace SteerLib
{


	class STEERLIB_API OrientedWallObstacle : public SteerLib::OrientedBoxObstacle
	{
	public:
		//OrientedBoxObstacle(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) : _bounds(xmin, xmax, ymin, ymax, zmin, zmax) { _blocksLineOfSight = (ymax > 0.7) ? true : false; }
		//OrientedBoxObstacle(const Util::AxisAlignedBox & newBounds ) : _bounds(newBounds) { _blocksLineOfSight = (newBounds.ymax > 0.7) ? true : false; }

		OrientedWallObstacle( Util::Point centerPosition, float lengthX, float lengthZ, float ymin, float ymax, float thetaY, double doorLocation, double doorRadius, float traversalCost=1001.0f);

		void draw(); // implementation in .cpp
		const Util::AxisAlignedBox & getBounds();

	protected:
		double doorLocation;
		double doorRadius;

	private :
		std::vector<OrientedBoxObstacle *> _wallSections;
	};

}
#endif

