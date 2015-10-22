//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/*
 * AgentInterface.cpp
 *
 *  Created on: 2013-12-05
 *      Author: glenpb
 */

#include "interfaces/AgentInterface.h"
#include "SteerLib.h"

using namespace Util;
using namespace SteerLib;


bool AgentInterface::hasLineOfSightTo(Util::Point target)
{
	float dummyt;
	Util::Vector _rightSide = rightSideInXZPlane(this->forward()*this->radius());
	SpatialDatabaseItemPtr dummyObject;
	Ray lineOfSightTestRight, lineOfSightTestLeft, lineOfSightTestCentre, lineOfSightTestBack;
	Util::Point tmp_pos;
	// lineOfSightTestRight.initWithUnitInterval(_position + _radius*_rightSide, target - (_position + _radius*_rightSide));
	// lineOfSightTestLeft.initWithUnitInterval(_position + _radius*(_rightSide), target - (_position - _radius*_rightSide));
	tmp_pos = _position + _rightSide + (Util::Vector(0.0,0.0,0.0));
	lineOfSightTestRight.initWithUnitInterval(tmp_pos, target - tmp_pos);
	tmp_pos = _position + (-_rightSide) + (Util::Vector(0.0,0.0,0.0));
	lineOfSightTestLeft.initWithUnitInterval(tmp_pos, target - tmp_pos);
	tmp_pos = _position - this->forward()*this->radius() ;
	lineOfSightTestBack.initWithUnitInterval(tmp_pos, target - tmp_pos);
	lineOfSightTestCentre.initWithUnitInterval(_position, target - _position);


	return !( (getSimulationEngine()->getSpatialDatabase()->trace(lineOfSightTestRight,dummyt, dummyObject, dynamic_cast<SpatialDatabaseItemPtr>(this),true))
		|| (getSimulationEngine()->getSpatialDatabase()->trace(lineOfSightTestLeft,dummyt, dummyObject, dynamic_cast<SpatialDatabaseItemPtr>(this),true))
		|| (getSimulationEngine()->getSpatialDatabase()->trace(lineOfSightTestBack,dummyt, dummyObject, dynamic_cast<SpatialDatabaseItemPtr>(this),true)) );

	// return !( (getSimulationEngine()->getSpatialDatabase()->trace(lineOfSightTestCentre,dummyt, dummyObject, dynamic_cast<SpatialDatabaseItemPtr>(this),true)) );

}

/**
 * Update the local target to the furthest point on the midterm path the agent can see.
 */
void AgentInterface::updateLocalTarget()
{
	// Util::Point tmpTarget = this->_goalQueue.front().targetLocation;
	Util::Point tmpTarget = this->_midTermPath.at(0);
	// std::cout << "Size of mid term path: " << this->_midTermPath.size() << std::endl;
	int i=0;
	for (i=0; (i < FURTHEST_LOCAL_TARGET_DISTANCE) &&
			i < this->_midTermPath.size(); i++ )
	{
		tmpTarget = this->_midTermPath.at(i);
		if ( this->hasLineOfSightTo(tmpTarget) )
		{
			this->_currentLocalTarget = tmpTarget;

		}
		else
		{ // test for RVO2D
			break;
		}
	}
	// std::cout << "Can see to midTermPath point: " << i << std::endl;
	// This makes SF TOO GOOD
	for (int j=0; (j < (i-2)) && (i > 3) && (this->_midTermPath.size() > 1 ); j++)
	{// remove points behind where the agent can see
		this->_midTermPath.pop_front();
	}
}

void AgentInterface::updateLocalTarget2()
{
	Util::Point tmpTarget = this->_goalQueue.front().targetLocation;
	unsigned int i=0;
	for (i=0; (i < FURTHEST_LOCAL_TARGET_DISTANCE) &&
			i < this->_midTermPath.size(); i++ )
	{
		tmpTarget = this->_midTermPath.at(i);
		if ( this->hasLineOfSightTo(tmpTarget) )
		{
			this->_currentLocalTarget = tmpTarget;
		}
	}
}

bool AgentInterface::runLongTermPlanning(Util::Point goalLocation, bool dontPlan)
{
	_midTermPath.clear();
	_waypoints.clear();
	if (dontPlan)
	{
		// _waypoints should be empty
		_waypoints.push_back(goalLocation);
		// std::cout << "Agent " << this->_id << " Skipping planning and returning goal location" << std::endl;
		return true;
	}
	//==========================================================================

	// run the main a-star search here
	std::vector<Util::Point> agentPath;
	Util::Point pos =  position();
	// std::cout << "this is the planner AgentInterface:" << getSimulationEngine()->getPathPlanner() << std::endl;
	SteerLib::PlanningDomainInterface * planner = getSimulationEngine()->getPathPlanner();
	if ( !planner->findPath(pos, goalLocation,
			agentPath, (unsigned int) 100000))
	{
		return false;
	}

	for  (int i=1; i <  agentPath.size(); i++)
	{
		_midTermPath.push_back(agentPath.at(i));
		if ((i % FURTHEST_LOCAL_TARGET_DISTANCE) == 0)
		{
			_waypoints.push_back(agentPath.at(i));
		}
	}
	_waypoints.push_back(goalLocation);
	return true;
}

