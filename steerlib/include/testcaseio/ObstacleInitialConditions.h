//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
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
#include "obstacles/PolygonObstacle.h"

namespace SteerLib {


	/**
	 * @brief The initial conditions of a single obstacle based on the input test case.
	 *
	 * Currently the only type of object supported by test cases is a simple axis-aligned box.
	 */
	struct ObstacleInitialConditions {
		virtual ObstacleInterface* createObstacle() = 0;
		virtual ~ObstacleInitialConditions() {}
	};

	struct BoxObstacleInitialConditions : public ObstacleInitialConditions
	{
		float xmin, xmax, ymin, ymax, zmin, zmax;
		BoxObstacleInitialConditions() {};
		BoxObstacleInitialConditions(float xmin_, float xmax_, float ymin_, float ymax_, float zmin_, float zmax_)
		{
			xmin=xmin_;
			xmax=xmax_;
			ymin=ymin_;
			ymax=ymax_;
			zmin=zmin_;
			zmax=zmax_;
		}
		BoxObstacleInitialConditions(const SteerLib::BoxObstacleInitialConditions & box)
		{
			xmin=box.xmin;
			xmax=box.xmax;
			ymin=box.ymin;
			ymax=box.ymax;
			zmin=box.zmin;
			zmax=box.zmax;
		}

		virtual ObstacleInterface* createObstacle() { return new SteerLib::BoxObstacle(xmin, xmax, ymin, ymax, zmin, zmax); }
	};
	
	inline std::ostream &operator<<(std::ostream &out, const BoxObstacleInitialConditions &a)
		{ // methods used here must be const
			out << "box initial conditions: (" << a.xmin << ", " << a.xmax << ", " << a.ymin <<
					", " << a.ymax << ", " << a.zmin << ", " << a.zmax << ")" <<  std::endl;
			return out;
		}

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

	struct PolygonObstacleInitialConditions : public ObstacleInitialConditions
	{
		std::vector<Util::Point> _vertices;
		PolygonObstacleInitialConditions() {};
		PolygonObstacleInitialConditions(const std::vector<Util::Point> & vertices)
		{
			for(int i=0; i<vertices.size(); i++)
				_vertices.push_back(vertices[i]);
		}
		
		virtual ObstacleInterface* createObstacle() { return new PolygonObstacle(_vertices); }
	};

	inline std::ostream &operator<<(std::ostream &out, const PolygonObstacleInitialConditions &a)
		{ // methods used here must be const
			out << "polygon initial conditions:"<<  std::endl;
			for(int i=0; i<a._vertices.size(); i++)
				out << "vertex-"<<i<<" (" << a._vertices[i].x << ", " << a._vertices[i].y << ", " << 
					a._vertices[i].z << ")" <<  std::endl;
			
			return out;
		}


} // end namespace SteerLib

#endif
