//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file GridDatabase2D.cpp
/// @brief Implements the SteerLib::GridDatabase2D spatial database.

#include <set>
#include <iostream>
#include <algorithm>

#include "util/GenericException.h"
#include "util/Geometry.h"
#include "util/DrawLib.h"
#include "util/Color.h"
#include "util/Misc.h"
#include "mersenne/MersenneTwister.h"

#include "interfaces/AgentInterface.h"
#include "griddatabase/GridDatabase2D.h"
#include "griddatabase/GridDatabasePlanningDomain.h"

using namespace std;
using namespace SteerLib;
using namespace Util;


//
// constructor for grid database - takes the bounds and desired number of cells
//
GridDatabase2D::GridDatabase2D(float xmin, float xmax, float zmin, float zmax, unsigned int numXCells, unsigned int numZCells, unsigned int maxItemsPerCell, bool drawGrid)
{
	if (xmin > xmax) swap(xmin, xmax);
	if (zmin > zmax) swap(xmin, xmax);

	_xOrigin = xmin;
	_zOrigin = zmin;
	_xGridSize = xmax - xmin;
	_zGridSize = zmax - zmin;
	_xInvGridSize = 1.0f / _xGridSize;
	_zInvGridSize = 1.0f / _zGridSize;
	_xNumCells = numXCells;
	_zNumCells = numZCells;
	_xCellSize = _xGridSize / ((float)numXCells);
	_zCellSize = _zGridSize / ((float)numZCells);
	_maxItemsPerCell = maxItemsPerCell;
	_drawGrid = drawGrid;

	_allocateDatabase();
	_planningDomain = new GridDatabasePlanningDomain(this);
}


//
// alternate constructor for grid database - specify the (minx,minz) corner, and the desired grid size and number of cells
//
GridDatabase2D::GridDatabase2D(const Point & origin2D, float xExtent, float zExtent, unsigned int numXCells, unsigned int numZCells, unsigned int maxItemsPerCell, bool drawGrid)
{
	_xOrigin = origin2D.x;
	_zOrigin = origin2D.z;
	_xGridSize = xExtent;
	_zGridSize = zExtent;
	_xInvGridSize = 1.0f / _xGridSize;
	_zInvGridSize = 1.0f / _zGridSize;
	_xNumCells = numXCells;
	_zNumCells = numZCells;
	_xCellSize = _xGridSize / ((float)numXCells);
	_zCellSize = _zGridSize / ((float)numZCells);
	_maxItemsPerCell = maxItemsPerCell;
	_drawGrid = drawGrid;

	_allocateDatabase();
	_planningDomain = new GridDatabasePlanningDomain(this);
}


//
// destructor
//
GridDatabase2D::~GridDatabase2D()
{
	delete [] _basePtr;
	delete [] _cells;
	delete _planningDomain;
}


//
// _allocateDatabase() - protected helper function called by constructors.
//
void GridDatabase2DPrivate::_allocateDatabase()
{
	if (_maxItemsPerCell > 15) {
		cerr << "WARNING: GridDatabase2D is linearly dependent on _maxItemsPerCell." << endl;
		cerr << "         right now _maxItemsPerCell is " << _maxItemsPerCell << "," << endl;
		cerr << "         performance may degrade." << endl;
	}

	unsigned int numTotalCells = _xNumCells*_zNumCells;
	unsigned int numTotalItems = numTotalCells * _maxItemsPerCell;

	_basePtr = new SpatialDatabaseItemPtr[numTotalItems];
	_cells = new GridCell[numTotalCells];

	for (unsigned int i=0; i < numTotalCells; i++) {
		// TODO: is it OK to make the traversal cost 0.0f ?? it would be more general.  need to double-check assumptions 
		// of astar lib...  is traversal cost a fixed cost to add, or is it a multiplicative factor?
		_cells[i].init( _maxItemsPerCell, _basePtr + (i*_maxItemsPerCell), 1.0f );
	}
}

// Rounds the given float to the nearest integer if it is in the specified error range.
float _roundClose(float f)
{
	const float epsilon = 0.00005f;

	unsigned int i = (unsigned int)f;
	float diff = f - i;

	if ((1-diff < epsilon && 1-diff > -epsilon) || (diff < epsilon && diff > -epsilon))
		return (float)(unsigned int)(f+0.5);
	else
		return f;
}

