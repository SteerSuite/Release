//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_SPATIALDATABASE_INTERFACE_H__
#define __STEERLIB_SPATIALDATABASE_INTERFACE_H__

/// @file BenchmarkTechniqueInterface.h
/// @brief Declares the SteerLib::BenchmarkTechniqueInterface virtual interface.

#include "Globals.h"
#include "interfaces/SpatialDatabaseItem.h"
#include "util/Geometry.h"

#include <set>
#include <stack>
#include <vector>

using namespace Util;

// forward declaration
class MTRand;

namespace SteerLib {

	/**
	 * @brief The basic interface for a benchmark technique
	 *
	 * Inherit this virtual interface to implement a benchmark technique.
	 * If possible, it would be a good idea to keep the benchmark technique in a state where the scores and details
	 * can be returned after any number of frames, even if the simulation did not finish.
	 *
	 */
	class STEERLIB_API SpatialDataBaseInterface
	{
	protected:
		SpatialDataBaseInterface() {}
	public:
		virtual ~SpatialDataBaseInterface() {}
		/// @name Accessor functions
		//@{
		/// Returns the x value of the "top-left" corner of the database.
		virtual inline float getOriginX() = 0;
		/// Returns the z value of the "top-left" corner of the database.
		virtual inline float getOriginZ() = 0;
		/// Returns the total size of the database along the x direction.
		virtual inline float getGridSizeX() = 0;
		/// Returns the total size of the database along the z direction.
		virtual inline float getGridSizeZ() = 0;
		/// Returns the size of one grid cell along the x direction.
		virtual inline float getCellSizeX() = 0;
		/// Returns the size of one grid cell along the z direction.
		virtual inline float getCellSizeZ() = 0;
		/// Returns the number of grid cells along the x direction.
		virtual inline unsigned int getNumCellsX() = 0;
		/// Returns the number of grid cells along the a direction.
		virtual inline unsigned int getNumCellsZ() = 0;
		//@}

		/// @name Conversions between index, location, and grid coordinates
		//@{
		/// Returns an integer index of the GridCell where (x,z) is located.
		virtual inline int getCellIndexFromLocation( float x, float z) = 0;
		/// Returns an integer index of the GridCell where Point v is located.
		virtual inline int getCellIndexFromLocation( const Util::Point &v ) = 0;
		/// Returns the location of the center of the GridCell indexed by cellIndex; the "return value" is placed in the result arg.
		virtual inline void getLocationFromIndex( unsigned int cellIndex, Util::Point & result ) = 0;
		/// Returns 2-D <b>integer</b> index coordinates of a GridCell indexed by cellIndex.
		virtual inline void getGridCoordinatesFromIndex(unsigned int cellIndex, unsigned int &xIndex, unsigned int & zIndex) = 0;
		/// Returns the index of the GridCell that is indexed by 2-D integer coordinates (x,z).
		virtual inline unsigned int getCellIndexFromGridCoords(unsigned int x, unsigned int z) = 0;
		//@}

