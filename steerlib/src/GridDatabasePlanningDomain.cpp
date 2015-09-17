//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * GridDatabasePlanningDomain.cpp
 *
 *  Created on: 2015-02-05
 *      Author: gberseth
 */

#include "griddatabase/GridDatabasePlanningDomain.h"
#include <limits.h>

using namespace SteerLib;

bool GridDatabasePlanningDomain::refresh()
{
	// TODO Clear database somehow???
	this->_spatialDatabase->clearDatabase();
	for(std::set<SteerLib::ObstacleInterface*>::const_iterator iter = this->_engineInfo->getObstacles().begin(); iter != this->_engineInfo->getObstacles().end(); iter++)
	{
		// std::cout << "Refreshing GridDataBasePlanningDomain" << std::endl;
		this->_spatialDatabase->addObject(*iter, (*iter)->getBounds());
	}

	return true;
}

bool GridDatabasePlanningDomain::planPath(unsigned int startLocation, unsigned int goalLocation, std::stack<unsigned int> & outputPlan) {
	BestFirstSearchPlanner<GridDatabasePlanningDomain, unsigned int> gridAStarPlanner;


	gridAStarPlanner.init(this, INT_MAX);

	return gridAStarPlanner.computePlan(startLocation, goalLocation, outputPlan);
}

/*
 * This planning does not always work out perfectly
 */
bool GridDatabasePlanningDomain::planPath(unsigned int startLocation, unsigned int goalLocation, std::stack<unsigned int> & outputPlan, unsigned int maxNodes) {
	BestFirstSearchPlanner<GridDatabasePlanningDomain, unsigned int> gridAStarPlanner;


	gridAStarPlanner.init(this, maxNodes);

	return gridAStarPlanner.computePlan(startLocation, goalLocation, outputPlan);
}

bool GridDatabasePlanningDomain::findPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
		unsigned int _maxNodesToExpandForSearch)
{
	// clearing path
	path.clear ();

	int startIndex = _spatialDatabase->getCellIndexFromLocation(startPosition);
	int goalIndex = _spatialDatabase->getCellIndexFromLocation(endPosition);
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
		_spatialDatabase->getLocationFromIndex(index,p);
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
bool GridDatabasePlanningDomain::findSmoothPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
		unsigned int _maxNodesToExpandForSearch)
{
	// clearing path
	path.clear ();

	int startIndex = _spatialDatabase->getCellIndexFromLocation(startPosition);
	int goalIndex = _spatialDatabase->getCellIndexFromLocation(endPosition);
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
		_spatialDatabase->getLocationFromIndex(agentPath.top(),temp_p);
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
		if ( _spatialDatabase->trace(lineOfSightRay,dummyt, dummyObject,
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
