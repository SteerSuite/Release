//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_OBSTACLE_INTERFACE_H__
#define __STEERLIB_OBSTACLE_INTERFACE_H__

/// @file ObstacleInterface.h
/// @brief Declares the SteerLib::ObstacleInterface virtual interface.

#include "Globals.h"
#include "interfaces/SpatialDataBaseInterface.h"
#include "util/Geometry.h"

namespace SteerLib {

	/**
	 * @brief The virtual interface for any non-agent object in the environment.
	 */
	class STEERLIB_API ObstacleInterface : public SteerLib::SpatialDatabaseItem {
	public:
		ObstacleInterface() : nextObstacle_(NULL), prevObstacle_(NULL), id_(0), isConvex_(false) {}
		virtual ~ObstacleInterface() { }
		virtual void init() { }
		virtual void update(float timeStamp, float dt, unsigned int frameNumber) { }
		virtual void draw() = 0;
		virtual const Util::AxisAlignedBox & getBounds() = 0;
		virtual void setBounds(const Util::AxisAlignedBox & bounds) = 0;

		/// @name The SpatialDatabaseItem interface
		/// @brief This interface is kept pure abstract.
		//@{
		virtual bool isAgent() = 0;
		virtual bool blocksLineOfSight() = 0;
		virtual float getTraversalCost() = 0;
		virtual bool intersects(const Util::Ray &r, float &t) = 0;
		virtual bool overlaps(const Util::Point & p, float radius) = 0;
		virtual float computePenetration(const Util::Point & p, float radius) = 0;
		virtual std::pair<std::vector<Util::Point>,std::vector<size_t> > getStaticGeometry() = 0;
		virtual std::vector<Util::Point> get2DStaticGeometry() = 0;

	public:
		ObstacleInterface *nextObstacle_;
		Util::Point point_;
		ObstacleInterface *prevObstacle_;
		Util::Vector unitDir_;

		size_t id_;
		bool isConvex_;
		//@}
	};

} // end namespace SteerLib

#endif
