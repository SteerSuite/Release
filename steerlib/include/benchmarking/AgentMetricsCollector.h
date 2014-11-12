//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_AGENT_METRICS_COLLECTOR_H__
#define __STEERLIB_AGENT_METRICS_COLLECTOR_H__

/// @file AgentMetricsCollector.h
/// @brief Declares the SteerLib::AgentMetricsCollector class.
///
/// @todo
///   - document this file properly
///   - make an AgentMetricsCollectorPrivate class
///

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include "Globals.h"
#include "griddatabase/GridDatabase2D.h"
#include "recfileio/RecFileIO.h"
#include "benchmarking/MetricsData.h"
#include "interfaces/AgentInterface.h"

#define ANGULAR_PRIORITY 1.0f

#define E_S 2.23f
#define E_W 1.26f
#define MASS 1.0f 
// #define COLLISION_EPSILON 0.05f // I think this would be best define as 1% of radius
#define COLLISION_EPSILON 0.001f // I think this would be best define as 1% of radius


#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {
    
	/**
	 * @brief Collects metrics for a single agent during a simulation.
	 *
	 * This class updates metrics of a single agent as the agent updates in the simulation.
	 * To use this class, simply instantiate the class with an initialized agent (these will be used as the initial conditions
	 * for this metrics collector), and then call update() exactly once for every simulation step after the agent was updated.
	 *
	 * Anyone interested in scrutinizing the benchmark process will want to look at the update()
	 * function here as well as the implementation of individual benchmark techniques.
	 *
	 */
	class STEERLIB_API AgentMetricsCollector
	{
	public:
		/// During initialization, the metrics collector is permanently associated with a particular agent.
		AgentMetricsCollector( SteerLib::AgentInterface * agent );
		/// Resets all metrics, and uses the latest status of the agent to form new initial conditions.
		void reset();
		/// Should be called exactly once every simulation step, after the agent has been updated in that step.
		void update(SteerLib::GridDatabase2D * gridDB, SteerLib::AgentInterface * updatedAgent, float currentTimeStamp, float timePassedSinceLastFrame);

	    /// @name query information for metrics
		//@{
		/// Returns all the metrics in their current form.
		AgentMetrics * getCurrentMetrics() { return &_metrics; }
		/// Returns information about all current collisions.
		std::map<uintptr_t, SteerLib::CollisionInfo> * getCurrentCollisions() { return &_currentCollidingObjects; }
		/// Returns the number of total unique collisions, both past and present.
	    size_t getNumTotalCollisions() { return _pastCollisions.size() + _currentCollidingObjects.size(); }
		/// Returns the number of unique past collisions (i.e. ones that are no longer still in a collision state) that are greater than both specified thresholds.
	    unsigned int getNumThresholdedCollisions(float penetrationThreshold, float timeDurationThreshold); // implemented in .cpp
		//@}
	    
	    /// dumps formatted console output with information of all current statistics.
	    void printFormattedCurrentStatistics(std::ostream & out);
		void printFormattedOverallStatistics(std::ostream & out);

	protected:
	    void _resetMetrics();
		void _updateCollisionStats(SteerLib::GridDatabase2D * gridDB, SteerLib::AgentInterface * updatedAgent, float currentTimeStamp);
	    void _checkAndUpdateOneCollision(uintptr_t collisionKey, float penetration, float currentTimeStamp);
	    void _updateAgentInformation(SteerLib::AgentInterface * updatedAgent);

	    unsigned int _numFramesMeasured;

		SteerLib::AgentInterface * _agentBeingAnalyzed;

	    Util::Point _currentPosition, _previousPosition;
	    Util::Vector _currentDirection, _previousDirection;
	    bool _enabled;
	    float _radius;
	    
	    // data collected during analysis - metrics and collision history
	    AgentMetrics _metrics;
	    
	    // window statistics:
	    // these arrays can be accessed like normal arrays, but they also have functionality to implement a sliding window.
	    windowArray<Util::Point> _positionWindow; // stores the agent position at each frame
	    //windowArray<int> _collisionWindow; // stores the number of collisions each frame
	    windowArray<float> _turnWindow; // stores the amount of turning each frame
	    windowArray<float> _distanceWindow; // stores the distance traveled each frame
	    windowArray<float> _changeInSpeedWindow; // stores the change in speed at each frame
	    windowArray<Util::Vector> _velocityWindow; // stores the velocity vector at each frame
	    windowArray<float> _accelerationWindow; // stores the *magnitude* only of change in velocity (not instantaneous acceleration) at each frame.
	    windowArray<Util::Vector> _instantaneousAccelerationWindow; // stores the *magnitude* only of change in velocity (not instantaneous acceleration) at each frame.

		// collision history
		std::map<uintptr_t, SteerLib::CollisionInfo> _currentCollidingObjects; // a list of agents and obstacles that this agent is colliding with.  hopefully won't ever be too large.
	    std::vector<CollisionInfo> _pastCollisions;
	};


} // end namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
    