//
// _clampSpatialBoundsToIndexRange - converts a bounding box to a valid range of indices in the database.
//                                  if the bounding box is entirely outside of the space covered by the
//                                  database, returns false.
//
// because it is inline, if this function ever needs to become public, place it in the .h file instead.
//
inline bool GridDatabase2DPrivate::_clampSpatialBoundsToIndexRange(float xmin, float xmax, float zmin, float zmax, unsigned int & xMinIndex, unsigned int & xMaxIndex, unsigned int & zMinIndex, unsigned int & zMaxIndex)
{
	// clamp and convert xmin to xMinIndex
	if (xmin < _xOrigin)
		xMinIndex = 0;
	else if (xmin > _xOrigin + _xGridSize)
		return false;
	else
		xMinIndex = (unsigned int)floor(_roundClose(((xmin - _xOrigin) * _xInvGridSize) * _xNumCells));

	// clamp and convert zmin to zMinIndex
	if (zmin < _zOrigin)
		zMinIndex = 0;
	else if (zmin > _zOrigin + _zGridSize)
		return false;
	else
		zMinIndex = (unsigned int)floor(_roundClose(((zmin - _zOrigin) * _zInvGridSize) * _zNumCells));

	// clamp and convert xmax to xMaxIndex
	if (xmax >= _xOrigin + _xGridSize)
		xMaxIndex = _xNumCells-1; // subtract one because xMaxIndex is included in the spatial bounds.
	else if (xmax < _xOrigin)
		return false;
	else
		xMaxIndex = (unsigned int)ceil(_roundClose(((xmax - _xOrigin) * _xInvGridSize) * _xNumCells))-1;

	// clamp and convert zmax to zMaxIndex
	if (zmax >= _zOrigin + _zGridSize)
		zMaxIndex = _zNumCells-1;  // subtract one because zMaxIndex is included in the spatial bounds.
	else if (zmax < _zOrigin)
		return false;
	else
		zMaxIndex = (unsigned int)ceil(_roundClose(((zmax  - _zOrigin) * _zInvGridSize) * _zNumCells))-1;

	return true;
}


//
// addObject() - adds the given item to the database.  Each grid cell that overlaps
//               "newBounds" will then contain a reference to the item.
//
void GridDatabase2D::addObject( SpatialDatabaseItemPtr item, const AxisAlignedBox & newBounds )
{
	// convert the spatial bounds of the object into index bounds
	unsigned int xMinIndex, xMaxIndex, zMinIndex, zMaxIndex;
	if (_clampSpatialBoundsToIndexRange(newBounds.xmin, newBounds.xmax, newBounds.zmin, newBounds.zmax, xMinIndex, xMaxIndex, zMinIndex, zMaxIndex) == false) {
		// if we get false here, the object's bounds are completely outside the database anyway.
		return;
	}

	unsigned int cellIndex;

	// iterate over all cells that overlap the bounding box of the object
	// Note that we use a shortcut to avoid wasting computation;  instead of using getCellIndexFromGridCoords(i,j) in the inner-loop,
	// we take advantage of the fact that the grid cells are contiguous in memory, and increment cellIndex directly,
	// recomputing it only when i changes.
	// std::cout << "Adding object to cell again, maxItems is: " << _maxItemsPerCell << std::endl;
	for (unsigned int i=xMinIndex; i<=xMaxIndex; i++) {
		cellIndex = getCellIndexFromGridCoords(i,zMinIndex);
		for (unsigned int j=zMinIndex; j<=zMaxIndex; j++) {
			_cells[cellIndex].add(item,_maxItemsPerCell, item->getTraversalCost());
			// std::cout << "CellIndex is: " << cellIndex << std::endl;
			cellIndex++;
		}
	}
}


//
// removeObject() - removes an item from the grid cells that overlap with "oldBounds"
//
void GridDatabase2D::removeObject( SpatialDatabaseItemPtr item, const AxisAlignedBox &oldBounds )
{
	// convert the spatial bounds of the object into index bounds
	unsigned int xMinIndex, xMaxIndex, zMinIndex, zMaxIndex;
	if (_clampSpatialBoundsToIndexRange(oldBounds.xmin, oldBounds.xmax, oldBounds.zmin, oldBounds.zmax, xMinIndex, xMaxIndex, zMinIndex, zMaxIndex) == false) {
		// if we get false here, the object's bounds are completely outside the database anyway.
		return;
	}

	unsigned int cellIndex;

	// iterate over all cells that overlap the bounding box of the object
	// Note that we use a shortcut to avoid wasting computation;  instead of using getCellIndexFromGridCoords(i,j) in the inner-loop,
	// we take advantage of the fact that the grid cells are contiguous in memory, and increment cellIndex directly,
	// recomputing it only when i changes.
#ifdef _DEBUG
	// std::cout << "about to remove(item, _maxItemsPerCell, item->getTraversalCost());\n";
#endif
	for (unsigned int i=xMinIndex; i<=xMaxIndex; i++) {
		cellIndex = getCellIndexFromGridCoords(i,zMinIndex);
		for (unsigned int j=zMinIndex; j<=zMaxIndex; j++) {
			_cells[cellIndex].remove(item, _maxItemsPerCell, item->getTraversalCost());
			cellIndex++;
		}
	}
}


