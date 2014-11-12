//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_SPATIAL_DATABASE_ITEM_H__
#define __STEERLIB_SPATIAL_DATABASE_ITEM_H__

/// @file SpatialDatabaseItem.h
/// @brief Declares the virtual interface inherited by any objects that want to be used by the SteerLib::GridDatabase2D.

#include "Globals.h"
#include "util/Geometry.h"

namespace SteerLib {

	/**
	 * @brief The virtual interface used by objects in the spatial database.
	 *
	 * Inherit this virtual interface if you want your object to be used by the spatial database.
	 * This way, the spatial database can use any objects without knowing their details.
	 *
	 * Geometry.h provides several intersection/overlap/penetration functions for rectangles and circles, 
	 * so in most cases, implementing these functions is just one or two lines of code.
	 *
	 * @see
	 *  - helpful utility functions in Geometry.h
	 *  - the GridDatabase2D spatial database
	 *  - examples of this virtual interface being used: RawAgentInfo, RawObstacleInfo.
	 *
	 */
	class STEERLIB_API SpatialDatabaseItem {
	public:
		/// Overriding this default (empty) destructor is optional.
		virtual ~SpatialDatabaseItem() {}
		/// Returns true if the object is an agent, false if not.
		virtual bool isAgent() = 0;
		/// Returns true if the object blocks line-of-sight.  Usually agents and invisible boundaries (like a pool or a street) should return false, while larger objects should return true.
		virtual bool blocksLineOfSight() = 0;
		/// An integer value that is added to the grid cells that this object overlaps.
		virtual float getTraversalCost() = 0;
		/// Returns true and update parameter t if the ray intersects your object.
		virtual bool intersects(const Util::Ray &r, float &t) = 0;
		/// Returns true if the circle (represented by Point p and the radius) overlaps your object.
		virtual bool overlaps(const Util::Point & p, float radius) = 0;
		/// Returns the amount of penetration that a circle has if it overlaps, or 0.0 if there is no overlap.
		virtual float computePenetration(const Util::Point & p, float radius) = 0;
	};

	typedef SpatialDatabaseItem* SpatialDatabaseItemPtr;


} // end namespace SteerLib


#endif