		/// @name Database update functions
		//@{
		/// Adds an object to the database
		virtual void addObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & newBounds ) = 0;
		/// Removes an object from the database.  <b>It is the user's responsibility to make sure oldBounds is correct.</b>
		virtual void removeObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox &oldBounds ) = 0;
		/// Updates an existing object in the database.  <b>It is the user's responsibility to make sure oldBounds is correct.</b>
		virtual void updateObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & oldBounds, const Util::AxisAlignedBox & newBounds ) = 0;
		/// clear Database
		virtual void clearDatabase() = 0;
		//@}

		/// @name Traversability queries
		//@{
		/// Returns true if there are any objects referenced in the GridCell.
		virtual inline bool hasAnyItems( unsigned int cellIndex )  = 0;
		/// Returns true if there are any objects referenced in the GridCell.
		virtual inline bool hasAnyItems( unsigned int x, unsigned int z ) = 0;
		/// Returns the sum total of traversal costs of all objects referenced in the GridCell.
		virtual inline float getTraversalCost( unsigned int cellIndex ) = 0;
		/// Returns the sum total of traversal costs of all objects referenced in the GridCell.
		virtual inline float getTraversalCost( unsigned int x, unsigned int z ) = 0;
		//@}

		/// @name Nearest neighbor queries
		//@{
		/// Returns an STL set of objects found in the specified spatial range.  Objects slightly outside the range may also be included.
		virtual void getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude) = 0;
		/// Returns an STL set of objects found in the specified range of GridCells.
		virtual void getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, unsigned int xMinIndex, unsigned int xMaxIndex, unsigned int zMinIndex, unsigned int zMaxIndex, SpatialDatabaseItemPtr exclude) = 0;
		/**
		 * \brief   Computes the agent neighbors of the specified agent.
		 * \param   agent    A pointer to the agent for which agent neighbors are to be computed.
		 * \param   rangeSq  The squared range around the agent.
		 */
		virtual void computeAgentNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const  = 0;

		/**
		 * \brief      Computes the obstacle neighbors of the specified agent.
		 * \param      agent           A pointer to the agent for which obstacle
		 *                             neighbors are to be computed.
		 * \param      rangeSq         The squared range around the agent.
		 */
		virtual void computeObstacleNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const  = 0;
		/// Returns an STL set of objects in the specified range, culling agent objects to a hemisphere centered around the facingDirection.
		virtual void getItemsInVisualField(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude, const Util::Point & position, const Util::Vector & facingDirection, float radiusSquared) = 0;
		//@}

		/// @name Ray tracing queries
		//@{
		/// Returns "true" if the ray found an intersection in-between r.mint and r.maxt
		virtual bool trace(const Util::Ray & r, float & t, SpatialDatabaseItemPtr &hitObject, SpatialDatabaseItemPtr exclude, bool excludeAgents) = 0;
		/// Returns "true" if no intersections were found with objects that might block line of sight. i.e., ignores objects that return blocksLineOfSight()==false.
		virtual bool hasLineOfSight(const Util::Ray & r, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2) = 0;
		/// Returns "true" if no intersections were found with objects that might block line of sight. i.e., ignores objects that return blocksLineOfSight()==false.
		virtual bool hasLineOfSight(const Util::Point & p1, const Util::Point & p2, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2) = 0;
		//@}


		/// @name Miscellaneous functions
		//@{
		/// Finds a random 2D point that has no other objects within the requested radius.
		virtual Util::Point randomPositionWithoutCollisions(float radius, bool excludeAgents) = 0;
		/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius.
		virtual Util::Point randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, float radius, bool excludeAgents) = 0;

		/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius, using an exising (already seeded) Mersenne Twister random number generator.
		virtual Util::Point randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, float radius, bool excludeAgents, MTRand & randomNumberGenerator) = 0;

		/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius, using an exising (already seeded) Mersenne Twister random number generator.
		virtual bool randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, SpatialDatabaseItemPtr item, bool excludeAgents, MTRand & randomNumberGenerator) = 0;

		/// Finds a random 2D point, within the specified region, using an exising (already seeded) Mersenne Twister random number generator.
		virtual Util::Point randomPositionInRegion(const Util::AxisAlignedBox & region, float radius,MTRand & randomNumberGenerator) = 0;

		/// Uses openGL and DrawLib to visualize the grid.
		virtual void draw() = 0;

		/// Gets the location of this agent, really used to get the y-location
		virtual Util::Point getLocation(SpatialDatabaseItemPtr exclude1) =0;
		/// Get Normal for item, This get the normal for an item, used to determine the orientation of the item
		virtual Util::Vector getUpVector(SpatialDatabaseItemPtr exclude1) =0;
		/// Used to refresh the state of the spatial database
		virtual void refreshDataBase() { }
		//@}

	};

}

#endif
