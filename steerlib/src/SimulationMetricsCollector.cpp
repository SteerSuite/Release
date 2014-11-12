//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SimulationMetricsCollector.cpp
/// @brief implements the SteerLib::SimulationMetricsCollector class

#include "benchmarking/SimulationMetricsCollector.h"

using namespace std;
using namespace SteerLib;
using namespace Util;


SimulationMetricsCollector::SimulationMetricsCollector( const std::vector<SteerLib::AgentInterface*> & agents )
{
	// allocate and organize the agent metrics collectors
	_agentCollectors.clear();
	for (unsigned int i=0; i<agents.size(); i++) {
		AgentMetricsCollector * collector = new AgentMetricsCollector( agents[i] );
		_agentCollectors.push_back(collector);
	}
	
	_resetEnvironmentMetrics();
}



SimulationMetricsCollector::~SimulationMetricsCollector()
{
	// std::cout << "The simulation metrics are being updated" << std::endl;
	for (unsigned int i=0; i<_agentCollectors.size(); i++) {
		if (_agentCollectors[i] != NULL) delete _agentCollectors[i];
	}
}


void SimulationMetricsCollector::reset()
{
	for (unsigned int i=0; i<_agentCollectors.size(); i++) {
		_agentCollectors[i]->reset();
	}
	
	_resetEnvironmentMetrics();
}


void SimulationMetricsCollector::_resetEnvironmentMetrics()
{
	// nothing to do here yet,
	// when we have environment metrics we should reset those.
}


void SimulationMetricsCollector::_updateAgentMetrics(SteerLib::GridDatabase2D * gridDB, const std::vector<SteerLib::AgentInterface*> & updatedAgents, float currentTimeStamp, float timePassedSinceLastFrame)
{
	for (unsigned int i=0; i < getNumAgents(); i++) {
		/// @todo do we need this enabled() check here?  It may even be undesirable to keep it here.
		// std::cout << "Updating agent " << i << " metrics" << std::endl;
		if (updatedAgents[i]->enabled()) _agentCollectors[i]->update(gridDB, updatedAgents[i], currentTimeStamp, timePassedSinceLastFrame);
	}
}


void SimulationMetricsCollector::_updateEnvironmentMetrics(GridDatabase2D * gridDB, float currentTimeStamp, float timePassedSinceLastFrame)
{
	// no environment metrics implemented yet
	// TODO : (Glen) This needs to be implemented now.
}



void SimulationMetricsCollector::update(SteerLib::GridDatabase2D * gridDB, const std::vector<SteerLib::AgentInterface*> & updatedAgents, float currentTimeStamp, float timePassedSinceLastFrame)
{
	_updateAgentMetrics(gridDB, updatedAgents, currentTimeStamp, timePassedSinceLastFrame);
	_updateEnvironmentMetrics(gridDB, currentTimeStamp, timePassedSinceLastFrame);
}


void SimulationMetricsCollector::printCurrentMetrics(unsigned int agentIndex, std::ostream & out)
{
	out << "------ Agent " << agentIndex << " ------\n";
	_agentCollectors[agentIndex]->printFormattedOverallStatistics(out);
	_agentCollectors[agentIndex]->printFormattedCurrentStatistics(out);
}