//
// updateObject() - updates the grid cells that have a reference to the item.
//
// for now this function calls removeObject() and addObject(). but later it may be
// appropriate to optimize this even further by updating only the grid cells that
// will be changed by the update.
//
//
void GridDatabase2D::updateObject( SpatialDatabaseItemPtr item, const AxisAlignedBox & oldBounds, const AxisAlignedBox & newBounds )
{
	// TODO: make an efficient "diff" between the two bounding boxes, and only iterate over the disjoint parts.
#ifdef _DEBUG_!
	std::cout << "about to updateObject()\n";
#endif
	removeObject(item, oldBounds);
	// assert(item != NULL);
	addObject(item, newBounds);
}


//
// getItemsInRange() - the protected version uses the integer index ranges.
//
void GridDatabase2D::getItemsInRange(set<SpatialDatabaseItemPtr> & neighborList, unsigned int xMinIndex, unsigned int xMaxIndex, unsigned int zMinIndex, unsigned int zMaxIndex, SpatialDatabaseItemPtr exclude)
{
	int cellIndex;

	// iterate over all grid cells in the range,
	for (unsigned int i=xMinIndex; i<=xMaxIndex; i++) {
		cellIndex = (i * _zNumCells) + zMinIndex;
		for (unsigned int j=zMinIndex; j<=zMaxIndex; j++) {
			for (unsigned int k=0; k < _maxItemsPerCell; k++) {
				if ((_cells[cellIndex]._items[k]!=NULL) && (_cells[cellIndex]._items[k]!=exclude)) {
					neighborList.insert(_cells[cellIndex]._items[k]);
				}
			}
			cellIndex++;
		}
	}
}


//
// getItemsInRange() - simply converts the spatial bounds into index range, and then calls the private getItemsInRange().
//
void GridDatabase2D::getItemsInRange(set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude)
{
	unsigned int xMinIndex=0, xMaxIndex=0, zMinIndex=0, zMaxIndex=0;
	_clampSpatialBoundsToIndexRange(xmin, xmax, zmin, zmax, xMinIndex, xMaxIndex, zMinIndex, zMaxIndex);
	getItemsInRange(neighborList,xMinIndex,xMaxIndex,zMinIndex,zMaxIndex,exclude);
}

//
// getItemsInVisualField()
//
void GridDatabase2D::getItemsInVisualField(set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude, const Point & position, const Vector & facingDirection, float radiusSquared)
{
	unsigned int xMinIndex=0, xMaxIndex=0, zMinIndex=0, zMaxIndex=0;
	_clampSpatialBoundsToIndexRange(xmin, xmax, zmin, zmax, xMinIndex, xMaxIndex, zMinIndex, zMaxIndex);

	int cellIndex;
	// iterate over all grid cells in the range,
	for (unsigned int i=xMinIndex; i<=xMaxIndex; i++) {
		cellIndex = getCellIndexFromGridCoords(i,zMinIndex);
		for (unsigned int j=zMinIndex; j<=zMaxIndex; j++) {
			for (unsigned int k=0; k < _maxItemsPerCell; k++) {
				SpatialDatabaseItemPtr possiblyVisibleObject = _cells[cellIndex]._items[k];


				// ignore this object if it is actually not an object (NULL), or if we are supposed to exclude it
				if ((possiblyVisibleObject==NULL) || (possiblyVisibleObject==exclude))
					continue;

				if (possiblyVisibleObject->isAgent()) {
					// three more conditions...

					// do a search to make sure it doesnt exist in the set already, if it does exist 
					// then we don't need to consider this object any further.
					if (neighborList.find(possiblyVisibleObject) != neighborList.end()) continue;

					// (1) if the agent is outside of the radius of the visual field, then forget it
					Point hisPosition = (dynamic_cast<AgentInterface*>(possiblyVisibleObject))->position();
					Vector directionToOtherAgent = hisPosition - position;
					float distSquared = directionToOtherAgent.lengthSquared();
					if (distSquared > radiusSquared) 
						continue;

					// (2) check whether the object is actually in the cone based on our facing direction
					// TODO: we shouldn't need to normalize here, because we just want to check sign, right?
					//float cosTheta = dot(directionToOtherAgent/sqrtf(distSquared),normalize(facingDirection));
					float cosTheta = dot(directionToOtherAgent/sqrtf(distSquared),normalize(facingDirection));
					if (cosTheta < 0.0f) 
						continue;

					// (3) finally, we can do the most expensive final check - checking line-of-sight.
					// previous database did not do this here, because ray tracing routines were not possible to call in the grid DB.
					// now with the virtualized interface of database items, we can.
					if (!hasLineOfSight(position, hisPosition, possiblyVisibleObject, exclude))
						continue;
					
					// if we really got this far, that means this object really is visible, so add it to the neighborList.
					neighborList.insert(possiblyVisibleObject);
				}
				else {
					// for non-Agent items, i.e. "objects" --> obstacles and such: we assume the 
					// agent will know where such items are, even if its not directly 
					// in its visual field.  so, always add it without needing to check
					// line-of-sight or distance.
					neighborList.insert(possiblyVisibleObject);
				}

			}
			cellIndex++;
		}

	}
}

