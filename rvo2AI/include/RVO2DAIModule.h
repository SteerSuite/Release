//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __SIMPLE_AI_MODULE__
#define __SIMPLE_AI_MODULE__

/// @file SimpleAIModule.h
/// @brief Declares the SimpleAIModule plugin.


#include "SteerLib.h"
#include "KdTree.h"
#include <vector>
#include "RVO2D_Parameters.h"
#include "Logger.h"


// globally accessible to the AI plugin
extern SteerLib::EngineInterface * gEngine;
// extern SteerLib::GridDatabase2D * gSpatialDatabase;



namespace RVO2DGlobals {

	struct PhaseProfilers {
		Util::PerformanceProfiler aiProfiler;
		Util::PerformanceProfiler drawProfiler;
		Util::PerformanceProfiler longTermPhaseProfiler;
		Util::PerformanceProfiler midTermPhaseProfiler;
		Util::PerformanceProfiler shortTermPhaseProfiler;
		Util::PerformanceProfiler perceptivePhaseProfiler;
		Util::PerformanceProfiler predictivePhaseProfiler;
		Util::PerformanceProfiler reactivePhaseProfiler;
		Util::PerformanceProfiler steeringPhaseProfiler;
	};


	extern SteerLib::EngineInterface * gEngineInfo;
	extern SteerLib::GridDatabase2D * gSpatialDatabase;
	extern unsigned int gLongTermPlanningPhaseInterval;
	extern unsigned int gMidTermPlanningPhaseInterval;
	extern unsigned int gShortTermPlanningPhaseInterval;
	extern unsigned int gPredictivePhaseInterval;
	extern unsigned int gReactivePhaseInterval;
	extern unsigned int gPerceptivePhaseInterval;
	extern bool gUseDynamicPhaseScheduling;
	extern bool gShowStats;
	extern bool gShowAllStats;


	// Adding a bunch of parameters so they can be changed via input
	extern float rvo_neighbor_distance;
	extern float rvo_time_horizon;
	extern float rvo_max_speed;
	extern float rvo_preferred_speed;
	extern float rvo_time_horizon_obstacles;
	extern int rvo_max_neighbors;
	extern int next_waypoint_distance;


	extern PhaseProfilers * gPhaseProfilers;
}


class RVO2DAIModule : public SteerLib::ModuleInterface
{
public:
	
	void cleanupSimulation();
	void preprocessSimulation();
	void initializeSimulation();
	//std::string getDependencies() { return "testCasePlayer"; }
	std::string getDependencies() { return ""; }
	
	std::string getConflicts() { return ""; }
	std::string getData() { return ""; }
	LogData * getLogData() { return new LogData(); }
	void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
	void finish();
	SteerLib::AgentInterface * createAgent();
	void destroyAgent( SteerLib::AgentInterface * agent );

	void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	std::vector<SteerLib::AgentInterface * > agents_;

	KdTree * kdTree_;
protected:
	std::string logFilename; // = "pprAI.log";
	bool logStats; // = false;
	Logger * _rvoLogger;
};

#endif
