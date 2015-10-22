//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * KdTreeDataBase.cpp
 *
 *  Created on: 2014-11-27
 *      Author: gberseth
 */

#include "KdTreeDataBase.h"
#include "util/DrawLib.h"

using namespace SteerLib;

#include "obstacles/BoxObstacle.h"

#include "SteerLib.h"
#include "util/DrawLib.h"

#include <vector>
#include <cmath>
#include <ctime>


using namespace SteerLib;
using namespace Util;

/*
void KdTreeDataBase::init()
{
 // Does nothing
}*/



KdTreeDataBase::KdTreeDataBase(EngineInterface * gEngine, float xmin, float xmax, float zmin, float zmax)
{
	// this->_mesh = mesh;
	this->_spatialDatabase = new KdTree();
	this->_spatialDatabase->setSimulator(gEngine);
	this->_engine = gEngine;

	_xOrigin = xmin;
	_zOrigin = zmin;
	_xGridSize = xmax - xmin;
	_zGridSize = zmax - zmin;
}

void KdTreeDataBase::getItemsInRange(std::set<SpatialDatabaseItemPtr> & neighborList, float xmin, float xmax, float zmin, float zmax, SpatialDatabaseItemPtr exclude)
{
	neighborList.clear();
	// std::cout << "kdtree update agent neighbours" <<  " pointer " << this << std::endl;
	AgentInterface * agent = dynamic_cast<AgentInterface*>(exclude);
	if ( agent == NULL )
	{
		std::cout << "agent pointer is null for get neighbours" << std::endl;
	}
	// std::cout << "agent id:" << agent->id() << std::endl;
	this->_spatialDatabase->computeAgentNeighbors(dynamic_cast<AgentInterface*>(exclude), 10.0f );
	for (size_t a=0; a < agent->agentNeighbors_.size(); a++)
	{
		if (const_cast<AgentInterface*>(agent->agentNeighbors_.at(a).second)->enabled())
		{
			neighborList.insert(const_cast<AgentInterface*>(agent->agentNeighbors_.at(a).second));
		}
		// agent->agentNeighbors_.at(a).second;
	}

	this->_spatialDatabase->computeObstacleNeighbors(dynamic_cast<AgentInterface*>(exclude), 10.0f );
	for (size_t a=0; a < agent->obstacleNeighbors_.size(); a++)
	{
		neighborList.insert(const_cast<ObstacleInterface*>(agent->obstacleNeighbors_.at(a).second));
		// agent->agentNeighbors_.at(a).second;
	}

}


void KdTreeDataBase::draw()
{
	// std::cout << "kdtree update draw" << std::endl;
	// _spatialDatabase->buildObstacleTree();
	// _spatialDatabase->buildAgentTree();
}

void KdTreeDataBase::buildObstacleTree()
{
	_spatialDatabase->buildObstacleTree();
}

void KdTreeDataBase::buildAgentTree()
{
	_spatialDatabase->buildAgentTree();
}