void GridDatabase2D::draw()
{
#ifdef ENABLE_GUI
	if (_drawGrid == false)
		return;

	Point beginning, end; // two points of a line segment to be drawn
	Point center;
	Vector xOffset(0.0f, 0.0f, 0.0f), zOffset(0.0f, 0.0f, 0.0f);
	xOffset.x = 0.5f * _xCellSize;
	zOffset.z = 0.5f * _zCellSize;

	// draw the grid itself
	DrawLib::glColor(gGray90);
	beginning.x = _xOrigin;   beginning.y = -0.01f;   beginning.z = _zOrigin;
	end.x = _xOrigin;   end.y = -0.01f;   end.z = _zOrigin + _zGridSize;
	DrawLib::drawLine(beginning, end);
	for (unsigned int i=1; i <= _xNumCells; i++) {
		float temp = _xOrigin + (i * _xCellSize);
		beginning.x = temp;
		end.x = temp;
		DrawLib::drawLine(beginning, end);
	}
	beginning.x = _xOrigin;   beginning.z = _zOrigin;
	end.x = _xOrigin + _xGridSize;   end.z = _zOrigin;
	DrawLib::drawLine(beginning, end);
	for (unsigned int j=1; j <= _zNumCells; j++) {
		float temp = _zOrigin + (j * _zCellSize);
		beginning.z = temp;
		end.z = temp;
		DrawLib::drawLine(beginning, end);
	}

	float gridQuadHeight = -0.1f;
	// draw representations for contents of each grid cell
	for (unsigned int i=0; i < _xNumCells; i++)
	{
		for (unsigned int j=0; j < _zNumCells; j++)
		{
			int cellIndex = getCellIndexFromGridCoords(i, j);

			Point p = Point(_xOrigin + i * _xCellSize, gridQuadHeight, _zOrigin + j * _zCellSize);
			Point a = p + Point(0, 0, 0);
			Point b = p + Point(0, 0, _zCellSize);
			Point c = p + Point(_xCellSize, 0, _zCellSize);
			Point d = p + Point(_xCellSize, 0, 0);

			// This is new and not documented yet.

			Color color(0.4,0.4,0.4);


			for (unsigned int item=0; item < _cells[cellIndex]._numItems; item++)
			{
				if ((_cells[cellIndex]._items[item] != NULL))
				{
					if (_cells[cellIndex]._items[item]->isAgent())
						color = color + Color(0,0,0.9f / _maxItemsPerCell);
					else
						color = color + Color(0.8f / _maxItemsPerCell,0,0);
				}
			}
			DrawLib::glColor(color);
			DrawLib::drawQuad(a, b, c, d);
		}
	}

#else
	throw GenericException("GridDatabase2D::draw() cannot be called, this version of SteerLib compiled without GUI functionality.");
#endif // ifdef ENABLE_GUI
}

