//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_COMPOSITE_TECHNIQUE_01_H__
#define __STEERLIB_COMPOSITE_TECHNIQUE_01_H__

/// @file CompositeTechnique01.h
/// @brief Declares SteerLib::CompositeBenchmarkTechnique01, the first benchmark scoring technique used in the original CAVW 2008 paper.
///
/// @todo
///   - add feature to set options for benchmark techniques in general...

#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "util/GenericException.h"

namespace SteerLib {
	
	class STEERLIB_API CompositeBenchmarkTechnique01 : public SteerLib::BenchmarkTechniqueInterface
	{
	public:
		void init();
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {init();}
		void update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt) { }
		void update(SteerLib::EngineInterface * engineInterface, float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber) {}
		void reset () 
		{ 
			_benchmarkScoreComputed = false; 
			_numCollisionsOfAllAgents = 0;
			_numAgents = 0.0f;
			_totalTimeOfAllAgents = 0.0f;
			_totalEnergyOfAllAgents = 0.0f;
			_totalBenchmarkScore = 0.0f;
		}

		virtual void getScores ( std::vector<float> & scores )
		{
			scores.push_back(_numCollisionsOfAllAgents);
			scores.push_back(_totalTimeOfAllAgents);
			scores.push_back(_totalEnergyOfAllAgents);
			//scores.push_back(_totalInstantaneousAcceleration);
		}
		void getAgentScores ( unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics,std::vector<float> & scores ) 
		{
			std::cerr << "need to implement \n";
		}


		float getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);
		float getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics);
		void printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out);
		void printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out);

	protected:
		float _computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);

		bool _benchmarkScoreComputed;
		float _alpha, _beta, _gamma;
		float _numCollisionsOfAllAgents, _totalTimeOfAllAgents, _totalEnergyOfAllAgents;
		float _numAgents;
		float _totalBenchmarkScore;
	};


} // end namespace SteerLib

#endif
