//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_ACCELERATION_EFFICIENCY_TECHNIQUE_H__
#define __STEERLIB_ACCELERATION_EFFICIENCY_TECHNIQUE_H__

/// @file AccelerationEfficiencyTechnique.h
/// @brief Declares the SteerLib::AccelerationEfficiencyBenchmarkTechnique class.

#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "util/GenericException.h"

namespace SteerLib {
	
	class STEERLIB_API AccelerationEfficiencyBenchmarkTechnique : public SteerLib::BenchmarkTechniqueInterface
	{
		void init() {
			_benchmarkScoreComputed = false;
		}

		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {init();}

		void update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt) { 
			// nothing to do here
		}
		void update(SteerLib::EngineInterface * engineInterface, float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber) {}

		void reset () 
		{ 
			_benchmarkScoreComputed = false; 
			_totalBenchmarkScore = 0.0f;
		}

		float getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) { 
			if (!_benchmarkScoreComputed) _computeTotalBenchmarkScore(simulationMetrics);
			return _totalBenchmarkScore;
		}

		float getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics) { 
			AgentMetrics * metrics = simulationMetrics->getAgentCollector(agentIndex)->getCurrentMetrics();
			return (metrics->totalAcceleration / metrics->totalTimeEnabled);
		}

		void printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out) {
			if (!_benchmarkScoreComputed) _computeTotalBenchmarkScore(simulationMetrics);
			out << " average instantaneous acceleration over all agents: " << _totalBenchmarkScore << "\n";
		}

		void printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out) { 
			AgentMetrics * metrics = simulationMetrics->getAgentCollector(agentIndex)->getCurrentMetrics();
			out << " total (integral) acceleration over simulation: " << metrics->totalAcceleration << "\n";
			out << " total simulation time: " << metrics->totalTimeEnabled << "\n";
			out << " average instantaneous acceleration = total acceleration / simulation time = " << metrics->totalAcceleration / metrics->totalTimeEnabled << "\n";
		}

		virtual void getScores ( std::vector<float> & scores )
		{
			std::cerr << "NOT IMPLEMENTED \n";
		}
		void getAgentScores ( unsigned int agentIndex,  SimulationMetricsCollector * simulationMetrics,std::vector<float> & scores ) 
		{
			std::cerr << "need to implement \n";
		}

	protected:
		void _computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) {
			float accumulator = 0.0f;
			for (unsigned int i=0; i < simulationMetrics->getNumAgents(); i++) {
				accumulator += getAgentBenchmarkScore(i, simulationMetrics);
			}
			_totalBenchmarkScore = accumulator / ((float)simulationMetrics->getNumAgents());
			_benchmarkScoreComputed = true;
		}

		float _totalBenchmarkScore;
		bool _benchmarkScoreComputed;
	};

} // end namespace SteerLib

#endif
