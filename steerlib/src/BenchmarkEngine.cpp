//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file BenchmarkEngine.cpp
/// @brief Implements the SteerLib::BenchmarkEngine class.

#include "benchmarking/BenchmarkEngine.h"


#include "benchmarking/CompositeTechnique01.h"
#include "benchmarking/CompositeTechnique02.h"
#include "benchmarking/EnergyEfficiencyTechnique.h"
#include "benchmarking/AccelerationEfficiencyTechnique.h"

#include "obstacles/BoxObstacle.h"

using namespace std;
using namespace SteerLib;
using namespace Util;



// TODO: this is not used anymore??

BenchmarkEngine::BenchmarkEngine(const std::string & recordingFilename, BenchmarkTechniqueInterface * benchmarkTechnique)
{
	_currentFrameNumber = 0;
	_done = false;

	// allocate the rec file reader and open the rec file
	_recFileReader = new RecFileReader(recordingFilename);

	// allocate the spatial database
	// @todo allocate the database according to the world bounds of the rec file instead of hard-coded
	_spatialDatabase = new GridDatabase2D(Point(-100.0f, 0.0f, -100.0f), 200.0f, 200.0f, 200, 200, 7, true);

	// allocate agents
	_agents.clear();
	for (unsigned int i=0; i < _recFileReader->getNumAgents(); i++) {
		BenchmarkAgent * newAgent = new BenchmarkAgent();

		/// @todo
		///   The next version of the RecFileReader should also return an AgentGoalInfo struct, and this indirection should be unnecessary.
		AgentGoalInfo newGoal;
		newGoal.targetLocation = _recFileReader->getAgentGoalAtFrame(i,0);

		newAgent->setEnabled(_recFileReader->isAgentEnabledAtFrame(i,0));
		newAgent->setPosition(_recFileReader->getAgentLocationAtFrame(i,0));
		newAgent->setForward(_recFileReader->getAgentOrientationAtFrame(i,0));
		newAgent->setRadius(_recFileReader->getAgentRadiusAtFrame(i,0));
		newAgent->setCurrentGoal(newGoal);
		_spatialDatabase->addObject(newAgent, newAgent->getBounds());
		_agents.push_back(newAgent);
	}

	// allocate obstacles
	_obstacles.clear();
	for (unsigned int i=0; i < _recFileReader->getNumObstacles(); i++) {
		BoxObstacle * newObstacle = new BoxObstacle(_recFileReader->getObstacleBoundsAtFrame(i,0));
		_spatialDatabase->addObject(newObstacle, newObstacle->getBounds());
		_obstacles.push_back(newObstacle);
	}

	// allocate and initialize the metrics collector
	_simulationMetricsCollector = new SimulationMetricsCollector( _agents);

	_benchmarkTechnique = benchmarkTechnique;
	_benchmarkTechnique->init();

}

bool BenchmarkEngine::isValidTestCaseSimulation(const std::string & testCaseDirectory)
{
	throw GenericException("validating rec files against test cases not implemented yet.");
}

void BenchmarkEngine::stepOneFrame()
{
	assert(!_done);

	// 0. increment the frame counter
	_currentFrameNumber++;
	if (_currentFrameNumber >= _recFileReader->getNumFrames()) {
		_done = true;
		return;
	}

	// 1. update all AgentInterface dummies and the spatial database based on the rec file
	for (unsigned int i=0; i < _recFileReader->getNumAgents(); i++) {
		BenchmarkAgent * agent = dynamic_cast<BenchmarkAgent*>(_agents[i]);

		/// @todo
		///   The next version of the RecFileReader should also return an AgentGoalInfo struct, and this indirection should be unnecessary.
		AgentGoalInfo newGoal;
		newGoal.targetLocation = _recFileReader->getAgentGoalAtFrame(i,_currentFrameNumber);

		AxisAlignedBox oldBounds = agent->getBounds();
		agent->setEnabled(_recFileReader->isAgentEnabledAtFrame(i,_currentFrameNumber));
		agent->setPosition(_recFileReader->getAgentLocationAtFrame(i,_currentFrameNumber));
		agent->setForward(_recFileReader->getAgentOrientationAtFrame(i,_currentFrameNumber));
		agent->setRadius(_recFileReader->getAgentRadiusAtFrame(i,_currentFrameNumber));
		agent->setCurrentGoal(newGoal);
		_spatialDatabase->updateObject(agent, oldBounds, agent->getBounds());
	}

	std::cout << "Is this ever called" << std::endl;
	// 2. give this list of agents to the simulation metrics collector along with the time/dt info
	_simulationMetricsCollector->update(_spatialDatabase, _agents, _recFileReader->getTimeStampForFrame(_currentFrameNumber), _recFileReader->getElapsedTimeBetweenFrames(_currentFrameNumber-1,_currentFrameNumber));

	// 3. give the benchmark technique a chance to update with the new metrics
	_benchmarkTechnique->update(_simulationMetricsCollector, _recFileReader->getTimeStampForFrame(_currentFrameNumber), _recFileReader->getElapsedTimeBetweenFrames(_currentFrameNumber-1,_currentFrameNumber));
}


float BenchmarkEngine::getTotalBenchmarkScore()
{
	//   ask the benchmarkTechnique for the total benchmark score across all agents, given a simulation metrics collector
	return _benchmarkTechnique->getTotalBenchmarkScore(_simulationMetricsCollector);
}

float BenchmarkEngine::getAgentBenchmarkScore(unsigned int agentIndex)
{
	//   ask the benchmarkTechnique for the agent benchmark score given a simulation metrics collector
	return _benchmarkTechnique->getAgentBenchmarkScore(agentIndex, _simulationMetricsCollector);
}


void BenchmarkEngine::printTotalScoreDetails(std::ostream & out)
{
	// ask the benchmark technique to do this
	_benchmarkTechnique->printTotalScoreDetails(_simulationMetricsCollector, out);
}

void BenchmarkEngine::printAgentScoreDetails(unsigned int agentIndex, std::ostream & out)
{
	// ask the benchmark technique to do this
	_benchmarkTechnique->printAgentScoreDetails(agentIndex, _simulationMetricsCollector, out);
}

void BenchmarkEngine::printCurrentMetrics(unsigned int agentIndex, std::ostream & out)
{
	// ask the agent metrics collector to do this.
	_simulationMetricsCollector->printCurrentMetrics(agentIndex, out);
}