Util::Point KdTreeDataBase::randomPositionWithoutCollisions(float radius, bool excludeAgents)
{
	srand (static_cast <unsigned> (time(0)));

	size_t numTries = 0;
	bool found_hit = false;
	float rx = 0;
	float rz = 0;
	float r = 0;
	float r2 = 0;

	AxisAlignedBox region(_xOrigin, _xOrigin + _xGridSize , 0, 1, _zOrigin, _zOrigin + _xGridSize );

	Util::Point out = Util::Point(rx,0,rz);
	while(found_hit)
	{
		//srandtd::cout << "Generated new random position: " << Util::Point(rx,0,rz) << std::endl;
		found_hit = false;

		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		rx = (r * (region.xmax - region.xmin)) + region.xmin;

		r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		rz = (r2 * (region.zmax - region.zmin)) + region.zmin;
		//std::cout << "Normalized R: " << Util::Point(r,0,r2) << " | Bounded R: " << Util::Point(rx,0,rz) << std::endl;

		std::vector<SteerLib::AgentInterface*> agents_list = this->_engine->getAgents();
		//std::cout << "Number of Agents: " << agents_list.size() << std::endl;


		for ( SteerLib::AgentInterface * ait : agents_list )
		{
			if (Util::circleOverlapsCircle2D(Util::Point(rx,0,rz), radius, (ait)->position(), (ait)->radius()))
			{
				found_hit = true;
				break;
			}
		}
		//Check if agent position overlaps with obstacles
		// std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
		for (std::set<SteerLib::ObstacleInterface*>::iterator neighbor = this->_engine->getObstacles().begin();  neighbor != this->_engine->getObstacles().end();  neighbor++)
		{
			SteerLib::ObstacleInterface * obstacle;
			obstacle = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbor);
			if(obstacle->overlaps(Util::Point(rx,0,rz), radius) == true)
			{
				found_hit = true;
				break;
			}
		}

		numTries++;
		if (numTries > 1000)
		{
			std::cerr << "ERROR: trying too hard to find a random position in region.  The region is probably already too dense." << std::endl;
			if (numTries > 10000)
			{
				throw GenericException("Gave up trying to find a random position in region.");
			}
		}
	}

	return out;
}
/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius.
Util::Point KdTreeDataBase::randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, float radius, bool excludeAgents)
{
	return Util::Point(0,0,0);
}

/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius, using an exising (already seeded) Mersenne Twister random number generator.
Util::Point KdTreeDataBase::randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, float radius, bool excludeAgents, MTRand & randomNumberGenerator)
{
	// srand (static_cast <unsigned> (time(0)));

	bool found_hit = true;
	float rx = 0;
	float rz = 0;
	float r = 0;
	float r2 = 0;
	size_t numTries = 0;
	

	while(found_hit)
	{
		//srandtd::cout << "Generated new random position: " << Util::Point(rx,0,rz) << std::endl;
		found_hit = false;
		
		r = randomNumberGenerator.rand();
		// r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		rx = (r * (region.xmax - region.xmin)) + region.xmin;
		
		// r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		r2 = randomNumberGenerator.rand();
		rz = (r2 * (region.zmax - region.zmin)) + region.zmin;

		//std::cout << "Normalized R: " << Util::Point(r,0,r2) << " | Bounded R: " << Util::Point(rx,0,rz) << std::endl;
				
		std::vector<SteerLib::AgentInterface*> agents_list = this->_engine->getAgents();
		//std::cout << "Number of Agents: " << agents_list.size() << std::endl;
		

		for ( SteerLib::AgentInterface * ait : agents_list )
		{
			if (Util::circleOverlapsCircle2D(Util::Point(rx,0,rz), radius, (ait)->position(), (ait)->radius()))
			{
				found_hit = true;
				break;
			}
		}
		//Check if agent position overlaps with obstacles
		// std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
		for (std::set<SteerLib::ObstacleInterface*>::iterator neighbor = this->_engine->getObstacles().begin();  neighbor != this->_engine->getObstacles().end();  neighbor++)
		{
			SteerLib::ObstacleInterface * obstacle;
			obstacle = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbor);
			if(obstacle->overlaps(Util::Point(rx,0,rz), radius) == true)
			{
				found_hit = true;
				break;
			}
		}

		numTries++;
		if (numTries > 1000)
		{
			std::cerr << "ERROR: trying too hard to find a random position in region.  The region is probably already too dense." << std::endl;
			if (numTries > 10000)
			{
				throw GenericException("Gave up trying to find a random position in region.");
			}
		}		
	}
	
	return Util::Point(rx,0,rz);
}

/// Finds a random 2D point, within the specified region, that has no other objects within the requested radius, using an exising (already seeded) Mersenne Twister random number generator.
bool KdTreeDataBase::randomPositionInRegionWithoutCollisions(const Util::AxisAlignedBox & region, SpatialDatabaseItemPtr item, bool excludeAgents, MTRand & randomNumberGenerator)
{
	return true;
}