bool GridDatabase2D::trace(const Ray & r, float & t, SpatialDatabaseItemPtr &hitObject, SpatialDatabaseItemPtr exclude, bool excludeAgents)
{
	// 1. march through grid cells
	// 2. for each grid cell:
	//   - test all items in each grid cell
	// 3. if the intersection found lies within the current grid cell, then return true with that info
	// 4. else, go on to the next grid cell.

	// just declare all vars in the first place
	unsigned int x,z; // (x,z) are 2-d coordinates and currentBin is the 1-d index to refer to the bin being traversed
	int currentBin;
	Point center;
	float xlow, xhi, zlow, zhi, xOffset, zOffset;  // spatial info for the grid cell being traversed
	float txnear, txfar, tznear, tzfar, invRayDirx, invRayDirz;
	float maxt, mint;
	bool validIntersectionFound;

	// one-time initialization stuff here
	currentBin = getCellIndexFromLocation(r.pos.x, r.pos.z);
#ifdef _DEBUG_!
	std::cout << "r.pos.x = " << r.pos.x << "\n";
	std::cout << "r.pos.z = " << r.pos.z << "\n";

#endif
	if (currentBin == -1) return false;
	getGridCoordinatesFromIndex(currentBin,x,z);
	invRayDirx = 1.0f / r.dir.x;
	invRayDirz = 1.0f / r.dir.z;
	xOffset = 0.5f * _xGridSize / ((float)_xNumCells);
	zOffset = 0.5f * _zGridSize / ((float)_zNumCells);

	// clamp maxt to be within the grid
	xlow = _xOrigin;
	xhi  = _xOrigin + _xGridSize;
	zlow = _xOrigin;
	zhi  = _xOrigin + _zGridSize;
	txnear = (xlow-r.pos.x) * invRayDirx;
	txfar = (xhi -r.pos.x) * invRayDirx;
	if (txnear>txfar) swap(txnear,txfar);
	tznear = (zlow-r.pos.z) * invRayDirz;
	tzfar = (zhi -r.pos.z) * invRayDirz;
	if (tznear>tzfar) swap(tznear,tzfar);
	maxt = min(r.maxt, min(txfar, tzfar));  // txnear and tznear will be "behind" the ray, since this bounding box is the entire grid and we only reach this code if the position is inside of the grid.

	// ****** URGENT TODO: ***** really should clamp mint, too...  if you are crashing when the agent is outside the grid, check this error.
	mint = r.mint;

	// set up info for the first grid cell
	getLocationFromIndex(currentBin, center);
	xlow = center.x - xOffset;
	xhi  = center.x + xOffset;
	zlow = center.z - zOffset;
	zhi  = center.z + zOffset;
	txnear = (xlow-r.pos.x) * invRayDirx;
	txfar = (xhi -r.pos.x) * invRayDirx;
	if (txnear>txfar) swap(txnear,txfar);
	tznear = (zlow-r.pos.z) * invRayDirz;
	tzfar = (zhi -r.pos.z) * invRayDirz;
	if (tznear>tzfar) swap(tznear,tzfar);

	// march through all appropriate grid cells, in order, terminating early when an *appropriate* intersection is found
	// "appropriate" means that the t value is INSIDE of the grid cell.  otherwise there is a risk of that t value not being
	// the closest one (if another object in the next grid cell is actually closer but we dont realize it yet)
	do {
		validIntersectionFound = false;
		hitObject = NULL;
		float mostRecent_maxt = min(maxt,min(txfar,tzfar)); // this way no intersection will be valid unless it was within this grid cell

		for (unsigned int i=0; i<_maxItemsPerCell; i++)
		{

			if ((_cells[currentBin]._items[i] != NULL) && (_cells[currentBin]._items[i] != exclude)) // && (_cells[currentBin]._numItems > i))
			{ // Getting trace errors for outside of mapped region.
				// Access not within mapped region at address 0xFFFFFFE00991E050

				if ((excludeAgents) && _cells[currentBin]._items[i]->isAgent())
					continue;

				float temp_t;
				bool intersected;
				intersected = false;
				Ray tempRay;
				tempRay.initWithUnitInterval(r.pos, r.dir);
				tempRay.maxt = mostRecent_maxt;
				tempRay.mint = mint;
				intersected = _cells[currentBin]._items[i]->intersects(tempRay,temp_t);
				if ((intersected) && (temp_t < mostRecent_maxt)) {
					// found a valid intersection, set all the values appropriately
					validIntersectionFound = true;
					mostRecent_maxt = temp_t;
					t = temp_t;
					hitObject = _cells[currentBin]._items[i];
				}
			}
		}

		// if a valid intersection was found in this bin, then just return
		if (validIntersectionFound) { return true; }

		// otherwise, decide which way to move to the next grid cell
		if (txfar < tzfar) {
			// then move along the x grid cells
			if (r.dir.x < 0.0f)
				x--;
			else
				x++;
			if ((x < 0) || (x > _xNumCells)) {
				return false; // no intersection found yet, and now we're out of bounds.
			}
		}
		else {
			// then move along the z grid cells
			if (r.dir.z < 0.0f)
				z--;
			else
				z++;
			if ((z < 0) || (z > _zNumCells)) {
				return false; // no intersection found yet, and now we're out of bounds.
			}
		}

		// set up info for the next grid cell
		currentBin = getCellIndexFromGridCoords(x,z);
		getLocationFromIndex(currentBin, center);
		xlow = center.x - xOffset;
		xhi  = center.x + xOffset;
		zlow = center.z - zOffset;
		zhi  = center.z + zOffset;
		txnear = (xlow-r.pos.x) * invRayDirx;
		txfar = (xhi -r.pos.x) * invRayDirx;
		if (txnear>txfar) swap(txnear,txfar);
		tznear = (zlow-r.pos.z) * invRayDirz;
		tzfar = (zhi -r.pos.z) * invRayDirz;
		if (tznear>tzfar) swap(tznear,tzfar);

	} while ( maxt > max(txnear,tznear));  // keep doing this loop until the ray's endpoint is in the cell you just traversed

	return false;

}

