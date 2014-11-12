//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_BENCHMARK_ENGINE_H__
#define __STEERLIB_BENCHMARK_ENGINE_H__

/// @file BenchmarkEngine.h
/// @brief Declares the SterLib::BenchmarkEngine class.

#include <string>
#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "benchmarking/AgentMetricsCollector.h"
#include "benchmarking/SimulationMetricsCollector.h"
#include "benchmarking/BenchmarkEnginePrivate.h"

#include "benchmarking/CompositeTechnique01.h"
#include "benchmarking/CompositeTechnique02.h"
#include "benchmarking/EnergyEfficiencyTechnique.h"
#include "benchmarking/AccelerationEfficiencyTechnique.h"
#include "benchmarking/CompositeTechniquePLE.h"
#include "benchmarking/CompositeTechniqueGraph.h"

namespace SteerLib {

	/**
	 * @brief Returns an instance of one of the implementations of the BenchmarkTechniqueInterface interface.
	 */
	static inline SteerLib::BenchmarkTechniqueInterface * createBenchmarkTechnique(const std::string & techniqueName)
	{
		if (techniqueName == "composite01") {
			return new CompositeBenchmarkTechnique01();
		}
		else if (techniqueName == "composite02") {
			return new CompositeBenchmarkTechnique02();
		}
		else if (techniqueName == "energyEfficiency") {
			return new EnergyEfficiencyBenchmarkTechnique();
		}
		else if (techniqueName == "accelerationEfficiency") {
			return new AccelerationEfficiencyBenchmarkTechnique();
		}
		else if (techniqueName == "compositePLE") {
			return new CompositeBenchmarkTechniquePLE();
		}
		else if (techniqueName == "compositeGraph")
		{
			return new CompositeBenchmarkTechniqueGraph();
		}
		else
		{
			throw Util::GenericException("Cannot create benchmark technique, unknown benchmark technique \"" + techniqueName + "\".");
		}
	}

	/**
	 * @brief Destroys an instance of the BenchmarkTechniqueInterface that was allocated in SteerLib.
	 */
	static inline void destroyBenchmarkTechnique( SteerLib::BenchmarkTechniqueInterface * technique )
	{
		if (technique != NULL) delete technique;
	}


	/**
	 * @brief Functionality for benchmarking a rec file offline.
	 *
	 * This class runs an offline benchmark process for a rec file.
	 *
	 */
	class STEERLIB_API BenchmarkEngine : public SteerLib::BenchmarkEnginePrivate
	{
	public:
		/// Initializes the engine
		BenchmarkEngine(const std::string & recordingFilename, SteerLib::BenchmarkTechniqueInterface * benchmarkTechnique);
		/// Validates the rec file against a test case, returns true if the rec file initial conditions match the test case initial conditions, false otherwise.
		bool isValidTestCaseSimulation(const std::string & testCaseDirectory);
		/// Updates metrics and benchmark scoring for the next frame of the rec file.
		void stepOneFrame();

		/// Returns true if the engine is done.
		bool isDone() { return _done; }
		/// Returns the current frame number.
		unsigned int currentFrameNumber() { return _currentFrameNumber; }
		/// Returns the number of agents being benchmarked.
		size_t numAgents() { return _agents.size(); }
		/// Returns a reference to the simulation's metrics collector.
		SteerLib::SimulationMetricsCollector * getSimulationMetricsCollector() { return _simulationMetricsCollector; }
		/// Returns a reference to a specific agent's metrics collector.
		SteerLib::AgentMetricsCollector * getAgentMetricsCollector(unsigned int agentIndex) { return _simulationMetricsCollector->getAgentCollector(agentIndex); }

		/// Returns the benchmark score across all agents
		float getTotalBenchmarkScore();
		/// Returns the benchmark score for a specific agent
		float getAgentBenchmarkScore(unsigned int agentIndex);

		/// Prints details about how the total benchmark score was computed
		void printTotalScoreDetails(std::ostream & out);
		/// Prints details about how an agent's benchmark score was computed
		void printAgentScoreDetails(unsigned int agentIndex, std::ostream & out);
		/// Prints the current metrics for an agent; this can become <b>very verbose</b> if used for multiple agents or multiple frames.
		void printCurrentMetrics(unsigned int agentIndex, std::ostream & out);
	};

} // end namespace SteerLib

#endif
