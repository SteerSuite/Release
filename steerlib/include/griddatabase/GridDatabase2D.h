//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_GRID_DATABASE_H__
#define __STEERLIB_GRID_DATABASE_H__

/// @file GridDatabase2D.h
/// @brief Defines the public interface for the SteerLib::GridDatabase2D spatial database.

#include <set>
#include <stack>
#include <vector>

#include "Globals.h"
#include "griddatabase/GridDatabase2DPrivate.h"
#include "interfaces/SpatialDatabaseItem.h"
#include "interfaces/SpatialDataBaseInterface.h"

// #define _DEBUG1

// forward declaration
class MTRand;

namespace SteerLib {


	/** 
	 * @brief A 2-D spatial database, that can contain any objects that inherit the SpatialDatabaseItem interface.
	 *
	 * This class is an efficient 2-D spatial database that organizes objects in your environment (i.e., agents or obstacles).
	 * In particular, this database organizes objects in a 2-D grid, where each cell in the grid contains a fixed
	 * number of references to objects.
	 *
	 * The database supports four main types of queries:
	 *  - <b>Updates and basic queries:</b> i.e. adding/removing objects from the database, and querying the basic properties of the database.
	 *  - <b>Traversability queries:</b> typically used for (but not limited to) A-star, to tell what the "cost" of traversing cells would be.
	 *  - <b>Nearest neighbor queries:</b> used to find closest objects or get a list of items in an agent's visual field.
	 *  - <b>Ray tracing queries:</b>, typically used to test line of sight or to determine exactly what objects are in front of you.
	 *
	 * <h3> How to use the database </h3>
	 *
	 * To use the spatial database with your own objects, simply inherit the SpatialDatabaseItem virtual interface.
	 * Note that several functions in Geometry.h make it easy to implement the SpatialDatabaseItem interface for boxes or circles.
	 * 
	 * Add, remove, and update objects in the database using the #addObject(), #removeObject(), 
	 * and #updateObject() functions.
	 *
	 * Perform queries on the database using the appropriate functionality described in the public interface.
	 *
	 * <h3> Notes </h3>
	 *  - The database implementation is not (yet) thread-safe.
	 *  - The grid is located on the x-z plane.
	 *  - During initialization you separately define (1) the spatial size of the grid, and (2) the 
	 *    number of cells to create along the x and z directions.
	 *  - You also define the max number of items to store in each cell.  Trying to storing more than 
	 *    this number of items in a single grid cell will cause a Util::GenericException to be thrown.
	 *
	 * <h3> Performance considerations </h3>
	 * Algorithmically, all types of queries are fairly efficient, by narrowing the computation cost down to 
	 * only the cells that overlap your query.  Nearest neighbor queries tend to be the most costly type of 
	 * query when the radius you are searching covers many grid cells.
	 *
	 * The performance of the grid database is sensitive to the value of numItemsPerCell (specified in the 
	 * constructors).  If it is too large, all queries will perform proportionally slower, and the storage
	 * cost of the database increases.  If it is too small, there is a risk of too many objects overlapping
	 * one grid cell, causing an exception.  A similar but less sensitive performance issue to consider is
	 * how many grid cells to use over the entire database.
	 *
	 * To balance these two points above, we suggest making the size of a grid cell approximately the same
	 * size as your smallest common objects; this way you can reduce the value of numItemsPerCell (e.g., perhaps around 7).
	 * Don't worry too much - these performance considerations can be addressed with a few simple trial and 
	 * error attempts when initializing the database.
	 *
	 * @see
	 *  - the SpatialDatabaseItem interface.
	 *
	 * @todo
	 *  - add a "leftover" grid cell that can become arbitrarily large.  The idea being that the database always
	 *    works with proper semantics, but may not perform well if too many things get leftover.
	 *
	 */
	class STEERLIB_API GridDatabase2D : public SpatialDataBaseInterface, public GridDatabase2DPrivate  {
	public:
		/// @name Constructors and destructors
		//@{
		GridDatabase2D(float xmin, float xmax, float zmin, float zmax, unsigned int numXCells, unsigned int numZCells, unsigned int numItemsPerCell, bool drawGrid);
		GridDatabase2D(const Util::Point & origin2D, float xExtent, float zExtent, unsigned int numXCells, unsigned int numZCells, unsigned int numItemsPerCell, bool drawGrid);
		~GridDatabase2D();
		//@}