bool GridDatabase2D::hasLineOfSight(const Ray & r, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2)
{
	// 1. march through grid cells
	// 2. for each grid cell:
	//   - test all items in each grid cell
	// 3. if th intersection found lies within the current grid cell, then return false
	// 4. else, go on to the next grid cell.

	// just declare all vars in the first place
	unsigned int x,z; // (x,z) are 2-d coordinates and currentBin is the 1-d index to refer to the bin being traversed
	int currentBin;
	Point center;
	float xlow, xhi, zlow, zhi, xOffset, zOffset;  // spatial info for the grid cell being traversed
	float txnear, txfar, tznear, tzfar, invRayDirx, invRayDirz;
	float maxt, mint;
	bool validIntersectionFound;

	// one-time initialization stuff here
	currentBin = getCellIndexFromLocation(r.pos.x, r.pos.z);
	if (currentBin == -1) return true;
	getGridCoordinatesFromIndex(currentBin,x,z);
	invRayDirx = 1.0f / r.dir.x;
	invRayDirz = 1.0f / r.dir.z;
	xOffset = 0.5f * _xGridSize / ((float)_xNumCells);
	zOffset = 0.5f * _zGridSize / ((float)_zNumCells);

	// clamp maxt to be within the grid
	xlow = _xOrigin;
	xhi  = _xOrigin + _xGridSize;
	zlow = _xOrigin;
	zhi  = _xOrigin + _zGridSize;
	txnear = (xlow-r.pos.x) * invRayDirx;
	txfar = (xhi -r.pos.x) * invRayDirx;
	if (txnear>txfar) swap(txnear,txfar);
	tznear = (zlow-r.pos.z) * invRayDirz;
	tzfar = (zhi -r.pos.z) * invRayDirz;
	if (tznear>tzfar) swap(tznear,tzfar);
	maxt = min(r.maxt, min(txfar, tzfar));  // txnear and tznear will be "behind" the ray, since this bounding box is the entire grid and we only reach this code if the position is inside of the grid.

	// ****** URGENT TODO: ***** really should clamp mint, too...  if you are crashing when the agent is outside the grid, check this error.
	mint = r.mint;

	// set up info for the first grid cell
	getLocationFromIndex(currentBin, center);
	xlow = center.x - xOffset;
	xhi  = center.x + xOffset;
	zlow = center.z - zOffset;
	zhi  = center.z + zOffset;
	txnear = (xlow-r.pos.x) * invRayDirx;
	txfar = (xhi -r.pos.x) * invRayDirx;
	if (txnear>txfar) swap(txnear,txfar);
	tznear = (zlow-r.pos.z) * invRayDirz;
	tzfar = (zhi -r.pos.z) * invRayDirz;
	if (tznear>tzfar) swap(tznear,tzfar);

	// march through all appropriate grid cells, in order, terminating early when an *appropriate* intersection is found
	// "appropriate" means that the t value is INSIDE of the grid cell.  otherwise there is a risk of that t value not being
	// the closest one (if another object in the next grid cell is actually closer but we dont realize it yet)
	do {
		validIntersectionFound = false;
		float mostRecent_maxt = min(maxt,min(txfar,tzfar)); // this way no intersection will be valid unless it was within this grid cell

		for (unsigned int i=0; i<_maxItemsPerCell; i++) {
			if ((_cells[currentBin]._items[i] != NULL) && (_cells[currentBin]._items[i] != exclude1) 
				&& (_cells[currentBin]._items[i] != exclude2) && (_cells[currentBin]._items[i]->blocksLineOfSight())) {

				float temp_t;
				bool intersected;
				intersected = false;
				Ray tempRay;
				tempRay.initWithUnitInterval(r.pos, r.dir);
				tempRay.maxt = mostRecent_maxt;
				tempRay.mint = mint;
				intersected = _cells[currentBin]._items[i]->intersects(tempRay,temp_t);
				if ((intersected) && (temp_t < mostRecent_maxt)) {
					// found a valid intersection, set all the values appropriately
					validIntersectionFound = true;
					mostRecent_maxt = temp_t;
				}
			}
		}


		// if a valid intersection was found in this bin, then just return
		if (validIntersectionFound) { return false; }

		// otherwise, decide which way to move to the next grid cell
		if (txfar < tzfar) {
			// then move along the x grid cells
			if (r.dir.x < 0.0f)
				x--;
			else
				x++;
			if ((x < 0) || (x > _xNumCells)) {
				return true; // no intersection found yet, and now we're out of bounds.
			}
		}
		else {
			// then move along the z grid cells
			if (r.dir.z < 0.0f)
				z--;
			else
				z++;
			if ((z < 0) || (z > _zNumCells)) {
				return true; // no intersection found yet, and now we're out of bounds.
			}
		}

		// set up info for the next grid cell
		currentBin = getCellIndexFromGridCoords(x,z);
		getLocationFromIndex(currentBin, center);
		xlow = center.x - xOffset;
		xhi  = center.x + xOffset;
		zlow = center.z - zOffset;
		zhi  = center.z + zOffset;
		txnear = (xlow-r.pos.x) * invRayDirx;
		txfar = (xhi -r.pos.x) * invRayDirx;
		if (txnear>txfar) swap(txnear,txfar);
		tznear = (zlow-r.pos.z) * invRayDirz;
		tzfar = (zhi -r.pos.z) * invRayDirz;
		if (tznear>tzfar) swap(tznear,tzfar);

	} while ( maxt > max(txnear,tznear));  // keep doing this loop until the ray's endpoint is in the cell you just traversed

	return true;
}

