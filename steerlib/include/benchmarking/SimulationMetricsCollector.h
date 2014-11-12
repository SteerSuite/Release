//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_SIMULATION_METRICS_COLLECTOR_H__
#define __STEERLIB_SIMULATION_METRICS_COLLECTOR_H__


/// @file SimulationMetricsCollector.h
/// @brief Declares the SteerLib::SimulationMetricsCollector class

#include <vector>
#include "Globals.h"
#include "benchmarking/AgentMetricsCollector.h"
#include "griddatabase/GridDatabase2D.h"
#include "recfileio/RecFileIO.h"
#include "interfaces/AgentInterface.h"

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
	 * @brief Functionality for collecting all metrics of a simulation, including an AgentMetricsCollector for each agent.
	 *
	 * @todo
	 *    - add more documentation for this class
	 */
    class STEERLIB_API SimulationMetricsCollector
	{
	public:
	    SimulationMetricsCollector( const std::vector<SteerLib::AgentInterface*> & agents);
	    ~SimulationMetricsCollector();
	    
		void reset();
		void update(SteerLib::GridDatabase2D * gridDB, const std::vector<SteerLib::AgentInterface*> & updatedAgents, float currentTimeStamp, float timePassedSinceLastFrame);
	    
	    AgentMetricsCollector * getAgentCollector(unsigned int agentIndex) { return _agentCollectors[agentIndex]; }
	    size_t getNumAgents() { return _agentCollectors.size(); }

		void printCurrentMetrics(unsigned int agentIndex, std::ostream & out);

	protected:
	    void _resetEnvironmentMetrics();
	    void _updateAgentMetrics(SteerLib::GridDatabase2D * gridDB, const std::vector<SteerLib::AgentInterface*> & updatedAgents, float currentTimeStamp, float timePassedSinceLastFrame);
	    void _updateEnvironmentMetrics(SteerLib::GridDatabase2D * gridDB, float currentTimeStamp, float timePassedSinceLastFrame);
	    
	    std::vector<AgentMetricsCollector*> _agentCollectors;
	    EnvironmentMetrics _environmentMetrics;
    
	};
    
} // end namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
    
