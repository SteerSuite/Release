//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * NavMeshDataBase.h
 *
 *  Created on: 2014-11-27
 *      Author: gberseth
 *
 * ./navmeshBuilder -ai ../modules/sfAI -testcase ../../testcases/4-way-oncomming.xml -module spatialDatabase,technique=composite01
 */

#ifndef KDTREEDATABASE_H_
#define KDTREEDATABASE_H_

#include "interfaces/SpatialDataBaseInterface.h"
#include "SimulationPlugin.h"
#include "KdTree.h"

namespace SteerLib
{

	class STEERLIB_API KdTreeDataBase : public SteerLib::SpatialDataBaseInterface
	{
	public:
		// KdTreeDataBase(EngineInterface * gEngin);
		KdTreeDataBase(EngineInterface * gEngine, float xmin, float xmax, float zmin, float zmax);

		// void init();
		// void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
		// void update(SteerLib::EngineInterface * engineInterface, float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber);
		void reset ()
		{
			_benchmarkScoreComputed = false;
			_numCollisionsOfAllAgents = 0;
			_numAgents = 0.0f;
			_totalTimeOfAllAgents = 0.0f;
			_totalEnergyOfAllAgents = 0.0f;
			_totalBenchmarkScore = 0.0f;

		}

		void buildObstacleTree();
		void buildAgentTree();

		/// Returns the x value of the "top-left" corner of the database.
		inline float getOriginX() { return _xOrigin; }
		/// Returns the z value of the "top-left" corner of the database.
		inline float getOriginZ() { return  _zOrigin; }
		/// Returns the total size of the database along the x direction.
		inline float getGridSizeX() { return  _xGridSize; }
		/// Returns the total size of the database along the z direction.
		inline float getGridSizeZ() { return  _zGridSize; }
		/// Returns the size of one grid cell along the x direction.
		inline float getCellSizeX() { return  0; }
		/// Returns the size of one grid cell along the z direction.
		inline float getCellSizeZ() { return  0; }
		/// Returns the number of grid cells along the x direction.
		inline unsigned int getNumCellsX() { return 0; }
		/// Returns the number of grid cells along the a direction.
		inline unsigned int getNumCellsZ() { return 0; }
		//@}

		/// @name Conversions between index, location, and grid coordinates
		//@{
		/// Returns an integer index of the GridCell where (x,z) is located.
		inline int getCellIndexFromLocation( float x, float z) {return 0;}
		/// Returns an integer index of the GridCell where Point v is located.
		inline int getCellIndexFromLocation( const Util::Point &v ) { return 0; }
		/// Returns the location of the center of the GridCell indexed by cellIndex; the "return value" is placed in the result arg.
		inline void getLocationFromIndex( unsigned int cellIndex, Util::Point & result ) {}
		/// Returns 2-D <b>integer</b> index coordinates of a GridCell indexed by cellIndex.
		inline void getGridCoordinatesFromIndex(unsigned int cellIndex, unsigned int &xIndex, unsigned int & zIndex) {}
		/// Returns the index of the GridCell that is indexed by 2-D integer coordinates (x,z).
		inline unsigned int getCellIndexFromGridCoords(unsigned int x, unsigned int z) { return 0; }
		//@}

		/// @name Database update functions
		//@{
		/// Adds an object to the database
		void addObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & newBounds );
		/// Removes an object from the database.  <b>It is the user's responsibility to make sure oldBounds is correct.</b>
		void removeObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox &oldBounds );
		/// Updates an existing object in the database.  <b>It is the user's responsibility to make sure oldBounds is correct.</b>
		void updateObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & oldBounds, const Util::AxisAlignedBox & newBounds ) {}
		///
		void clearDatabase();
		//@}

		/// @name Traversability queries
		//@{
		/// Returns true if there are any objects referenced in the GridCell.
		inline bool hasAnyItems( unsigned int cellIndex ) { return false; }
		/// Returns true if there are any objects referenced in the GridCell.
		inline bool hasAnyItems( unsigned int x, unsigned int z ) { return false; }
		/// Returns the sum total of traversal costs of all objects referenced in the GridCell.
		inline float getTraversalCost( unsigned int cellIndex ) { return 10.0; }
		/// Returns the sum total of traversal costs of all objects referenced in the GridCell.
		inline float getTraversalCost( unsigned int x, unsigned int z ) { return 10.0; }
		//@}

		/// @name Nearest neighbor queries
		//@{
		/// Returns an STL set of objects found in the specified spatial range.  Objects slightly outside the range may also be included.
		void getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude);
		/// Returns an STL set of objects found in the specified range of GridCells.
		void getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, unsigned int xMinIndex, unsigned int xMaxIndex, unsigned int zMinIndex, unsigned int zMaxIndex, SpatialDatabaseItemPtr exclude) {}
		/**
		 * \brief   Computes the agent neighbors of the specified agent.
		 * \param   agent    A pointer to the agent for which agent neighbors are to be computed.
		 * \param   rangeSq  The squared range around the agent.
		 */
		void computeAgentNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const;

		/**
		 * \brief      Computes the obstacle neighbors of the specified agent.
		 * \param      agent           A pointer to the agent for which obstacle
		 *                             neighbors are to be computed.
		 * \param      rangeSq         The squared range around the agent.
		 */
		void computeObstacleNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const;

		/// Returns an STL set of objects in the specified range, culling agent objects to a hemisphere centered around the facingDirection.
		void getItemsInVisualField(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude, const Util::Point & position, const Util::Vector & facingDirection, float radiusSquared) {}
		//@}

		/// @name Ray tracing queries
		//@{
		/// Returns "true" if the ray found an intersection in-between r.mint and r.maxt
		bool trace(const Util::Ray & r, float & t, SpatialDatabaseItemPtr &hitObject, SpatialDatabaseItemPtr exclude, bool excludeAgents);
		/// Returns "true" if no intersections were found with objects that might block line of sight. i.e., ignores objects that return blocksLineOfSight()==false.
		bool hasLineOfSight(const Util::Ray & r, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2) {return true;}
		/// Returns "true" if no intersections were found with objects that might block line of sight. i.e., ignores objects that return blocksLineOfSight()==false.
		bool hasLineOfSight(const Util::Point & p1, const Util::Point & p2, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2) {return true;}
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

		void draw();

		/// Gets the location of this agent, really used to get the y-location
		virtual Util::Point getLocation(SpatialDatabaseItemPtr exclude1) { return Util::Point(0.0,0.0,0.0); }
		/// Get Normal for item, This get the normal for an item, used to determine the orientation of the item
		virtual Util::Vector getUpVector(SpatialDatabaseItemPtr exclude1) { return Util::Vector(0.0, 1.0, 0.0); }

	protected:

		bool _benchmarkScoreComputed;
		float _alpha, _beta, _gamma;
		float _numCollisionsOfAllAgents, _totalTimeOfAllAgents, _totalEnergyOfAllAgents;
		float _numAgents;
		float _totalBenchmarkScore;

		SteerLib::EngineInterface * _engine;

		KdTree * _spatialDatabase;

		float _xOrigin;
		float _zOrigin;
		float _xGridSize;
		float _zGridSize;
		// friend KdTreeDataBaseModule;

	};
}


#endif /* KdTreeDATABASE_H_ */
