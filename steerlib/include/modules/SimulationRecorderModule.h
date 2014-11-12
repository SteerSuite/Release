//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_SIMULATION_RECORDER_MODULE__
#define __STEERLIB_SIMULATION_RECORDER_MODULE__

/// @file SimulationRecorderModule.h
/// @brief Declares the SimulationRecorderModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"

namespace SteerLib {

	class SimulationRecorderModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return ""; }
		std::string getConflicts() { return ""; }
		std::string getData() { return ""; }
		LogData * getLogData() { return new LogData(); }

		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );

		void initializeRecorder ( std::string & recFileName )
		{
			_recFilename = recFileName;
			preprocessSimulation ();
		}

		void finishRecording ()
		{
			postprocessSimulation ();
			_initialized = false; 
		}

		void finish() { }
		void preprocessSimulation();
		void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
		void postprocessSimulation();

	protected:
		SteerLib::EngineInterface * _engine;
		SteerLib::RecFileWriter * _simulationWriter;
		std::string _recFilename;

		bool _initialized;

	};

} // end namespace SteerLib

#endif
