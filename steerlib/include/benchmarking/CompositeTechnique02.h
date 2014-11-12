//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_COMPOSITE_TECHNIQUE_02_H__
#define __STEERLIB_COMPOSITE_TECHNIQUE_02_H__

/// @file CompositeTechnique02.h
/// @brief Declares SteerLib::CompositeBenchmarkTechnique02, an extension of SteerLib::CompositeTechnique01 that tries to capture "second-order" effort efficiency.

#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "util/GenericException.h"

namespace SteerLib {
	
	class STEERLIB_API CompositeBenchmarkTechnique02 : public SteerLib::BenchmarkTechniqueInterface
	{
	public:
		void init();
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
		{
			init();
		}
		void update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt)
		{
			// std::cout << "benchmark 02" << std::endl;
		}
		void update(SteerLib::EngineInterface * engineInterface, float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber) {}
		void reset () 
		{ 
			_benchmarkScoreComputed = false; 
			_numCollisionsOfAllAgents = 0;
			_numAgents = 0.0f;
			_totalTimeOfAllAgents = 0.0f;
			_totalEnergyOfAllAgents = 0.0f;
			_totalInstantaneousAcceleration = 0.0f;
			_totalBenchmarkScore = 0.0f;
		}

		void getScores ( std::vector<float> & scores )
		{
			scores.push_back(_numCollisionsOfAllAgents);
			scores.push_back(_totalTimeOfAllAgents);
			scores.push_back(_totalEnergyOfAllAgents);
			scores.push_back(_totalInstantaneousAcceleration);
		}

		void getAgentScores ( unsigned int agentIndex,  SimulationMetricsCollector * simulationMetrics,std::vector<float> & scores ) 
		{
			AgentMetricsCollector * agentMetrics = simulationMetrics->getAgentCollector(agentIndex);
			
			//float numCollisions= (float)agentMetrics->getNumThresholdedCollisions(0.1f, 0.1f);
			//scores.push_back(numCollisions);
			// std::cout << "agent metrics " << agentMetrics << " for agent " << agentIndex << std::endl;
			scores.push_back((float)agentMetrics->getCurrentMetrics()->numUniqueCollisions);
			

			scores.push_back(agentMetrics->getCurrentMetrics()->totalTimeEnabled);
			scores.push_back(agentMetrics->getCurrentMetrics()->totalAcceleration);
			scores.push_back(agentMetrics->getCurrentMetrics()->totalDistanceTraveled);
			scores.push_back(agentMetrics->getCurrentMetrics()->totalChangeInSpeed);
			scores.push_back(agentMetrics->getCurrentMetrics()->totalDegreesTurned);

			scores.push_back(agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousAcceleration);
			scores.push_back(agentMetrics->getCurrentMetrics()->sumTotalOfInstantaneousKineticEnergies);

			scores.push_back(agentMetrics->getCurrentMetrics()->averageKineticEnergy);
		}




		float getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);
		float getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics);
		void printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out);
		void printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out);

	protected:
		float _computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);

		bool _benchmarkScoreComputed;
		float _alpha, _beta, _gamma, _delta;
		float _numCollisionsOfAllAgents, _totalTimeOfAllAgents, _totalEnergyOfAllAgents, _totalInstantaneousAcceleration;
		float _numAgents;
		float _totalBenchmarkScore;
	};


} // end namespace SteerLib

#endif
