//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_STEER_BENCH_MODULE__
#define __STEERLIB_STEER_BENCH_MODULE__

/// @file SteerBenchModule.h
/// @brief Declares the SteerBenchModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"
#include "benchmarking/BenchmarkEngine.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "modules/MetricsCollectorModule.h"

namespace SteerLib {

	class SteerBenchModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return "metricsCollector"; }

		std::string getConflicts() { return ""; }
		std::string getData() { return ""; }
		LogData * getLogData() { return new LogData(); }

		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {

			_engine = engineInfo;
			_metricsCollectorModule = NULL;
			_techniqueName = "composite02";

			// iterate over all the options
			SteerLib::OptionDictionary::const_iterator optionIter;
			for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
				if ((*optionIter).first == "technique") {
					_techniqueName = (*optionIter).second;
				}
				else {
					throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to steerBench module.");
				}
			}

			_benchmarkTechnique = SteerLib::createBenchmarkTechnique(_techniqueName);
			_benchmarkTechnique->init(options, engineInfo);
			// _benchmarkTechnique->setEngineInterface(_engine);
		}

		void finish() { }

		void preprocessSimulation() {
			_metricsCollectorModule = dynamic_cast<MetricsCollectorModule*>( _engine->getModule("metricsCollector"));
		}

		void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
		{
			_benchmarkTechnique->update( _engine, timeStamp, dt, frameNumber);
		}

		void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber) {
			assert(_metricsCollectorModule != NULL);
			// std::cout << "found the business? " << _engine->getAgents().size() << std::endl;
			// _benchmarkTechnique->update(_metricsCollectorModule->getSimulationMetrics(), timeStamp, dt);
			// _benchmarkTechnique->update( _engine, timeStamp, dt);
		}

		void reportBenchmarkResults () 
		{
			std::cout << "Benchmark score using the \"" << _techniqueName << "\" benchmark technique:  ";
			std::cout << _benchmarkTechnique->getTotalBenchmarkScore(_metricsCollectorModule->getSimulationMetrics()) << "\n";
			_benchmarkTechnique->printTotalScoreDetails(_metricsCollectorModule->getSimulationMetrics(), std::cout);
			//_metricsCollectorModule = NULL;

		}

		// this is just for doing some initial logging experiments
		float getTotalBenchmarkScore ()
		{
			return _benchmarkTechnique->getTotalBenchmarkScore(_metricsCollectorModule->getSimulationMetrics());
		}

		void getAgentScore(unsigned int index, std::vector<float> &score)
		{
			_benchmarkTechnique->getAgentScores(index,_metricsCollectorModule->getSimulationMetrics(),score);
		}

		SteerLib::BenchmarkTechniqueInterface * getBenchmarkTechnique () { return _benchmarkTechnique; }

		void resetBenchmarkModule () 
		{
			// resetting the benchmark technique 
			_benchmarkTechnique->reset();

			// reinitializing the metrics collector module 
			// Note: This must be called after the scenario has been loaded. (i.e. agents have been created by the engine) 
			_metricsCollectorModule->initializeSimulationMetrics ();
		}

		void postprocessSimulation() {
		}

		void setBenchmarkTechnique ( const std::string & techniqueName ) 
		{
			this->_techniqueName = techniqueName;

			if ( _benchmarkTechnique )
				delete _benchmarkTechnique;

			_benchmarkTechnique = SteerLib::createBenchmarkTechnique(_techniqueName);
			_benchmarkTechnique->init();

		}

		void setBenchmarkTechnique ( const std::string & techniqueName,  const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
		{
			this->_techniqueName = techniqueName;

			if ( _benchmarkTechnique )
				delete _benchmarkTechnique;

			_benchmarkTechnique = SteerLib::createBenchmarkTechnique(_techniqueName);
			_benchmarkTechnique->init( options, engineInfo);

		}


	protected:
		SteerLib::EngineInterface * _engine;
		MetricsCollectorModule * _metricsCollectorModule;

		std::string _techniqueName;
		SteerLib::BenchmarkTechniqueInterface * _benchmarkTechnique;
	};

} // end namespace SteerLib

#endif
