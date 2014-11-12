//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file CompositeTechnique02.cpp
/// @brief Implements SteerLib::CompositeBenchmarkTechnique02, an extension of SteerLib::CompositeTechnique01 that tries to capture "second-order" effort efficiency.

#include "benchmarking/CompositeTechnique02.h"

using namespace SteerLib;

void CompositeBenchmarkTechnique02::init() {
	/// @todo add feature to set options for benchmark techniques...
	_benchmarkScoreComputed = false;
	_alpha = 50.0f;
	_beta = 1.0f;
	_gamma = 1.0f;
	_delta = 1.0f;
}

float CompositeBenchmarkTechnique02::getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) { 
	return _computeTotalBenchmarkScore(simulationMetrics);
}

float CompositeBenchmarkTechnique02::getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics) { 
	AgentMetricsCollector * agentMetrics = simulationMetrics->getAgentCollector(agentIndex);
	return _alpha * (float)agentMetrics->getNumThresholdedCollisions(0.0f, 0.0f) +
		_beta * agentMetrics->getCurrentMetrics()->totalTimeEnabled +
		_gamma * agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies +
		_delta * agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousAcceleration;
}

void CompositeBenchmarkTechnique02::printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out) {

	_computeTotalBenchmarkScore(simulationMetrics);



	out << "\n";
	out << "             total number of agents: " << (unsigned int)_numAgents << "\n";
	out << "avg. number of collisions per agent: " << _numCollisionsOfAllAgents / _numAgents << "\n";
	out << "    average time spent by one agent: " << _totalTimeOfAllAgents / _numAgents << "\n";
	out << "  average energy spent by one agent: " << _totalEnergyOfAllAgents / _numAgents << "\n";
	out << " sum of instantaneous accelerations: " << _totalInstantaneousAcceleration /  _numAgents << "\n";
	out << "(alpha, beta, gamma, delta) weights: " << "(" << _alpha << "," << _beta << "," << _gamma << "," << _delta << ")\n";
	out << "                       weighted sum: " << _alpha << "*" << _numCollisionsOfAllAgents / _numAgents << " + " << _beta << "*" << _totalTimeOfAllAgents / _numAgents << " + " << _gamma << "*" << _totalEnergyOfAllAgents / _numAgents << " + " << _delta << "*" << _totalInstantaneousAcceleration / _numAgents << " = " << _totalBenchmarkScore << "\n";
	out << "                        final score: " << _totalBenchmarkScore << "\n";

}

void CompositeBenchmarkTechnique02::printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out) {

	float score = getAgentBenchmarkScore(agentIndex,simulationMetrics);
	AgentMetricsCollector * agentMetrics = simulationMetrics->getAgentCollector(agentIndex);
	float numCollisions = (float)agentMetrics->getNumThresholdedCollisions(0.0f, 0.0f);

	out << "\n";
	out << " number of collisions for the agent: " << numCollisions << "\n";
	out << "      total time spent by the agent: " << agentMetrics->getCurrentMetrics()->totalTimeEnabled << "\n";
	out << "    total energy spent by the agent: " << agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies << "\n";
	out << " sum of instantaneous accelerations: " << agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousAcceleration << "\n";
	out << "(alpha, beta, gamma, delta) weights: " << "(" << _alpha << "," << _beta << "," << _gamma << "," << _delta << ")\n";
	out << "                       weighted sum: " << _alpha << "*" << numCollisions << " + " << _beta << "*" << agentMetrics->getCurrentMetrics()->totalTimeEnabled << " + " << _gamma << "*" << agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies << " + " << _delta << "*" << agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousAcceleration << " = " << score << "\n";
	out << "                        final score: " << score << "\n";

}


float CompositeBenchmarkTechnique02::_computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) {


	if (_benchmarkScoreComputed) return _totalBenchmarkScore;

	_numCollisionsOfAllAgents = 0.0f;
	_totalTimeOfAllAgents = 0.0f;
	_totalEnergyOfAllAgents = 0.0f;

	_numAgents = (float)simulationMetrics->getNumAgents();

	if (_numAgents == 0.0f)
		return 0.0f; 


	// compute the "total" values over all agents
	AgentMetricsCollector * agentMetrics;
	for (unsigned int i = 0; i < simulationMetrics->getNumAgents(); i++) {
		agentMetrics = simulationMetrics->getAgentCollector(i);
		_numCollisionsOfAllAgents += (float)agentMetrics->getNumThresholdedCollisions(0.0f, 0.0f);
		_totalTimeOfAllAgents += agentMetrics->getCurrentMetrics()->totalTimeEnabled;
		_totalEnergyOfAllAgents += agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies;
		_totalInstantaneousAcceleration += agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousAcceleration;
	}

	_totalBenchmarkScore = 
		_alpha * (_numCollisionsOfAllAgents / _numAgents) 
		+ _beta * (_totalTimeOfAllAgents / _numAgents) 
		+ _gamma * (_totalEnergyOfAllAgents / _numAgents)
		+ _delta * (_totalInstantaneousAcceleration / _numAgents);

	_benchmarkScoreComputed = true;
	return _totalBenchmarkScore;
}
