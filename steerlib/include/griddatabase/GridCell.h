//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_GRID_CELL_H__
#define __STEERLIB_GRID_CELL_H__

/// @file GridCell.h
/// @brief Defines the GridCell data structure used by the SteerLib::GridDatabase2D spatial database.

#include "Globals.h"
// #include "SteerLib.h"
#include "util/Geometry.h"
#include "util/GenericException.h"
#include "util/Mutex.h"
// #include "interfaces/AgentInterface.h"
#include <sstream>

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
	class STEERLIB_API SpatialDatabaseItem;
	typedef SpatialDatabaseItem* SpatialDatabaseItemPtr;

	/**
	 * @brief A single grid cell of the GridDatabase2D spatial database.
	 *
	 * This class represents one grid cell of the GridDatabase2D spatial database.
	 * The cell contains a list of pointers of SpatialDatabaseItem objects that 
	 * overlap the cell.
	 *
	 * Most users should not need to use this class at all, the GridDatabase2D is the main 
	 * public interface for using the spatial database functionality.
	 *
	 */
	class STEERLIB_API GridCell {

	public:
		void init( unsigned int maxNumItems, SpatialDatabaseItemPtr * localBasePtr, float initialTraversalCost) {
			_items = localBasePtr;
			for (unsigned int j=0; j < maxNumItems; j++) {
				_items[j] = NULL;
			}
			_numItems = 0; // initialize with no items in the grid cell
			_traversalCost = initialTraversalCost;
		}

		/// Adds an object reference to this cell.
		inline void add(SpatialDatabaseItemPtr entry, unsigned int maxItems, float traversalCostToAdd) {

			_gridCellMutex.lock();

			/** 
			 * If an exception is thrown in this function, it means, there are too many 
			 * agents in one grid cell.  If you are certain its not a bug, you can increase 
			 * the value of numItemsPerCell in the GridDatabase2D constructor, or increase 
			 * the resolution of the grid. (Note that such changes may affect performance, too.)
			 */

			if (_numItems >= maxItems)
			{
				_gridCellMutex.unlock();
				std::cout << "The number of items in this cell is:" << _numItems << " The max number of items can be: " << maxItems << std::endl;
				throw Util::GenericException("There are too many items in a single cell of the grid database.\nIn the next version, this will be handled robstly and not be an error.\nFor now, use a higher number for maxItemsPerGridCell (in the config file), or\nincrease the resolution of the grid (both of which may decrease performance).");
			}
			_numItems++;
			unsigned int i=0;
			while (_items[i] != NULL) {
				i++;
			}
			_items[i] = entry;

			_traversalCost += traversalCostToAdd;

			_gridCellMutex.unlock();
		}

		/// Removes an object reference from this cell.
		inline void remove(SpatialDatabaseItemPtr entry, unsigned int maxItems, float traversalCostToSubtract) {

			_gridCellMutex.lock();

			if (_numItems <= 0) {
				_gridCellMutex.unlock();
				// throw Util::GenericException("Tried to remove an object from a grid cell, but the grid cell was empty." );
				std::stringstream errormsg;
				// I was trying to create a more informative error message
				// SteerLib::AgentInterface * agent = dynamic_cast<SteerLib::AgentInterface*>(*entry);
				// if ( agent != NULL)
				{
					// errormsg << "Tried to remove " << agent->position() << " from a grid cell, but the grid cell was empty.";
				}
				// else
				{
					errormsg << "Tried to remove " << entry << " from a grid cell, but the grid cell was empty.";
				}
				throw Util::GenericException(errormsg.str());
			}
			_numItems--;
			unsigned int i=0;
			while ((_items[i] != entry) && (i<maxItems)) i++;
			if (i >= maxItems) {
				_gridCellMutex.unlock();
				throw Util::GenericException("Tried to remove an object from a grid cell, but it did not exist there in the first place.");
			}
			_items[i] = NULL;

			_traversalCost -= traversalCostToSubtract;

			_gridCellMutex.unlock();

		}

	private:

		// The grid database is allowed to access the grid cell's private data directly.
		friend class GridDatabase2D;
		friend class GridDatabase2DPrivate;

		/// The number of items currently referenced in this cell.
		unsigned int _numItems;

		/// An array of pointers of fixed length; the length is determined during GridDatabase initialization.
		SpatialDatabaseItemPtr * _items;

		/// Cost of traversing this grid cell
		float _traversalCost;

		/// This lock is used by the grid database directly; by itself the grid cell is not necessarily thread safe.
		Util::Mutex _gridCellMutex;
	};



} // end namespace SteerLib;

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
