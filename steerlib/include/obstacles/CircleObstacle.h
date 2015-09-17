//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_CIRCLE_OBSTACLE_H__
#define __STEERLIB_CIRCLE_OBSTACLE_H__

/// @file CircleObstacle.h
/// @brief Declares the CircleObstacle class
/// @todo
///   - remove the hard-coded arbitrary value for getTraversalCost


#include "interfaces/ObstacleInterface.h"
#include "Globals.h"

#define CIRCLE_SAMPLES 35

namespace SteerLib {


	class STEERLIB_API CircleObstacle : public SteerLib::ObstacleInterface
	{
	public:
		CircleObstacle ( Util::Point centerPosition, float radius, float ymin, float ymax, float traversalCost=1001.0f );

		// ObstacleInterface functionality (not all virtual functions were overridden here)
		void draw(); // implementation in .cpp
		const Util::AxisAlignedBox & getBounds() { return _bounds; }
		virtual void setBounds(const Util::AxisAlignedBox & bounds) { _bounds = bounds; }

		/// @name The SpatialDatabaseItem interface
		/// @brief The CircleObstacle implementation of this interface represents a box that blocks line of sight if it is taller than 0.5 meter, and cannot be traversed.
		//@{
		virtual bool isAgent() { return false; }
		bool blocksLineOfSight() { return _blocksLineOfSight; }
		float getTraversalCost() { return _traversalCost; }

		virtual bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_centerPosition,_radius, r, t); }
		virtual bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D(_centerPosition,_radius,p, radius); }
		virtual float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D(_centerPosition, _radius, p, radius); }
		virtual std::pair<std::vector<Util::Point>,std::vector<size_t> > getStaticGeometry();
		virtual std::vector<Util::Point> get2DStaticGeometry()
		{
			return getCirclePoints();
		}

		Util::Point position() { return this->_centerPosition; }
		float radius() { return this->_radius; }

		std::vector<Util::Point> getCirclePoints();
		//@}

	protected:
		float _radius;
		Util::Point _centerPosition;

		Util::AxisAlignedBox _bounds;
		bool _blocksLineOfSight;
		float _traversalCost;
	};

}
#endif