bool AgentInterface::runLongTermPlanning2(Util::Point goalLocation, bool dontPlan)
{
	_midTermPath.clear();
	_waypoints.clear();
	if (dontPlan)
	{
		// _waypoints should be empty
		_waypoints.push_back(goalLocation);
		// std::cout << "Agent " << this->_id << " Skipping planning and returning goal location" << std::endl;
		return true;
	}
	//==========================================================================

	// run the main a-star search here
	// std::cout << "Planning again angent" << this->id() << " goal: " << _goalQueue.front().targetLocation << std::endl;
	// run the main a-star search here
	std::vector<Util::Point> agentPath;
	Util::Point pos =  position();

	if ( !getSimulationEngine()->getPathPlanner()->findSmoothPath(pos, _goalQueue.front().targetLocation,
			agentPath, (unsigned int) 100000))
	{
		return false;
	}

	// Push path into _waypoints

	// Skip first node that is at location of agent
	for  (int i=1; i <  agentPath.size(); i++)
	{
		_waypoints.push_back(agentPath.at(i));

	}

	return true;
}

void AgentInterface::insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq)
{
	if (this != agent) {
		const float distSq = (position() - (agent->position())).lengthSquared();

		if (distSq < rangeSq) {
			if (agentNeighbors_.size() < AGENT_NEIGHBOURS) {
				agentNeighbors_.push_back(std::make_pair(distSq, agent));
			}

			size_t i = agentNeighbors_.size() - 1;

			while (i != 0 && distSq < agentNeighbors_[i - 1].first) {
				agentNeighbors_[i] = agentNeighbors_[i - 1];
				--i;
			}

			agentNeighbors_[i] = std::make_pair(distSq, agent);

			if (agentNeighbors_.size() == AGENT_NEIGHBOURS) {
				rangeSq = agentNeighbors_.back().first;
			}
		}
	}
}

void AgentInterface::insertObstacleNeighbor(const ObstacleInterface *obstacle, float rangeSq)
{
	const ObstacleInterface *const nextObstacle = obstacle->nextObstacle_;

	const float distSq = distSqPointLineSegment(obstacle->point_, nextObstacle->point_, position());

	if (distSq < rangeSq) {
		obstacleNeighbors_.push_back(std::make_pair(distSq, obstacle));

		size_t i = obstacleNeighbors_.size() - 1;

		while (i != 0 && distSq < obstacleNeighbors_[i - 1].first) {
			obstacleNeighbors_[i] = obstacleNeighbors_[i - 1];
			--i;
		}

		obstacleNeighbors_[i] = std::make_pair(distSq, obstacle);
	}
}

bool AgentInterface::reachedCurrentWaypoint()
{

	if ( !_waypoints.empty())
	{
		return (position() - _waypoints.front()).lengthSquared() <= (radius()*radius());
	}
	else
	{
		return false;
	}
	// return (position() - _currentLocalTarget).lengthSquared() < (radius()*radius());
}

void AgentInterface::updateMidTermPath()
{
	if ( this->_midTermPath.size() < FURTHEST_LOCAL_TARGET_DISTANCE)
	{
		return;
	}
	if ( !_waypoints.empty())
	{
		_waypoints.erase(_waypoints.begin());
	}
	// save good path
	std::vector<Util::Point> tmpPath;
	// std::cout << "midterm path size " << _midTermPath.size() << std::endl;
	// std::cout << "distance between position and current waypoint " << (position() - _waypoints.front()).length() << std::endl;
	for (unsigned int i=(FURTHEST_LOCAL_TARGET_DISTANCE); i < _midTermPath.size();i++ )
	{
		tmpPath.push_back(_midTermPath.at(i));
	}
	_midTermPath.clear();

	for (unsigned int i=0; i < tmpPath.size(); i++)
	{
		_midTermPath.push_back(tmpPath.at(i));
	}

}

