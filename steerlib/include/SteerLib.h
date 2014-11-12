//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_H__
#define __STEERLIB_H__

/// @file SteerLib.h
/// @brief This is the primary .h file to include in your own code; it indirectly includes all functionality in SteerLib and Util.
///
/// SteerLib.h is the only include file that most users will need to include in their code.
///
/// <h3> Notes </h3>
///  - Because %SteerLib indirectly includes other .h files, you should add 
///    SteerLib's include/ directory your project's include path.
///  - Also remember to link the compiled %SteerLib library into your project.
///

#include "Globals.h"
#include "Util.h"

#include "recfileio/RecFileIO.h"

#include "testcaseio/TestCaseIO.h"
#include "testcaseio/AgentInitialConditions.h"
#include "testcaseio/ObstacleInitialConditions.h"

#include "griddatabase/GridCell.h"
#include "griddatabase/GridDatabase2D.h"
#include "griddatabase/GridDatabasePlanningDomain.h"

#include "obstacles/BoxObstacle.h"
#include "obstacles/OrientedBoxObstacle.h"
#include "obstacles/CircleObstacle.h"

#include "planning/BestFirstSearchPlanner.h"

#include "simulation/Camera.h"
#include "simulation/Clock.h"
#include "simulation/SimulationOptions.h"
#include "simulation/SimulationEngine.h"
#include "simulation/SteeringCommand.h"

#include "benchmarking/AgentMetricsCollector.h"
#include "benchmarking/MetricsData.h"
#include "benchmarking/SimulationMetricsCollector.h"
#include "benchmarking/BenchmarkEngine.h"
#include "benchmarking/CompositeTechnique01.h"
#include "benchmarking/CompositeTechnique02.h"
#include "benchmarking/EnergyEfficiencyTechnique.h"
#include "benchmarking/AccelerationEfficiencyTechnique.h"

#include "interfaces/AgentInterface.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "interfaces/EngineControllerInterface.h"
#include "interfaces/EngineInterface.h"
#include "interfaces/ObstacleInterface.h"
#include "interfaces/ModuleInterface.h"
#include "interfaces/SpatialDatabaseItem.h"

#include "modules/DummyAIModule.h"
#include "modules/MetricsCollectorModule.h"
#include "modules/RecFilePlayerModule.h"
#include "modules/SimulationRecorderModule.h"
#include "modules/SteerBenchModule.h"
#include "modules/SteerBugModule.h"
#include "modules/TestCasePlayerModule.h"

#endif
