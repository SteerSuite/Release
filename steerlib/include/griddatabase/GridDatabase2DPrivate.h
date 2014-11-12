//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_GRID_DATABASE_PRIVATE_H__
#define __STEERLIB_GRID_DATABASE_PRIVATE_H__

/// @file GridDatabase2DPrivate.h
/// @brief Defines private functionality for the SteerLib::GridDatabase2D spatial database.

#include "Globals.h"
#include "util/Geometry.h"
#include "util/GenericException.h"
#include "util/Mutex.h"
#include "griddatabase/GridCell.h"


#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {

	// forward declarations
	class GridDatabasePlanningDomain;


	/** 
	 * @brief The protected data and member functions used by the GridDatabase2D class.
	 *
	 * This class should not be used directly.  Instead, use the GridDatabase2D public interface that
	 * inherits from this class.
	 */
	class STEERLIB_API GridDatabase2DPrivate {
	protected:
		/// Protected constructor enforces that users cannot publically instantiate this class.
		GridDatabase2DPrivate() { } 
		
		/// Helper function that allocates the database correctly during initialization
		void _allocateDatabase();

		/// Helper function that converts a spatial range to a 2-D integer index range.
		inline bool _clampSpatialBoundsToIndexRange(float xmin, float xmax, float zmin, float zmax, unsigned int & xMinIndex, unsigned int & xMaxIndex, unsigned int & zMinIndex, unsigned int & zMaxIndex);

		float _xOrigin; // location of the min x,y point of the grid.
		float _zOrigin;
		float _xGridSize; // size of the entire grid
		float _zGridSize;
		float _xInvGridSize; // (1/size), where size is over the entire grid.
		float _zInvGridSize;
		float _xCellSize; // size of each cell
		float _zCellSize;
		unsigned int _xNumCells;  // number of cells along the x or z axis
		unsigned int _zNumCells;

		unsigned int _maxItemsPerCell;

		bool _drawGrid; // should the grid be drawn?

		/// The internal pointer to all SpatialDatabaseItem pointers, used so that all database data remains contiguous for better data locality; this is the pointer to de-allocate instead of each grid cell's pointer separately.
		SpatialDatabaseItemPtr *  _basePtr;

		/// A 2-D array of grid cells, but organized in a 1-D array.
		GridCell* _cells;

		/// The state space interface used by the planner to plan paths through the database.
		GridDatabasePlanningDomain * _planningDomain;
	};



} // end namespace SteerLib;

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