void AgentInterface::draw()
{
#ifdef ENABLE_GUI

	#ifdef DRAW_HISTORIES
		__oldPositions.push_back(position());
		float mostPoints = 100.0f;
		while ( __oldPositions.size() > mostPoints )
		{
			__oldPositions.pop_front();
		}
		for (int q = __oldPositions.size()-1 ; q > 0 && __oldPositions.size() > 1; q--)
		{
			DrawLib::drawLineAlpha(__oldPositions.at(q), __oldPositions.at(q-1),gBlack, q/(float)__oldPositions.size());
		}

	#endif
	Util::Vector adjust = Util::Vector(0.0,0.6,0.0);
	if (getSimulationEngine()->isAgentSelected(this))
	{
		Util::Ray ray;
		ray.initWithUnitInterval(_position, _forward);
		Util::DrawLib::drawLine(ray.pos + adjust, ray.eval(1.0f));
		// if (getSimulationEngine()->getSpatialDatabase()->trace(ray, t, objectFound, this, false))
		{
			Util::DrawLib::drawAgentDisc(_position, _forward, _radius, Util::gBlue);
		}
		// else
		{
			// Util::DrawLib::drawAgentDisc(_position, _forward, _radius);
		}
		Util::DrawLib::drawFlag( this->currentGoal().targetLocation, Color(0.5f,0.8f,0), 2);
		if ( this->currentGoal().goalType == GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL )
		{
			Color color(0.4,0.9,0.4);
			DrawLib::glColor(color);
			DrawLib::drawQuad(Point(this->currentGoal().targetRegion.xmin, 0.1, this->currentGoal().targetRegion.zmin),
					Point(this->currentGoal().targetRegion.xmin, 0.1, this->currentGoal().targetRegion.zmax),
					Point(this->currentGoal().targetRegion.xmax, 0.1, this->currentGoal().targetRegion.zmax),
					Point(this->currentGoal().targetRegion.xmax, 0.1, this->currentGoal().targetRegion.zmin));
		}

		if ( _midTermPath.size() > 0 )
		{
			// DrawLib::drawStar(_midTermPath.at(0), Util::Vector(1,0,0), 0.34f, gBlue);
			DrawLib::drawLine(this->position(), _midTermPath.at(0), gOrange, 1.5f);
		}
		for (size_t p=1; ( _midTermPath.size() > 1 ) && p < _midTermPath.size(); p++)
		{
			DrawLib::drawLine(_midTermPath.at(p), _midTermPath.at(p-1), gOrange, 1.5f);
			//  DrawLib::drawStar(_midTermPath.at(p), Util::Vector(1,0,0), 0.34f, gOrange);
			// std::cout << "point " << p << ", " << _midTermPath.at(p) << std::endl;
		}
		int i;
		for (i=0; ( _waypoints.size() > 1 ) && (i < (_waypoints.size() - 1)); i++)
		{
			// DrawLib::drawLine(_waypoints.at(i)+adjust, _waypoints.at(i+1)+adjust, gYellow);
			// DrawLib::drawStar(_waypoints.at(i)+adjust, Util::Vector(1,0,0), 0.34f, gBlue);
		}
		// DrawLib::drawStar(_waypoints.at(i), Util::Vector(1,0,0), 0.34f, gBlue);
		// draw line to current localTarget
		// DrawLib::drawLine(position()+adjust, this->_currentLocalTarget+adjust, gYellow);

		// DrawLib::drawLine(position()+adjust, position()+_prefVelocity+adjust, Util::gCyan, 20.0f);
		if (false)
		{
			Util::Vector _rightSide = rightSideInXZPlane(this->forward()*this->radius());
			Ray lineOfSightTestRight, lineOfSightTestLeft, lineOfSightTestCentre, lineOfSightTestBack;
			Util::Point tmp_pos;
			// lineOfSightTestRight.initWithUnitInterval(_position + _radius*_rightSide, target - (_position + _radius*_rightSide));
			// lineOfSightTestLeft.initWithUnitInterval(_position + _radius*(_rightSide), target - (_position - _radius*_rightSide));
			tmp_pos = _position + _rightSide + (Util::Vector(0.0,0.0,0.0));
			lineOfSightTestRight.initWithUnitInterval(tmp_pos, this->_currentLocalTarget - tmp_pos);
			DrawLib::drawRay(lineOfSightTestRight, gGray60);
			tmp_pos = _position + (-_rightSide) + (Util::Vector(0.0,0.0,0.0));
			lineOfSightTestLeft.initWithUnitInterval(tmp_pos, this->_currentLocalTarget - tmp_pos);
			DrawLib::drawRay(lineOfSightTestLeft, gGray60);
			tmp_pos = _position - this->forward()*this->radius() ;
			lineOfSightTestBack.initWithUnitInterval(tmp_pos, this->_currentLocalTarget - tmp_pos);
			DrawLib::drawRay(lineOfSightTestBack, gGray60);
			lineOfSightTestCentre.initWithUnitInterval(_position, this->_currentLocalTarget - _position);
		}

	}
	else {
		Util::DrawLib::drawAgentDisc(_position, _radius, this->_color);
	}
	if (_goalQueue.front().goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
		Util::DrawLib::drawFlag(_goalQueue.front().targetLocation);
	}

#endif
}