		/// @name Accessor functions
		//@{
		/// Returns the x value of the "top-left" corner of the database.
		inline float getOriginX() { return _xOrigin; }
		/// Returns the z value of the "top-left" corner of the database.
		inline float getOriginZ() { return  _zOrigin; }
		/// Returns the total size of the database along the x direction.
		inline float getGridSizeX() { return  _xGridSize; }
		/// Returns the total size of the database along the z direction.
		inline float getGridSizeZ() { return  _zGridSize; }
		/// Returns the size of one grid cell along the x direction.
		inline float getCellSizeX() { return  _xCellSize; }
		/// Returns the size of one grid cell along the z direction.
		inline float getCellSizeZ() { return  _zCellSize; }
		/// Returns the number of grid cells along the x direction.
		inline unsigned int getNumCellsX() { return _xNumCells; }
		/// Returns the number of grid cells along the a direction.
		inline unsigned int getNumCellsZ() { return _zNumCells; }
		//@}

		/// @name Conversions between index, location, and grid coordinates
		//@{
		/// Returns an integer index of the GridCell where (x,z) is located.
		inline int getCellIndexFromLocation( float x, float z);
		/// Returns an integer index of the GridCell where Point v is located.
		inline int getCellIndexFromLocation( const Util::Point &v ) { return getCellIndexFromLocation(v.x, v.z); }
		/// Returns the location of the center of the GridCell indexed by cellIndex; the "return value" is placed in the result arg.
		inline void getLocationFromIndex( unsigned int cellIndex, Util::Point & result );
		/// Returns 2-D <b>integer</b> index coordinates of a GridCell indexed by cellIndex.
		inline void getGridCoordinatesFromIndex(unsigned int cellIndex, unsigned int &xIndex, unsigned int & zIndex);
		/// Returns the index of the GridCell that is indexed by 2-D integer coordinates (x,z).
		inline unsigned int getCellIndexFromGridCoords(unsigned int x, unsigned int z) { return (x * _zNumCells) + z; }
		//@}

