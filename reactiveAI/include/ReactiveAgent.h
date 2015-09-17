//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __PPR_AGENT_H__
#define __PPR_AGENT_H__

/// @file ReactiveAgent.h
/// @brief Declares the ReactiveAgent class.

#include "SteerLib.h"
#include "ReactiveParameters.h"

// #define USE_ANNOTATIONS

#define AGENT_PTR(agent) (dynamic_cast<AgentInterface*>(agent))


//======================================================================================
// forward declarations
//======================================================================================

class ReactiveAgent;

namespace ReactiveGlobals {
	// NOTE this is forward declared for an inline function defined below
	// other declarations for this namespace belong in PPRAIModule.h
	extern SteerLib::EngineInterface * gEngineInfo;
}


//======================================================================================
// helper data structures
//======================================================================================


//
// PredictedThreat - data that describes a threat that was found by the predictive phase.
//
struct PredictedThreat {
	enum ThreatTypeEnum { THREAT_TYPE_UNKNOWN, THREAT_TYPE_ONCOMING, THREAT_TYPE_CROSSING_SOON, THREAT_TYPE_CROSSING_LATE };

	// the agent that the threat is predicted on
	ReactiveAgent * threatGuy;
	// interval of time that the threat is expected
	float minTime, maxTime, originalMaxTime;
	// the type of threat it is
	ThreatTypeEnum threatType;
	// indicates whether the threat is imminent, or already avoided, but still cannot steer normally.
	bool imminent;
	bool oncomingToRightSide;
};


//
// FeelerInfo - the "t" parameters and object references that result from tracing the agent's "feelers" in the reactive phase.
//
struct FeelerInfo {
	void clear() {
		object_front = object_right = object_left = object_rside = object_lside = NULL;
		t_front = t_right = t_left = t_rside = t_lside = INFINITY;
	}
	float t_front;
	float t_right;
	float t_left;
	float t_rside;
	float t_lside;
	SteerLib::SpatialDatabaseItemPtr object_front;
	SteerLib::SpatialDatabaseItemPtr object_right;
	SteerLib::SpatialDatabaseItemPtr object_left;
	SteerLib::SpatialDatabaseItemPtr object_rside;
	SteerLib::SpatialDatabaseItemPtr object_lside;
};


//======================================================================================
// The actual ReactiveAgent class
//======================================================================================

class ReactiveAgent : public SteerLib::AgentInterface
{
public:
	// AgentInterface functionality:
	ReactiveAgent();
	~ReactiveAgent();
	void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo);
	void updateAI(float timeStamp, float dt, unsigned int frameNumber);
	void draw();


	bool enabled() const { return _enabled; }
	Util::Point position() const { return _position; }
	Util::Vector forward() const { return _forward; }
	float radius() const { return _radius; }
	const SteerLib::AgentGoalInfo & currentGoal() const { return _currentGoal; }
	void addGoal(const SteerLib::AgentGoalInfo & newGoal);
	size_t id() const { return 0;}
	const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const { throw Util::GenericException("agentGoals() not implemented yet"); }
	void clearGoals() {
		while (!_landmarkQueue.empty()) _landmarkQueue.pop();
		_nextFrameToRunLongTermPlanningPhase = 0;
		_nextFrameToRunMidTermPlanningPhase = 0;
		_nextFrameToRunShortTermPlanningPhase = 0;
		_nextFrameToRunPerceptivePhase = 0;
		_nextFrameToRunPredictivePhase = 0;
		_nextFrameToRunReactivePhase = 0;
	}


	bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
	bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
	float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }

	void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) { throw Util::GenericException("insertAgentNeighbor not implemented yet for BenchmarkAgent"); }
	void setParameters(SteerLib::Behaviour behave)
	{
		throw Util::GenericException("setParameters() not implemented yet for this Agent");
	}

	// public enum types:
	enum SteeringStateEnum { STEERING_STATE_NO_THREAT,  STEERING_STATE_PROACTIVELY_AVOID,  STEERING_STATE_WAIT_UNTIL_CLEAR,
		STEERING_STATE_STATIONARY_OBJECT,  STEERING_STATE_TURN_TOWARDS_TARGET,  STEERING_STATE_COOPERATE_WITH_CROWD,
		STEERING_STATE_FOLLOW_SPACETIME_PATH };

	enum ReactiveSituationEnum { REACTIVE_SITUATION_ONE_AGENT,  REACTIVE_SITUATION_TWO_AGENTS,  REACTIVE_SITUATION_THREE_AGENTS,
		REACTIVE_SITUATION_STATIC_OBJECTS_ZERO_AGENTS,  REACTIVE_SITUATION_STATIC_OBJECTS_ONE_AGENT,  REACTIVE_SITUATION_STATIC_OBJECTS_TWO_AGENTS, 
		REACTIVE_SITUATION_NO_THREATS,  REACTIVE_SITUATION_UNKNOWN };

	// native functionality:
	SteeringStateEnum steeringState() { return _steeringState; }
	Util::Vector velocity() const { return _forward * _currentSpeed; }
	Util::Vector rightSide() { return _rightSide; }
	Util::Point localTargetLocation() { return _localTargetLocation; }
	Util::Vector localTargetDirection() { return _finalSteeringCommand.targetDirection; }
	bool isSelected() { 
		return ReactiveGlobals::gEngineInfo->isAgentSelected(this);
	}


