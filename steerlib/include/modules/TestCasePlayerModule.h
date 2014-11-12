//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_TEST_CASE_PLAYER_MODULE__
#define __STEERLIB_TEST_CASE_PLAYER_MODULE__

/// @file TestCasePlayerModule.h
/// @brief Declares the TestCasePlayerModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"
#include "obstacles/BoxObstacle.h"

namespace SteerLib {

	class TestCasePlayerModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return ""; }
		std::string getConflicts() { return "recFilePlayer"; }
		std::string getData() { return ""; }
		LogData * getLogData() { return new LogData(); }
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
		void finish();
		void initializeSimulation();
		void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
		void cleanupSimulation();

	protected:
		SteerLib::EngineInterface * _engine;
		std::string _testCaseFilename;
		std::string _aiModuleName;
		std::string _aiModuleSearchPath;
		SteerLib::ModuleInterface * _aiModule;

		std::vector<SteerLib::ObstacleInterface *> _obstacles;

	};

} // end namespace SteerLib

#endif
