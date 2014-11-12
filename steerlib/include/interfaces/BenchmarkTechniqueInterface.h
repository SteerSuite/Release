//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_BENCHMARK_TECHNIQUE_INTERFACE_H__
#define __STEERLIB_BENCHMARK_TECHNIQUE_INTERFACE_H__

/// @file BenchmarkTechniqueInterface.h
/// @brief Declares the SteerLib::BenchmarkTechniqueInterface virtual interface.

#include "Globals.h"
#include "benchmarking/SimulationMetricsCollector.h"
#include "interfaces/ModuleInterface.h" // For OptionsDictionary

namespace SteerLib {

	/**
	 * @brief The basic interface for a benchmark technique
	 *
	 * Inherit this virtual interface to implement a benchmark technique.
	 * If possible, it would be a good idea to keep the benchmark technique in a state where the scores and details
	 * can be returned after any number of frames, even if the simulation did not finish.
	 *
	 */
	class STEERLIB_API BenchmarkTechniqueInterface
	{
	public:
		virtual ~BenchmarkTechniqueInterface() { }
		/// Initializes the technique using the rec file; some benchmark techniques may need to load the associated test case as well.
		virtual void init() = 0;

		virtual void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) = 0;
		/// Updates the benchmark technique given the updated metrics and timing information; not all benchmark techniques need to implement anything here.
		virtual void update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt) = 0;
		// Added to support functionality for Entropy Metric.
		virtual void update(SteerLib::EngineInterface * engineInterface, float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber) = 0 ;

		void setEngineInterface(SteerLib::EngineInterface * engineInfo) { this->_engineInfo = engineInfo; }
		SteerLib::EngineInterface * getEngineInterface() { return this->_engineInfo; }

		/// reset the benchmark technique 
		virtual void reset () = 0;

		virtual void getScores ( std::vector<float> & scores ) = 0;
		virtual void getAgentScores ( unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics,std::vector<float> & scores ) = 0; 

		/// Returns the benchmark score across all agents, given all the metrics of the test case.
		virtual float getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics) = 0;
		/// Returns the benchmark score for a specific agent, given all metrics; most techniques will only need to look at a particular agent's metrics.
		virtual float getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics) = 0;
		/// Prints details about how the benchmark technique computed its total score.
		virtual void printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out) = 0;
		/// Prints details about how the benchmark technique computed the score for an agent.
		virtual void printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out) = 0;
		/// Prints CSV-formatted details for the total score, if applicable to the techniqe.
		virtual void printTotalScoreCSV(SimulationMetricsCollector * simulationMetrics, std::ostream & out) { throw Util::GenericException("printTotalScoreCSV not implemented for this class"); }
		/// Prints CSV-formatted details for the score of an agent, if applicable to the techniqe.
		virtual void printAgentScoreCSV(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out)  { throw Util::GenericException("printAgentScoreCSV not implemented for this class"); }

	private:
		SteerLib::EngineInterface * _engineInfo;
	};

}

#endif
