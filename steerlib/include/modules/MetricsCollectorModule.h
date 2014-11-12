//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_METRICS_COLLECTOR_MODULE_H__
#define __STEERLIB_METRICS_COLLECTOR_MODULE_H__

/// @file MetricsCollectorModule.h
/// @brief Declares the MetricsCollectorModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"
#include "benchmarking/SimulationMetricsCollector.h"

namespace SteerLib {

	class MetricsCollectorModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return ""; }
		std::string getConflicts() { return ""; }
		std::string getData() { return ""; }
		LogData * getLogData() { return new LogData(); }

		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {
			_engine = engineInfo;
			_simulationMetrics = NULL;
		}

		void finish() {
			delete _simulationMetrics;
			_simulationMetrics = NULL;
		}

		void initializeSimulationMetrics () 
		{

			// if a previous metrics collection was ongoing (i.e. from a previous simulation),
			// then clean it up here.   We don't clean it up earlier, so that it is still accessible to the GUI.
			delete _simulationMetrics;

			// allocate and setup metrics collection
			_simulationMetrics = new SteerLib::SimulationMetricsCollector( _engine->getAgents() );

		}


		void preprocessSimulation() {

			initializeSimulationMetrics ();
		}

		void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber) {
			_simulationMetrics->update( _engine->getSpatialDatabase(), _engine->getAgents(), timeStamp, dt);
		}

		inline SteerLib::SimulationMetricsCollector * getSimulationMetrics() { return _simulationMetrics; }

	protected:
		SteerLib::EngineInterface * _engine;
		SteerLib::SimulationMetricsCollector * _simulationMetrics;
	};

} // end namespace SteerLib

#endif
