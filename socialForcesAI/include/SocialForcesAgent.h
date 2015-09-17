//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __SocialForces_AGENT__
#define __SocialForces_AGENT__

/// @file SocialForcesAgent.h
/// @brief Declares the SimpleAgent class.

#include <queue>
#include <list>
#include "SteerLib.h"
// #include "SimpleAgent.h"
// #include "SocialForcesAIModule.h"
#include "SocialForces_Parameters.h"


/**
 * @brief Social Forces Agent stuff
 *
 *
 */

// #define DRAW_ANNOTATIONS 1
// #define DRAW_HISTORIES 1
// #define DRAW_COLLISIONS 1


class SocialForcesAgent : public SteerLib::AgentInterface
{
public:
	SocialForcesAgent();
	~SocialForcesAgent();
	void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo);
	void updateAI(float timeStamp, float dt, unsigned int frameNumber);
	void disable();
	void draw();

	bool enabled() const { return _enabled; }
	Util::Point position() const { return _position; }
	Util::Vector forward() const { return _forward; }
	Util::Vector velocity() const {return _velocity; }
	float radius() const { return _radius; }
	const SteerLib::AgentGoalInfo & currentGoal() const { return _goalQueue.front(); }
	size_t id() const { return id_;}
	const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const { return _goalQueue; }
	void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for SocialForcesAgent"); }
	void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for SocialForcesAgent"); }
	void setParameters(SteerLib::Behaviour behave);
	/// @name The SteerLib::SpatialDatabaseItemInterface
	/// @brief These functions are required so that the agent can be used by the SteerLib::SpatialDataBaseInterface spatial database;
	/// The Util namespace helper functions do the job nicely for basic circular agents.
	//@{
	bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
	bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
	float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }
	//@}

	// bool collidesAtTimeWith(const Util::Point & p1, const Util::Vector & rightSide, float otherAgentRadius, float timeStamp, float footX, float footZ);
	// void insertAgentNeighbor(const SteerLib::AgentInterface * agent, float &rangeSq) {throw Util::GenericException("clearGoals() not implemented yet for SocialForcesAgent");}
	// bool compareDist(SteerLib::AgentInterface * a1, SteerLib::AgentInterface * a2 );

protected:
	/// Updates position, velocity, and orientation of the agent, given the force and dt time step.
	// void _doEulerStep(const Util::Vector & steeringDecisionForce, float dt);

	SocialForcesParameters _SocialForcesParams;

	virtual SteerLib::EngineInterface * getSimulationEngine();


	/**
		 * \brief   Updates the three-dimensional position and three-dimensional velocity of this agent.
		 */
	void update(float timeStamp, float dt, unsigned int frameNumber);


	// Stuff specific to RVO
	// should be normalized
	// Util::Vector prefVelocity_; // This is the velocity the agent wants to be at
	// Util::Vector newVelocity_;
	size_t id_;
	SteerLib::ModuleInterface * rvoModule;

	SteerLib::EngineInterface * _gEngine;

	// Used to store Waypoints between goals
	// A waypoint is choosen every FURTHEST_LOCAL_TARGET_DISTANCE

private:
	// bool runLongTermPlanning();
	// bool reachedCurrentWaypoint();
	// void updateMidTermPath();
	// bool hasLineOfSightTo(Util::Point point);


	void calcNextStep(float dt);
	Util::Vector calcRepulsionForce(float dt);
	Util::Vector calcProximityForce(float dt);

	Util::Vector calcAgentRepulsionForce(float dt);
	Util::Vector calcWallRepulsionForce(float dt);

	Util::Vector calcWallNormal(SteerLib::ObstacleInterface* obs);
	std::pair<Util::Point, Util::Point> calcWallPointsFromNormal(SteerLib::ObstacleInterface* obs, Util::Vector normal);
	Util::Vector calcObsNormal(SteerLib::ObstacleInterface* obs);

	// For midterm planning stores the plan to the current goal
	// holds the location of the best local target along the midtermpath

	friend class SocialForcesAIModule;

};


#endif