		/// @name Database update functions
		//@{
		/// Adds an object to the database
		void addObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & newBounds );
		/// Removes an object from the database.  <b>It is the user's responsibility to make sure oldBounds is correct.</b>
		void removeObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox &oldBounds );
		/// Updates an existing object in the database.  <b>It is the user's responsibility to make sure oldBounds is correct.</b>
		void updateObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & oldBounds, const Util::AxisAlignedBox & newBounds );
		///
		virtual void clearDatabase();
		//@}

		/// @name Traversability queries
		//@{
		/// Returns true if there are any objects referenced in the GridCell.
		inline bool hasAnyItems( unsigned int cellIndex ) { return (_cells[cellIndex]._numItems != 0); }
		/// Returns true if there are any objects referenced in the GridCell.
		inline bool hasAnyItems( unsigned int x, unsigned int z ) { return (_cells[getCellIndexFromGridCoords(x,z)]._numItems != 0); }
		/// Returns the sum total of traversal costs of all objects referenced in the GridCell.
		inline float getTraversalCost( unsigned int cellIndex ) { return _cells[cellIndex]._traversalCost; }
		/// Returns the sum total of traversal costs of all objects referenced in the GridCell.
		inline float getTraversalCost( unsigned int x, unsigned int z ) { return _cells[getCellIndexFromGridCoords(x,z)]._traversalCost; }
		//@}

		/// @name Nearest neighbor queries
		//@{
		/// Returns an STL set of objects found in the specified spatial range.  Objects slightly outside the range may also be included.
		void getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude);
		/// Returns an STL set of objects found in the specified range of GridCells.
		void getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, unsigned int xMinIndex, unsigned int xMaxIndex, unsigned int zMinIndex, unsigned int zMaxIndex, SpatialDatabaseItemPtr exclude);
		void computeAgentNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const ;
		void computeObstacleNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const ;
		/// Returns an STL set of objects in the specified range, culling agent objects to a hemisphere centered around the facingDirection.
		void getItemsInVisualField(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude, const Util::Point & position, const Util::Vector & facingDirection, float radiusSquared);
		//@}

		/// @name Ray tracing queries
		//@{
		/// Returns "true" if the ray found an intersection in-between r.mint and r.maxt
		bool trace(const Util::Ray & r, float & t, SpatialDatabaseItemPtr &hitObject, SpatialDatabaseItemPtr exclude, bool excludeAgents);
		/// Returns "true" if no intersections were found with objects that might block line of sight. i.e., ignores objects that return blocksLineOfSight()==false.
		bool hasLineOfSight(const Util::Ray & r, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2);
		/// Returns "true" if no intersections were found with objects that might block line of sight. i.e., ignores objects that return blocksLineOfSight()==false.
		bool hasLineOfSight(const Util::Point & p1, const Util::Point & p2, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2);
		//@}

		/// @name Path planning queries
		//@{
		/// Returns "true" if a path was found from startLocation to goalLocation, or "false" if no complete path was found; in either case, the path (complete if returning true, or partial path if returning false) is stored in outputPlan as a sequence of grid cell indices.
		bool planPath(unsigned int startLocation, unsigned int goalLocation, std::stack<unsigned int> & outputPlan);

		bool planPath(unsigned int startLocation, unsigned int goalLocation, std::stack<unsigned int> & outputPlan, unsigned int maxNodes);

		bool findPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
				unsigned int _maxNodesToExpandForSearch);

		bool findSmoothPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
				unsigned int _maxNodesToExpandForSearch);
		//@}

		/// @name Miscellaneous functions
		//@{
		/// Finds a random 2D point that has no other objects within the requested radius.
		Util::Point randomPositionWithoutCollisions(float radius, bool excludeAgents);
		/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius.
		Util::Point randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, float radius, bool excludeAgents);

		/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius, using an exising (already seeded) Mersenne Twister random number generator.
		Util::Point randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, float radius, bool excludeAgents, MTRand & randomNumberGenerator);

		/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius, using an exising (already seeded) Mersenne Twister random number generator.
		virtual bool randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, SpatialDatabaseItemPtr item, bool excludeAgents, MTRand & randomNumberGenerator);

		/// Finds a random 2D point, within the specified region, using an exising (already seeded) Mersenne Twister random number generator.
		Util::Point randomPositionInRegion(const Util::AxisAlignedBox & region, float radius,MTRand & randomNumberGenerator);

		/// Uses openGL and DrawLib to visualize the grid.
		void draw();

		/// Gets the location of this agent, really used to get the y-location
		virtual Util::Point getLocation(SpatialDatabaseItemPtr exclude1) { return Util::Point(0.0,0.0,0.0); }
		/// Get Normal for item, This get the normal for an item, used to determine the orientation of the item
		virtual Util::Vector getUpVector(SpatialDatabaseItemPtr exclude1) { return Util::Vector(0.0, 1.0, 0.0); }
		//@}

	}; // end class GridDatabase2D



	inline int GridDatabase2D::getCellIndexFromLocation( float x, float z)
	{
		int ix, iz;
#ifdef _DEBUG_1
	std::cout << "_xOrigin = " << _xOrigin << ", _zOrigin = " << _zOrigin << "\n";
	std::cout << "_xGridsize = " << _xGridSize << ", _zGridsize = " << _zGridSize << "\n";
#endif
		if (x < _xOrigin) return -1; // _xOrigin is not initialized properly somewhere.
		if (z < _zOrigin) return -1;
		if (x >= _xOrigin + _xGridSize) return -1;
		if (z >= _zOrigin + _zGridSize) return -1;
		ix = (int) (((x - _xOrigin) * _xInvGridSize) * _xNumCells);
		iz = (int) (((z - _zOrigin) * _zInvGridSize) * _zNumCells);
		return getCellIndexFromGridCoords(ix, iz);
	}

	inline void GridDatabase2D::getLocationFromIndex( unsigned int cellIndex, Util::Point & result ) {
		unsigned int x,z;
		getGridCoordinatesFromIndex(cellIndex, x, z);
		// the location along one axis is equal to # cells times the size of each grid cell, plus half of one more cell to locate in the center of the cell.
		result.x = (((float)x) + 0.5f)*_xCellSize + _xOrigin,
		result.y = 0.0f;
		result.z = (((float)z) + 0.5f)*_zCellSize + _zOrigin;
	}

	inline void GridDatabase2D::getGridCoordinatesFromIndex(unsigned int cellIndex, unsigned int &xIndex, unsigned int & zIndex) {
		xIndex = cellIndex / _zNumCells; // integer division so that remainders also get truncated
		zIndex = cellIndex - (xIndex * _zNumCells);
	}


} // end namespace SteerLib;


#endif