bool GridDatabase2D::hasLineOfSight(const Point & p1, const Point & p2, SpatialDatabaseItemPtr exclude1, SpatialDatabaseItemPtr exclude2)
{
	Ray r;
	r.initWithUnitInterval(p1, p2-p1);
	return hasLineOfSight(r, exclude1, exclude2);
}


Point GridDatabase2D::randomPositionWithoutCollisions(float radius, bool excludeAgents)
{
	AxisAlignedBox aab(_xOrigin, _xOrigin + _xGridSize, 0.0f, 0.0f, _zOrigin, _zOrigin + _zGridSize);
	return randomPositionInRegionWithoutCollisions(aab, radius, excludeAgents);
}

Point GridDatabase2D::randomPositionInRegionWithoutCollisions(const AxisAlignedBox & region, float radius, bool excludeAgents)
{
	static MTRand _randomNumberGenerator(2);
	return randomPositionInRegionWithoutCollisions(region, radius, excludeAgents, _randomNumberGenerator);
}

Point GridDatabase2D::randomPositionInRegionWithoutCollisions(const AxisAlignedBox & region, float radius, bool excludeAgents,  MTRand & randomNumberGenerator)
{
	Point ret(0.0f, 0.0f, 0.0f);
	bool notFoundYet;
	unsigned int numTries = 0;
	float xspan = region.xmax - region.xmin - 2*radius;
	float zspan = region.zmax - region.zmin - 2*radius;

	do {

		ret.x = region.xmin + radius + ((float)randomNumberGenerator.rand(xspan));
		ret.y = 0.0f;
		ret.z = region.zmin + radius + ((float)randomNumberGenerator.rand(zspan));

		// assume this new point has no collisions, until we find out below
		notFoundYet = false;

		// check if ret collides with anything
		set<SpatialDatabaseItemPtr> neighbors;
		neighbors.clear();
		float _new_radius = radius; //  + 0.2f; // Glen testing effects for footstepAI
		getItemsInRange(neighbors, ret.x - _new_radius, ret.x + _new_radius, ret.z - _new_radius, ret.z + _new_radius, NULL);

		set<SpatialDatabaseItemPtr>::iterator neighbor;
		/*
		 * increment must be after loop body or (*neighbor)->isAgent()) will be called on an
		 * element that is beyond the set.
		 */
		for (neighbor = neighbors.begin(); neighbor != neighbors.end(); neighbor++)
		{
			if ((excludeAgents) && ((*neighbor)->isAgent()))
			{
				continue;
			}
			notFoundYet = (*neighbor)->overlaps(ret, radius);
			if (notFoundYet)
			{
				break;
			}
		}
		numTries++;
		if (numTries > 1000)
		{
			cerr << "ERROR: trying too hard to find a random position in region.  The region is probably already too dense." << endl;
			if (numTries > 10000)
			{
				throw GenericException("Gave up trying to find a random position in region.");
			}
		}
	} while (notFoundYet);

	return ret;
}

Util::Point GridDatabase2D::randomPositionInRegion(const Util::AxisAlignedBox & region, float radius,MTRand & randomNumberGenerator)
{
	Point ret(0.0f, 0.0f, 0.0f);
	float xspan = region.xmax - region.xmin - 2*radius;
	float zspan = region.zmax - region.zmin - 2*radius;

	ret.x = region.xmin + radius + ((float)randomNumberGenerator.rand(xspan));
	ret.y = 0.0f;
	ret.z = region.zmin + radius + ((float)randomNumberGenerator.rand(zspan));

	return ret;
}



