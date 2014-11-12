//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file CompositeTechnique01.cpp
/// @brief Implements SteerLib::CompositeBenchmarkTechnique01, the first benchmark scoring technique used in the original CAVW 2008 paper.

#include "benchmarking/CompositeTechnique01.h"

using namespace SteerLib;

void CompositeBenchmarkTechnique01::init() {
	_benchmarkScoreComputed = false;
	_alpha = 50.0f;
	_beta = 1.0f;
	_gamma = 1.0f;
}

float CompositeBenchmarkTechnique01::getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) { 
	return _computeTotalBenchmarkScore(simulationMetrics);
}

float CompositeBenchmarkTechnique01::getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics) { 
	AgentMetricsCollector * agentMetrics = simulationMetrics->getAgentCollector(agentIndex);
	return _alpha * (float)agentMetrics->getNumThresholdedCollisions(0.0f, 0.0f) +
		_beta * agentMetrics->getCurrentMetrics()->totalTimeEnabled +
		_gamma * agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies;
}

void CompositeBenchmarkTechnique01::printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out) {

	_computeTotalBenchmarkScore(simulationMetrics);


	out << _numCollisionsOfAllAgents / _numAgents << " " << _totalTimeOfAllAgents / _numAgents << " " <<  _totalEnergyOfAllAgents / _numAgents << "\n";

	/*
	out << "\n";
	out << "             total number of agents: " << (unsigned int)_numAgents << "\n";
	out << "avg. number of collisions per agent: " << _numCollisionsOfAllAgents / _numAgents << "\n";
	out << "    average time spent by one agent: " << _totalTimeOfAllAgents / _numAgents << "\n";
	out << "  average energy spent by one agent: " << _totalEnergyOfAllAgents / _numAgents << "\n";
	out << "       (alpha, beta, gamma) weights: (" << _alpha << "," << _beta << "," << _gamma << ")\n";
	out << "                       weighted sum: " << _alpha << "*" << _numCollisionsOfAllAgents / _numAgents << " + " << _beta << "*" << _totalTimeOfAllAgents / _numAgents << " + " << _gamma << "*" << _totalEnergyOfAllAgents / _numAgents << " = " << _totalBenchmarkScore << "\n";
	out << "                        final score: " << _totalBenchmarkScore << "\n";

	*/


}

void CompositeBenchmarkTechnique01::printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out) {

	float score = getAgentBenchmarkScore(agentIndex,simulationMetrics);
	AgentMetricsCollector * agentMetrics = simulationMetrics->getAgentCollector(agentIndex);
	float numCollisions = (float)agentMetrics->getNumThresholdedCollisions(0.0f, 0.0f);

	out << "\n";
	out << "number of collisions for the agent: " << numCollisions << "\n";
	out << "     total time spent by the agent: " << agentMetrics->getCurrentMetrics()->totalTimeEnabled << "\n";
	out << "   total energy spent by the agent: " << agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies << "\n";
	out << "      (alpha, beta, gamma) weights: (" << _alpha << "," << _beta << "," << _gamma << ")\n";
	out << "                      weighted sum: " << _alpha << "*" << numCollisions << " + " << _beta << "*" << agentMetrics->getCurrentMetrics()->totalTimeEnabled << " + " << _gamma << "*" << agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies << " = " << score << "\n";
	out << "                       final score: " << score << "\n";

}


float CompositeBenchmarkTechnique01::_computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) {
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
	}

	_totalBenchmarkScore = 
		_alpha * (_numCollisionsOfAllAgents / _numAgents) 
		+ _beta * (_totalTimeOfAllAgents / _numAgents) 
		+ _gamma * (_totalEnergyOfAllAgents / _numAgents);

	_benchmarkScoreComputed = true;
	return _totalBenchmarkScore;
}