protected:
	//========================
	// private functionality:
	//========================

	// phases of AI computation; the ultimate output of these phases is a steering command.
	void runCognitivePhase();
	void runLongTermPlanningPhase();
	void runMidTermPlanningPhase();
	void runShortTermPlanningPhase();
	void runPerceptivePhase();
	void runPredictivePhase();
	void runReactivePhase();

	// given a steering command, these functions do the actual steering.
	void doSteering();
	void doDynamicsSteering();
	void doCommandBasedSteering();
	void doEulerStepWithForce(const Util::Vector & force); // updates _position, _velocity, and _currentSpeed
	
	// helper functions
	bool updateReactiveFeelers( FeelerInfo & feelers );  // returns false if the 3 front rays don't intersect anything, even if the rside/lside rays do.
	void collectObjectsInVisualField();
	bool reachedCurrentGoal();
	bool reachedCurrentWaypoint();
	bool reachedLocalTarget();
	bool threatListContainsAgent(ReactiveAgent * agent, unsigned int &index);
	inline bool threatListContainsAgent(ReactiveAgent * agent) { unsigned int dummy; return threatListContainsAgent(agent, dummy); }
	void disable();
	void drawPlannedPath();

	//========================
	// private data:
	//========================

	// LONG-TERM PLANNING PHASE
	std::vector<Util::Point> _waypoints;
	int _currentWaypointIndex;

	// MID-TERM PLANNING PHASE
	int _midTermPath[PED_NEXT_WAYPOINT_DISTANCE+2];  // "+2" is a very terrible hack to avoid bugs.
	unsigned int _midTermPathSize;

	// SHORT-TERM PLANNING PHASE
	Util::Point _localTargetLocation;

	// PERCEPTION PHASE
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	unsigned int _numAgentsInVisualField;  // different than _neighbors.size(), which includes static objects.

	// PREDICTION PHASE
	float _timeToWait;
	float _minThreatTime;
	float _maxThreatTime;
	int _mostImminentThreatIndex;
	std::vector<PredictedThreat> _threatList;
	Util::Vector _crowdControlDirection;
	SteeringStateEnum _steeringState;

	// REACTIVE PHASE
	SteerLib::SteeringCommand _finalSteeringCommand;

	// PHASE SCHEDULERS
	// NOTE: In some cases (planning phases in particular) the phases may be called
	// on-demand sooner than scheduled.

	// schedules for each phase
	unsigned int _nextFrameToRunLongTermPlanningPhase;
	unsigned int _nextFrameToRunMidTermPlanningPhase;
	unsigned int _nextFrameToRunShortTermPlanningPhase;
	unsigned int _nextFrameToRunPerceptivePhase;
	unsigned int _nextFrameToRunPredictivePhase;
	unsigned int _nextFrameToRunReactivePhase;

	unsigned int _lastFrameLongTermWasCalled;
	unsigned int _lastFrameMidTermWasCalled;
	unsigned int _lastFrameShortTermWasCalled;
	unsigned int _lastFramePerceptiveWasCalled;
	unsigned int _lastFramePredictiveWasCalled;
	unsigned int _lastFrameReactiveWasCalled;

	// used for dynamic adaptation of schedules
	unsigned int _framesToNextLongTermPlanning;
	unsigned int _framesToNextMidTermPlanning;
	unsigned int _framesToNextShortTermPlanning;
	unsigned int _framesToNextPerceptivePhase;
	unsigned int _framesToNextPredictivePhase;
	unsigned int _framesToNextReactivePhase;


	// GEOMETRY STATE of the agent (can potentially change per frame)
	float _radius;
	Util::Point _position;
	Util::Vector _forward;
	Util::Vector _rightSide;

	// PHYSICS STATE of the agent
	Util::Vector _velocity;
	float _mass;
	float _maxSpeed;
	float _maxForce;
	float _currentSpeed;

	// OTHER STATE
	bool _enabled;
	
	// TODO THESE VALUES SHOULD BE MOVED TO ReactiveGlobals namespace, they are just wasting space per agent.
	// there is one issue with _currentFrameNumber that has to be checked first, though.
	float _currentTimeStamp;
	float _dt;
	unsigned int _currentFrameNumber;

	// GOAL information
	SteerLib::AgentGoalInfo _currentGoal;
	std::queue<SteerLib::AgentGoalInfo> _landmarkQueue;
// #define DRAW_HISTORIES 1

#ifdef DRAW_HISTORIES
	std::vector<Util::Point> __oldPositions;
#endif

	// Adding a bunch of

	//
	// NOTE CAREFULLY these annotation varibales have TWO UNDERSCORES prefix.
	//
#ifdef USE_ANNOTATIONS
	Util::Ray __myRay, __myLeftRay, __myRightRay, 
		__myPredictorRay, __myPredictorLeftRay, __myPredictorRightRay, 
		__myLSideRay, __myRSideRay;
	bool __front, __leftFront, __rightFront, __leftSide, __rightSide;
	SpatialDatabaseItem* __objectInPath;
	bool __comfortZoneViolated;
	bool __hitSomething;
	Util::Vector __plannedSteeringForce;
	Util::Point __hitPosFront, __hitPosRight, __hitPosLeft;
	float __threat_min_t, __threat_max_t;
	unsigned int __closestPathNode;
	std::stack<unsigned int> midTermPathStack;
	std::stack<unsigned int> longTermPath; // Should be changed to vectors
#endif

};


#endif