bool GridDatabase2D::planPath(unsigned int startLocation, unsigned int goalLocation, std::stack<unsigned int> & outputPlan) { 
	BestFirstSearchPlanner<GridDatabasePlanningDomain, unsigned int> gridAStarPlanner;
	
	
	gridAStarPlanner.init(_planningDomain, INT_MAX);

	return gridAStarPlanner.computePlan(startLocation, goalLocation, outputPlan);
}

/*
 * This planning does not always work out perfectly
 */
bool GridDatabase2D::planPath(unsigned int startLocation, unsigned int goalLocation, std::stack<unsigned int> & outputPlan, unsigned int maxNodes) { 
	BestFirstSearchPlanner<GridDatabasePlanningDomain, unsigned int> gridAStarPlanner;
	
	
	gridAStarPlanner.init(_planningDomain, maxNodes);

	return gridAStarPlanner.computePlan(startLocation, goalLocation, outputPlan);
}

bool GridDatabase2D::findPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
		unsigned int _maxNodesToExpandForSearch)
{
	// clearing path
	path.clear ();

	int startIndex = getCellIndexFromLocation(startPosition);
	int goalIndex = getCellIndexFromLocation(endPosition);
	std::stack<unsigned int> agentPath;
	bool pathComplete = planPath(startIndex,goalIndex,agentPath,_maxNodesToExpandForSearch);

	while (agentPath.empty() == 0)
	{
		unsigned int index = agentPath.top();
		agentPath.pop();


		/*
		 * Removed to make optimal path calculations more accurate
		// skipping one waypoint
		if (agentPath.size() > 1) // there is more than 1 left
			agentPath.pop ();

		// skipping one more waypoint
		if (agentPath.size() > 1) // there is more than 1 left
			agentPath.pop ();
		 */
		Util::Point p;
		getLocationFromIndex(index,p);
		path.push_back(p);

	}

	return pathComplete;

}

/**
 * Eliminate all of the unnecessary nodes that are within sight of each other
 * Also known as the string pulling algorithm.
 *
 * Note: This algorithm ignores agents when tracing
 */
bool GridDatabase2D::findSmoothPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
		unsigned int _maxNodesToExpandForSearch)
{
	// clearing path
	path.clear ();

	int startIndex = getCellIndexFromLocation(startPosition);
	int goalIndex = getCellIndexFromLocation(endPosition);
	std::stack<unsigned int> agentPath;
	std::deque<Util::Point> plannedPath;
	Util::Point temp_p;

	bool pathComplete = planPath(startIndex,goalIndex,agentPath,_maxNodesToExpandForSearch);
	/*
	std::cout << "path length found is " << agentPath.size() << std::endl;
	int path_size = agentPath.size();
	for (int i = 0 ; i < path_size ;i++)
	{
		getLocationFromIndex(agentPath.top(),temp_p);
		agentPath.pop();
		std::cout << "point location is" << temp_p << std::endl;
	}
	*/
	if ( pathComplete == false )
	{
		return pathComplete;
	}


	/*
	 * Turns the path into a list of points appending the start and end to the path for
	 * accuracy
	 */
	while ( !agentPath.empty() )
	{
		getLocationFromIndex(agentPath.top(),temp_p);
		plannedPath.push_back(temp_p);
		agentPath.pop();
	}

	/*
	 * Most likely these are in the same grid square which causes
	 * funny business when a trace is done.
	 */
	plannedPath.pop_front();
	plannedPath.push_front(startPosition);

	plannedPath.pop_back();
	plannedPath.push_back(endPosition);


	float dummyt;
	SpatialDatabaseItemPtr dummyObject=NULL;
	unsigned int current_index=0;
	Util::Point current_p;
	Util::Point previous_p;
	Util::Point p;


	if ( plannedPath.empty() == 0 )
	{

		path.push_back(plannedPath.front());
		plannedPath.pop_front();
		// Store previous p that is used to insert when an intersection is found.
		if ( !plannedPath.empty() )
		{
			previous_p = plannedPath.front();
			plannedPath.pop_front();
		}


	}

	Ray lineOfSightRay;

	while (plannedPath.empty() == 0)
	{
		p = plannedPath.front();
		plannedPath.pop_front();
		lineOfSightRay.initWithUnitInterval(current_p, p-current_p);

		// If intersection found then need to push last point where there was
		// no intersection.
		if ( trace(lineOfSightRay,dummyt, dummyObject,
				dummyObject,true) )
		{
			path.push_back(previous_p);
			current_p = previous_p;
			previous_p = p;
			// Store previous p that is used to insert when an intersection is found.
			/*
			if ( !agentPath.empty() )
			{
				gEngine->getSpatialDatabase()->getLocationFromIndex(agentPath.top(),previous_p);
				agentPath.pop();
			}*/
		}
		previous_p = p;

	}
	path.push_back(p);

	return pathComplete;

}

