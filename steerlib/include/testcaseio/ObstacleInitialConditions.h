//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_OBSTACLE_INITIAL_CONDITIONS_H__
#define __STEERLIB_OBSTACLE_INITIAL_CONDITIONS_H__

/// @file ObstacleInitialConditions.h
/// @brief Declares the data structures relevant to an obstacles's initial conditions.

#include "Globals.h"
#include "obstacles/BoxObstacle.h"
#include "obstacles/CircleObstacle.h"
#include "obstacles/OrientedBoxObstacle.h"
#include "obstacles/OrientedWallObstacle.h"

namespace SteerLib {


	/**
	 * @brief The initial conditions of a single obstacle based on the input test case.
	 *
	 * Currently the only type of object supported by test cases is a simple axis-aligned box.
	 */
	struct ObstacleInitialConditions {
		virtual ObstacleInterface* createObstacle() = 0;
	};

	struct BoxObstacleInitialConditions : public ObstacleInitialConditions
	{
		float xmin, xmax, ymin, ymax, zmin, zmax;
		BoxObstacleInitialConditions() {};
		BoxObstacleInitialConditions(float xmin_, float xmax_, float ymin, float ymax, float zmin, float zmax)
		{
			xmin=xmin_;
		}

		virtual ObstacleInterface* createObstacle() { return new SteerLib::BoxObstacle(xmin, xmax, ymin, ymax, zmin, zmax); }
	};

	struct CircleObstacleInitialConditions : public ObstacleInitialConditions {
		Util::Point position;
		float radius;
		float height;

		virtual ObstacleInterface* createObstacle() { return new SteerLib::CircleObstacle(position, radius, position.y, position.y+height); }
	};

	struct OrientedBoxObstacleInitialConditions : public ObstacleInitialConditions {
		Util::Point position;
		float lengthX, lengthZ, height, thetaY;

		virtual ObstacleInterface* createObstacle() { return new SteerLib::OrientedBoxObstacle(position, lengthX, lengthZ, position.y, position.y+height, thetaY); }
	};

	struct OrientedWallObstacleInitialConditions : public OrientedBoxObstacleInitialConditions {
		double doorLocation, doorRadius;

		virtual ObstacleInterface* createObstacle() { return new SteerLib::OrientedWallObstacle(position, lengthX, lengthZ, position.y, position.y+height, thetaY, doorLocation, doorRadius); }
	};


} // end namespace SteerLib

#endif
