//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __PPR_AI_MODULE_H__
#define __PPR_AI_MODULE_H__

/// @file PPRAIModule.h
/// @brief Declares the PPRAIModule class.


#include "SteerLib.h"
#include "PPRAgent.h"
#include "Logger.h"

// forward declaration
class GridEnvironment;
class PerformanceProfiler;

namespace PPRGlobals {

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


	extern SteerLib::EngineInterface * gEngine;
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
	extern float ped_max_speed;
	extern float ped_typical_speed ;
	extern float ped_max_force  ;
	extern float ped_max_speed_factor  ;
	extern float ped_faster_speed_factor ;
	extern float ped_slightly_faster_speed_factor;
	extern float ped_typical_speed_factor   ;
	extern float ped_slightly_slower_speed_factor;
	extern float ped_slower_speed_factor;
	extern float ped_cornering_turn_rate;
	extern float ped_adjustment_turn_rate;
	extern float ped_faster_avoidance_turn_rate;
	extern float ped_typical_avoidance_turn_rate;
	extern float ped_braking_rate ;
	extern float ped_comfort_zone   ;
	extern float ped_query_radius  ;
	extern float ped_similar_direction_dot_product_threshold;
	extern float ped_same_direction_dot_product_threshold;
	extern float ped_oncoming_prediction_threshold;
	extern float ped_oncoming_reaction_threshold;
	extern float ped_wrong_direction_dot_product_threshold;
	extern float ped_threat_distance_threshold;
	extern float ped_threat_min_time_threshold;
	extern float ped_threat_max_time_threshold;
	extern float ped_predictive_anticipation_factor ;
	extern float ped_reactive_anticipation_factor;
	extern float ped_crowd_influence_factor;
	extern float ped_facing_static_object_threshold;
	extern float ped_ordinary_steering_strength;
	extern float ped_oncoming_threat_avoidance_strength;
	extern float ped_cross_threat_avoidance_strength;
	extern float ped_max_turning_rate;
	extern int ped_feeling_crowded_threshold;
	extern float ped_scoot_rate ;
	extern float ped_reached_target_distance_threshold ;
	extern float ped_dynamic_collision_padding;
	extern int ped_furthest_local_target_distance;
	extern int ped_next_waypoint_distance;
	extern int ped_max_num_waypoints;


	extern PhaseProfilers * gPhaseProfilers;
}

class PPRAIModule : public SteerLib::ModuleInterface
{
public:
	
	// removing test case dependency for the purpose of automatic scenario generation
	//std::string getDependencies() { return "testCasePlayer"; }
	std::string getDependencies() { return ""; }

	std::string getConflicts() { return ""; }
	std::string getData() { return ""; }
	LogData * getLogData() { return new LogData(); }
	void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
	void finish();
	SteerLib::AgentInterface * createAgent();
	void destroyAgent( SteerLib::AgentInterface * agent ) { if (agent) delete agent;  agent = NULL; }

	void initializeSimulation();
	void cleanupSimulation();

private:
	bool logToFie;
	std::string logFilename;
	Logger * _pprLogger;

};


#endif