/// Finds a random 2D point, within the specified region, using an exising (already seeded) Mersenne Twister random number generator.
Util::Point KdTreeDataBase::randomPositionInRegion(const Util::AxisAlignedBox & region, float radius,MTRand & randomNumberGenerator)
{
	return Util::Point(0,0,0);
}



/**
 * \brief   Computes the agent neighbors of the specified agent.
 * \param   agent    A pointer to the agent for which agent neighbors are to be computed.
 * \param   rangeSq  The squared range around the agent.
 */
void KdTreeDataBase::computeAgentNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const
{
	// std::cout << "Pointer to spatialDatabase: " << this << std::endl;
	this->_spatialDatabase->computeAgentNeighbors(agent, rangeSq);
}

/**
 * \brief      Computes the obstacle neighbors of the specified agent.
 * \param      agent           A pointer to the agent for which obstacle
 *                             neighbors are to be computed.
 * \param      rangeSq         The squared range around the agent.
 */
void KdTreeDataBase::computeObstacleNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const
{
	this->_spatialDatabase->computeObstacleNeighbors(agent, rangeSq);
}


void KdTreeDataBase::clearDatabase()
{
	this->_spatialDatabase->agents_.clear();
	this->_spatialDatabase->agentTree_.clear();
	// this->_spatialDatabase->deleteObstacleTree(this->_spatialDatabase->obstacleTree_);
}

void KdTreeDataBase::addObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox & newBounds )
{
	if ( item->isAgent())
	{
		this->_spatialDatabase->agents_.push_back(dynamic_cast<AgentInterface *>(item));
	}
	else
	{

	}
}

/*
 * Very in efficient
 */
void KdTreeDataBase::removeObject( SpatialDatabaseItemPtr item, const Util::AxisAlignedBox &oldBounds )
{
	if ( item->isAgent())
	{

		for (std::vector<AgentInterface*>::iterator it = this->_spatialDatabase->agents_.begin();
				it != this->_spatialDatabase->agents_.end(); it++)
		{
			if ( (*it) == item)
			{
				this->_spatialDatabase->agents_.erase(it);
				break;
			}
		}
		/*
		this->_spatialDatabase->agents_.erase(std::remove_if(this->_spatialDatabase->agents_.begin(),
				this->_spatialDatabase->agents_.end(),
							   [](SpatialDatabaseItemPtr _item, ) { return _item == item; }), this->_spatialDatabase->agents_.end());
		*/
	}
	else
	{

	}

}

/*
 * Oh the incredible slowness...
 */
bool KdTreeDataBase::trace(const Util::Ray & r, float & t, SpatialDatabaseItemPtr &hitObject, SpatialDatabaseItemPtr exclude, bool excludeAgents)
{
	float current_closest_dist = 1000000.0f;
	bool found_hit = false;
	// std::cout << "Looking for obstacle colisions" << std::endl;
	for ( std::set<SteerLib::ObstacleInterface*>::iterator oit = this->_engine->getObstacles().begin();
			oit != this->_engine->getObstacles().end(); oit++)
	{
		// std::cout << "Looking for obstacle colissions" << std::endl;
		Ray tempRay;
		float temp_t;
		tempRay.initWithUnitInterval(r.pos, r.dir);
		tempRay.maxt = current_closest_dist;
		tempRay.mint = 0.0f;
		// intersected = _cells[currentBin]._items[i]->intersects(tempRay,temp_t);
		if ( (*oit)->intersects(tempRay, temp_t ) && ((*oit) != exclude) )
		{ // There is a potential intersection,
			if ( temp_t < r.maxt )
			{ // check if intersection is before ray time limit
				current_closest_dist = temp_t;
				hitObject = (*oit);
				found_hit=true;
				// std::cout << "Hit obstacle" << std::endl;
			}
		}

	}
	return found_hit;
}
