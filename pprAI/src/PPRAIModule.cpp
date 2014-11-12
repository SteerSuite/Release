//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "SteerLib.h"
#include "SimulationPlugin.h"
#include "PPRAIModule.h"
#include "PPRAgent.h"



#define LONG_TERM_PLANNING_INTERVAL    10000
#define MID_TERM_PLANNING_INTERVAL     10000
#define SHORT_TERM_PLANNING_INTERVAL   1
#define PERCEPTIVE_PHASE_INTERVAL      1
#define PREDICTIVE_PHASE_INTERVAL      1
#define REACTIVE_PHASE_INTERVAL        1

#define PERCENT 100.0f
#define TO_MILLISECONDS 1000.0f


using namespace Util;
using namespace SteerLib;

#include "LogObject.h"
#include "LogManager.h"

// todo: make these static?
namespace PPRGlobals {
	SteerLib::EngineInterface * gEngine;
	SteerLib::GridDatabase2D * gSpatialDatabase;
	unsigned int gLongTermPlanningPhaseInterval;
	unsigned int gMidTermPlanningPhaseInterval;
	unsigned int gShortTermPlanningPhaseInterval;
	unsigned int gPredictivePhaseInterval;
	unsigned int gReactivePhaseInterval;
	unsigned int gPerceptivePhaseInterval;

	bool gUseDynamicPhaseScheduling;
	bool gShowStats;
	bool logStats;
	bool gShowAllStats;
	
	// Adding a bunch of parameters so they can be changed via input
	float ped_max_speed;
	float ped_typical_speed ;
	float ped_max_force  ;
	float ped_max_speed_factor  ;
	float ped_faster_speed_factor ;
	float ped_slightly_faster_speed_factor;
	float ped_typical_speed_factor   ;
	float ped_slightly_slower_speed_factor;
	float ped_slower_speed_factor;
	float ped_cornering_turn_rate;
	float ped_adjustment_turn_rate;
	float ped_faster_avoidance_turn_rate;
	float ped_typical_avoidance_turn_rate;
	float ped_braking_rate ;
	float ped_comfort_zone   ;
	float ped_query_radius  ;
	float ped_similar_direction_dot_product_threshold;
	float ped_same_direction_dot_product_threshold;
	float ped_oncoming_prediction_threshold;
	float ped_oncoming_reaction_threshold;
	float ped_wrong_direction_dot_product_threshold;
	float ped_threat_distance_threshold;
	float ped_threat_min_time_threshold;
	float ped_threat_max_time_threshold;
	float ped_predictive_anticipation_factor ;
	float ped_reactive_anticipation_factor;
	float ped_crowd_influence_factor;
	float ped_facing_static_object_threshold;
	float ped_ordinary_steering_strength;
	float ped_oncoming_threat_avoidance_strength;
	float ped_cross_threat_avoidance_strength;
	float ped_max_turning_rate;
	int ped_feeling_crowded_threshold;
	float ped_scoot_rate ;
	float ped_reached_target_distance_threshold ;
	float ped_dynamic_collision_padding;
	int ped_furthest_local_target_distance;
	int ped_next_waypoint_distance;
	int ped_max_num_waypoints;

	PhaseProfilers * gPhaseProfilers;
}

using namespace PPRGlobals;

//
// 
//
void PPRAIModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	gSpatialDatabase = engineInfo->getSpatialDatabase();

	gEngine = engineInfo;


	gLongTermPlanningPhaseInterval = LONG_TERM_PLANNING_INTERVAL;
	gMidTermPlanningPhaseInterval = MID_TERM_PLANNING_INTERVAL;
	gShortTermPlanningPhaseInterval = SHORT_TERM_PLANNING_INTERVAL;
	gPerceptivePhaseInterval = PERCEPTIVE_PHASE_INTERVAL;
	gPredictivePhaseInterval = PREDICTIVE_PHASE_INTERVAL;
	gReactivePhaseInterval = REACTIVE_PHASE_INTERVAL;
	gUseDynamicPhaseScheduling = false;
	gShowStats = false;
	logStats = false;
	gShowAllStats = false;
	logFilename = "pprAI.log";


	ped_max_speed = PED_MAX_SPEED;
	ped_typical_speed  = PED_TYPICAL_SPEED ;
	ped_max_force   = PED_MAX_FORCE  ;
	ped_max_speed_factor   = PED_MAX_SPEED_FACTOR  ;
	ped_faster_speed_factor  = PED_FASTER_SPEED_FACTOR ;
	ped_slightly_faster_speed_factor = PED_SLIGHTLY_FASTER_SPEED_FACTOR;
	ped_typical_speed_factor    = PED_TYPICAL_SPEED_FACTOR   ;
	ped_slightly_slower_speed_factor = PED_SLIGHTLY_SLOWER_SPEED_FACTOR;
	ped_slower_speed_factor = PED_SLOWER_SPEED_FACTOR;
	ped_cornering_turn_rate = PED_CORNERING_TURN_RATE;
	ped_adjustment_turn_rate = PED_ADJUSTMENT_TURN_RATE;
	ped_faster_avoidance_turn_rate = PED_FASTER_AVOIDANCE_TURN_RATE;
	ped_typical_avoidance_turn_rate = PED_TYPICAL_AVOIDANCE_TURN_RATE;
	ped_braking_rate  = PED_BRAKING_RATE ;
	ped_comfort_zone    = PED_COMFORT_ZONE   ;
	ped_query_radius   = PED_QUERY_RADIUS  ;
	ped_similar_direction_dot_product_threshold = PED_SIMILAR_DIRECTION_DOT_PRODUCT_THRESHOLD;
	ped_same_direction_dot_product_threshold = PED_SAME_DIRECTION_DOT_PRODUCT_THRESHOLD;
	ped_oncoming_prediction_threshold = PED_ONCOMING_PREDICTION_THRESHOLD;
	ped_oncoming_reaction_threshold = PED_ONCOMING_REACTION_THRESHOLD;
	ped_wrong_direction_dot_product_threshold = PED_WRONG_DIRECTION_DOT_PRODUCT_THRESHOLD;
	ped_threat_distance_threshold = PED_THREAT_DISTANCE_THRESHOLD;
	ped_threat_min_time_threshold = PED_THREAT_MIN_TIME_THRESHOLD;
	ped_threat_max_time_threshold = PED_THREAT_MAX_TIME_THRESHOLD;
	ped_predictive_anticipation_factor  = PED_PREDICTIVE_ANTICIPATION_FACTOR ;
	ped_reactive_anticipation_factor = PED_REACTIVE_ANTICIPATION_FACTOR;
	ped_crowd_influence_factor = PED_CROWD_INFLUENCE_FACTOR;
	ped_facing_static_object_threshold = PED_FACING_STATIC_OBJECT_THRESHOLD;
	ped_ordinary_steering_strength = PED_ORDINARY_STEERING_STRENGTH;
	ped_oncoming_threat_avoidance_strength = PED_ONCOMING_THREAT_AVOIDANCE_STRENGTH;
	ped_cross_threat_avoidance_strength = PED_CROSS_THREAT_AVOIDANCE_STRENGTH;
	ped_max_turning_rate = PED_MAX_TURNING_RATE;
	ped_feeling_crowded_threshold = PED_FEELING_CROWDED_THRESHOLD;
	ped_scoot_rate  = PED_SCOOT_RATE ;
	ped_reached_target_distance_threshold  = PED_REACHED_TARGET_DISTANCE_THRESHOLD ;
	ped_dynamic_collision_padding = PED_DYNAMIC_COLLISION_PADDING;
	ped_furthest_local_target_distance = PED_FURTHEST_LOCAL_TARGET_DISTANCE;
	ped_next_waypoint_distance = PED_NEXT_WAYPOINT_DISTANCE;
	ped_max_num_waypoints = PED_MAX_NUM_WAYPOINTS;

	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		std::stringstream value((*optionIter).second);
		if ((*optionIter).first == "longplan") {
			value >> gLongTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "midplan")
		{
			value >> gMidTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "shortplan")
		{
			value >> gShortTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "perceptive")
		{
			value >> gPerceptivePhaseInterval;
		}
		else if ((*optionIter).first == "predictive")
		{
			value >> gPredictivePhaseInterval;
		}
		else if ((*optionIter).first == "reactive")
		{
			value >> gReactivePhaseInterval;
		}
		else if ((*optionIter).first == "dynamic")
		{
			gUseDynamicPhaseScheduling = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "ped_max_speed")
		{
			value >> ped_max_speed;
		}
		else if ((*optionIter).first == "ped_typical_speed")
		{
			value >> ped_typical_speed;
		}
		else if ((*optionIter).first == "ped_max_force")
		{
			std::cout << "Setting max_force to: " << value.str();
			value >> ped_max_force;
		}

		else if ((*optionIter).first == "ped_max_speed_factor")
		{
			value >> ped_max_speed_factor;
		}
		else if ((*optionIter).first == "ped_faster_speed_factor")
		{
			value >> ped_faster_speed_factor;
		}
		else if ((*optionIter).first == "ped_slightly_faster_speed_factor")
		{
			value >> ped_slightly_faster_speed_factor;
		}
		else if ((*optionIter).first == "ped_typical_speed_factor")
		{
			value >> ped_typical_speed_factor;
		}
		else if ((*optionIter).first == "ped_slightly_slower_speed_factor")
		{
			value >> ped_slightly_slower_speed_factor;
		}
		else if ((*optionIter).first == "ped_slower_speed_factor")
		{
			value >> ped_slower_speed_factor;
		}
		else if ((*optionIter).first == "ped_cornering_turn_rate")
		{
			value >> ped_cornering_turn_rate;
		}
		else if ((*optionIter).first == "ped_adjustment_turn_rate")
		{
			value >> ped_adjustment_turn_rate;
		}
		else if ((*optionIter).first == "ped_faster_avoidance_turn_rate")
		{
			value >> ped_faster_avoidance_turn_rate;
		}
		else if ((*optionIter).first == "ped_typical_avoidance_turn_rate")
		{
			value >> ped_typical_avoidance_turn_rate;
		}
		else if ((*optionIter).first == "ped_braking_rate")
		{
			value >> ped_braking_rate;
		}
		else if ((*optionIter).first == "ped_comfort_zone")
		{
			value >> ped_comfort_zone;
		}
		else if ((*optionIter).first == "ped_query_radius")
		{
			value >> ped_query_radius;
		}
		else if ((*optionIter).first == "ped_similar_direction_dot_product_threshold")
		{
			value >> ped_similar_direction_dot_product_threshold;
		}
		else if ((*optionIter).first == "ped_same_direction_dot_product_threshold")
		{
			value >> ped_same_direction_dot_product_threshold;
		}
		else if ((*optionIter).first == "ped_oncoming_prediction_threshold")
		{
			value >> ped_oncoming_prediction_threshold;
		}
		else if ((*optionIter).first == "ped_oncoming_reaction_threshold")
		{
			value >> ped_oncoming_reaction_threshold;
		}
		else if ((*optionIter).first == "ped_wrong_direction_dot_product_threshold")
		{
			value >> ped_wrong_direction_dot_product_threshold;
		}
		else if ((*optionIter).first == "ped_threat_distance_threshold")
		{
			value >> ped_threat_distance_threshold;
		}
		else if ((*optionIter).first == "ped_threat_min_time_threshold")
		{
			value >> ped_threat_min_time_threshold;
		}
		else if ((*optionIter).first == "ped_threat_max_time_threshold")
		{
			value >> ped_threat_max_time_threshold;
		}
		else if ((*optionIter).first == "ped_predictive_anticipation_factor")
		{
			value >> ped_predictive_anticipation_factor;
		}
		else if ((*optionIter).first == "ped_reactive_anticipation_factor")
		{
			value >> ped_reactive_anticipation_factor;
		}
		else if ((*optionIter).first == "ped_crowd_influence_factor")
		{
			value >> ped_crowd_influence_factor;
		}
		else if ((*optionIter).first == "ped_facing_static_object_threshold")
		{
			value >> ped_facing_static_object_threshold;
		}
		else if ((*optionIter).first == "ped_ordinary_steering_strength")
		{
			value >> ped_ordinary_steering_strength;
		}
		else if ((*optionIter).first == "ped_oncoming_threat_avoidance_strength")
		{
			value >> ped_oncoming_threat_avoidance_strength;
		}
		else if ((*optionIter).first == "ped_cross_threat_avoidance_strength")
		{
			value >> ped_cross_threat_avoidance_strength;
		}
		else if ((*optionIter).first == "ped_max_turning_rate")
		{
			value >> ped_max_turning_rate;
		}
		else if ((*optionIter).first == "ped_feeling_crowded_threshold")
		{
			value >> ped_feeling_crowded_threshold;
		}
		else if ((*optionIter).first == "ped_scoot_rate")
		{
			value >> ped_scoot_rate;
		}
		else if ((*optionIter).first == "ped_reached_target_distance_threshold")
		{
			value >> ped_reached_target_distance_threshold;
		}
		else if ((*optionIter).first == "ped_dynamic_collision_padding")
		{
			value >> ped_dynamic_collision_padding;
		}
		else if ((*optionIter).first == "ped_furthest_local_target_distance")
		{
			value >> ped_furthest_local_target_distance;
		}
		else if ((*optionIter).first == "ped_next_waypoint_distance")
		{
			value >> ped_next_waypoint_distance;
		}
		else if ((*optionIter).first == "ped_max_num_waypoints")
		{
			value >> ped_max_num_waypoints;
		}
		else if ((*optionIter).first == "ailogFileName")
		{
			logFilename = value.str();
			logStats = true;
		}
		else if ((*optionIter).first == "stats")
		{
			gShowStats = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "allstats")
		{
			gShowAllStats = Util::getBoolFromString(value.str());
		}
		else
		{
			// throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to PPR AI module.");
		}
	}


	if (gShowStats)
	{
		std::cout << std::endl;
		if (!gUseDynamicPhaseScheduling) {
			std::cout << " PHASE INTERVALS (in frames):\n";
			std::cout << "   longplan: " << gLongTermPlanningPhaseInterval << "\n";
			std::cout << "    midplan: " << gMidTermPlanningPhaseInterval << "\n";
			std::cout << "  shortplan: " << gShortTermPlanningPhaseInterval << "\n";
			std::cout << " perceptive: " << gPerceptivePhaseInterval << "\n";
			std::cout << " predictive: " << gPredictivePhaseInterval << "\n";
			std::cout << "   reactive: " << gReactivePhaseInterval << "\n";
		}
		else {
			std::cout << " PHASE INTERVALS (in frames):\n";
			std::cout << "   longplan: " << "on demand" << "\n";
			std::cout << "    midplan: " << "on demand" << "\n";
			std::cout << "  shortplan: " << "dynamic" << "\n";
			std::cout << " perceptive: " << "dynamic" << "\n";
			std::cout << " predictive: " << "dynamic" << "\n";
			std::cout << "   reactive: " << "dynamic" << "\n";
		}
		std::cout << std::endl;
	}

	//
	// print a warning if we are using annotations with too many agents.
	//
#ifdef USE_ANNOTATIONS
	if (gEngine->getAgents().size() > 30) {
		std::cerr << "WARNING: using annotations with a large number of agents will use a lot of memory and will be much slower." << std::endl;
	}
#endif

	if ( logStats )
	{
	_pprLogger = LogManager::getInstance()->createLogger(logFilename,LoggerType::BASIC_WRITE);

	_pprLogger->addDataField("longplan",DataType::LongLong );
	_pprLogger->addDataField("midplan",DataType::LongLong );
	_pprLogger->addDataField("shortplan",DataType::LongLong );
	_pprLogger->addDataField("perceptive",DataType::LongLong );
	_pprLogger->addDataField("predictive",DataType::LongLong );
	_pprLogger->addDataField("reactive",DataType::LongLong );


	_pprLogger->addDataField("long_term_planning_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("long_term_planning_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("long_term_planning_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("long_term_planning_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("long_term_planning_fastest_execution", DataType::Float);
	_pprLogger->addDataField("long_term_planning_slowest_execution", DataType::Float);
	_pprLogger->addDataField("long_term_planning_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("long_term_planning_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("long_term_planning_tick_frequency", DataType::Float);

	_pprLogger->addDataField("mid_term_planning_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("mid_term_planning_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("mid_term_planning_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("mid_term_planning_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("mid_term_planning_fastest_execution", DataType::Float);
	_pprLogger->addDataField("mid_term_planning_slowest_execution", DataType::Float);
	_pprLogger->addDataField("mid_term_planning_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("mid_term_planning_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("mid_term_planning_tick_frequency", DataType::Float);

	_pprLogger->addDataField("short_term_planning_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("short_term_planning_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("short_term_planning_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("short_term_planning_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("short_term_planning_fastest_execution", DataType::Float);
	_pprLogger->addDataField("short_term_planning_slowest_execution", DataType::Float);
	_pprLogger->addDataField("short_term_planning_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("short_term_planning_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("short_term_planning_tick_frequency", DataType::Float);

	_pprLogger->addDataField("perceptive_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("perceptive_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("perceptive_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("perceptive_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("perceptive_fastest_execution", DataType::Float);
	_pprLogger->addDataField("perceptive_slowest_execution", DataType::Float);
	_pprLogger->addDataField("perceptive_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("perceptive_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("perceptive_tick_frequency", DataType::Float);

	_pprLogger->addDataField("predictive_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("predictive_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("predictive_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("predictive_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("predictive_fastest_execution", DataType::Float);
	_pprLogger->addDataField("predictive_slowest_execution", DataType::Float);
	_pprLogger->addDataField("predictive_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("predictive_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("predictive_tick_frequency", DataType::Float);

	_pprLogger->addDataField("reactive_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("reactive_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("reactive_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("reactive_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("reactive_fastest_execution", DataType::Float);
	_pprLogger->addDataField("reactive_slowest_execution", DataType::Float);
	_pprLogger->addDataField("reactive_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("reactive_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("reactive_tick_frequency", DataType::Float);

	_pprLogger->addDataField("steering_number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("steering_total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("steering_shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("steering_longest_execution",DataType::LongLong );
	_pprLogger->addDataField("steering_fastest_execution", DataType::Float);
	_pprLogger->addDataField("steering_slowest_execution", DataType::Float);
	_pprLogger->addDataField("steering_average_time_per_call", DataType::Float);
	_pprLogger->addDataField("steering_total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("steering_tick_frequency", DataType::Float);

	_pprLogger->addDataField("number_of_times_executed",DataType::LongLong );
	_pprLogger->addDataField("total_ticks_accumulated",DataType::LongLong );
	_pprLogger->addDataField("shortest_execution",DataType::LongLong );
	_pprLogger->addDataField("longest_execution",DataType::LongLong );
	_pprLogger->addDataField("fastest_execution", DataType::Float);
	_pprLogger->addDataField("slowest_execution", DataType::Float);
	_pprLogger->addDataField("average_time_per_call", DataType::Float);
	_pprLogger->addDataField("total_time_of_all_calls", DataType::Float);
	_pprLogger->addDataField("tick_frequency", DataType::Float);

	_pprLogger->addDataField("percent_mid_term", DataType::Float);
	_pprLogger->addDataField("percent_short_term", DataType::Float);
	_pprLogger->addDataField("percent_perceptive", DataType::Float);
	_pprLogger->addDataField("percent_predictive", DataType::Float);
	_pprLogger->addDataField("percent_reactive", DataType::Float);
	_pprLogger->addDataField("percent_steering", DataType::Float);
	_pprLogger->addDataField("average_per_agent", DataType::Float);
	_pprLogger->addDataField("average_per_agent_4hz", DataType::Float);
	_pprLogger->addDataField("average_per_agent_5hz", DataType::Float);

	 // TODO: 20 frames per second is a hard-coded assumption in the following calculations
	_pprLogger->addDataField("average_frequency_mid_term", DataType::Float);  // :   0.0542373 Hz (skipping 368.75 frames)
	_pprLogger->addDataField("average_frequency_short_term", DataType::Float);  // : 20 Hz (skipping 1 frames)
	_pprLogger->addDataField("average_frequency_perceptive", DataType::Float);  // : 20 Hz (skipping 1 frames)
	_pprLogger->addDataField("average_frequency_predictive", DataType::Float);  // : 0 Hz (skipping inf frames)
	_pprLogger->addDataField("average_frequency_reactive", DataType::Float);  // :   20 Hz (skipping 1 frames)
	_pprLogger->addDataField("average_frequency_steering", DataType::Float);  // :   20 Hz (skipping 1 frames)

	_pprLogger->addDataField("amortized_percent_mid_term", DataType::Float);
	_pprLogger->addDataField("amortized_percent_short_term", DataType::Float);
	_pprLogger->addDataField("amortized_percent_perceptive", DataType::Float);
	_pprLogger->addDataField("amortized_percent_predictive", DataType::Float);
	_pprLogger->addDataField("amortized_percent_reactive", DataType::Float);
	_pprLogger->addDataField("amortized_percent_steering", DataType::Float);
	_pprLogger->addDataField("average_per_agent_per_update", DataType::Float);


	// LETS TRY TO WRITE THE LABELS OF EACH FIELD
	std::stringstream labelStream;
	unsigned int i;
	for (i=0; i < _pprLogger->getNumberOfFields() - 1; i++)
		labelStream << _pprLogger->getFieldName(i) << " ";
	labelStream << _pprLogger->getFieldName(i);

	_pprLogger->writeData(labelStream.str());
	}

}


void PPRAIModule::initializeSimulation()
{
	//
	// initialize the performance profilers
	//
	gPhaseProfilers = new PhaseProfilers;
	gPhaseProfilers->aiProfiler.reset();
	gPhaseProfilers->longTermPhaseProfiler.reset();
	gPhaseProfilers->midTermPhaseProfiler.reset();
	gPhaseProfilers->shortTermPhaseProfiler.reset();
	gPhaseProfilers->perceptivePhaseProfiler.reset();
	gPhaseProfilers->predictivePhaseProfiler.reset();
	gPhaseProfilers->reactivePhaseProfiler.reset();
	gPhaseProfilers->steeringPhaseProfiler.reset();
	
}


//
// cleanupSimulation()
//
void PPRAIModule::cleanupSimulation()
{

	if ( logStats )
	{
		LogObject pprLogObject;
		pprLogObject.addLogData((long long) gLongTermPlanningPhaseInterval);
		pprLogObject.addLogData((long long) gMidTermPlanningPhaseInterval);
		pprLogObject.addLogData((long long) gShortTermPlanningPhaseInterval);
		pprLogObject.addLogData((long long) gPerceptivePhaseInterval);
		pprLogObject.addLogData((long long) gPredictivePhaseInterval);
		pprLogObject.addLogData((long long) gReactivePhaseInterval);
		if (gShowAllStats)
		{
			std::cout << "===================================================\n";
			std::cout << "PROFILE RESULTS  " << std::endl;
			std::cout << "===================================================\n";

			std::cout << "--- Long-term planning ---\n";
			std::cout << std::endl;
			gPhaseProfilers->longTermPhaseProfiler.displayStatistics(std::cout);
		}
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->longTermPhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- Mid-term planning ---\n";
				gPhaseProfilers->midTermPhaseProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- Short-term planning ---\n";
				gPhaseProfilers->shortTermPhaseProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- Perceptive phase ---\n";
				gPhaseProfilers->perceptivePhaseProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- Predictive phase ---\n";
				gPhaseProfilers->predictivePhaseProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- Reactive phase ---\n";
				gPhaseProfilers->reactivePhaseProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- Steering phase ---\n";
				gPhaseProfilers->steeringPhaseProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << "--- TOTAL AI ---\n";
				gPhaseProfilers->aiProfiler.displayStatistics(std::cout);
				std::cout << std::endl;
			}
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getNumTimesExecuted());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTicksAccumulated());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getMinTicks());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getMaxTicks());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getMinExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getMaxExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getAverageExecutionTimeMills());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTime());
			pprLogObject.addLogData(gPhaseProfilers->aiProfiler.getTickFrequency());

			if (gShowAllStats)
			{
				std::cout << std::endl;


				std::cout << "--- ESTIMATED REFERENCE (excluding long-term and space-time planning) ---\n";
				std::cout << "\n(NOTE: this is not rigorously valid to compare against amortized costs below,\n";
				std::cout << "         because it excludes space-time planning)\n\n";
			}
			float totalAgentTime =
				gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime() + 
				gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime() +
				gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime();
			float totalAgentTime_5Hz_amortized =   // 5 Hz skips every 4 frames, so scale by 0.25
				gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime() * 0.25f + 
				gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime() * 0.25f +
				gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime() * 0.25f +
				gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime() * 0.25f +
				gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime() +  // reactive and steering phases still execute 20 Hz.
				gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime();
			float totalAgentTime_4Hz_amortized =    // 4 Hz skips every 5 frames, so scale by 0.2
				gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime() * 0.2f + 
				gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime() * 0.2f +
				gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime() * 0.2f +
				gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime() * 0.2f +
				gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime() +  // reactive and steering phases still execute 20 Hz.
				gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime();

			if (gShowAllStats)
			{
				std::cout << " percent mid-term:   " << gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT<< "\n";
				std::cout << " percent short-term: " << gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT<< "\n";
				std::cout << " percent perceptive: " << gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT << "\n";
				std::cout << " percent predictive: " << gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT << "\n";
				std::cout << " percent reactive:   " << gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT << "\n";
				std::cout << " percent steering:   " << gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT << "\n";
				std::cout << "\n";
				std::cout << " Average per agent, no amortization: " << totalAgentTime * 1000.0 << " milliseconds\n";
				std::cout << " Average per agent, 5Hz (skip 4 frames): " << totalAgentTime_5Hz_amortized * 1000.0 << " milliseconds\n";
				std::cout << " Average per agent, 4Hz (skip 5 frames): " << totalAgentTime_4Hz_amortized * 1000.0 << " milliseconds\n";
				std::cout << "\n";
			}


			pprLogObject.addLogData(gPhaseProfilers->midTermPhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT);
			pprLogObject.addLogData(gPhaseProfilers->shortTermPhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT);
			pprLogObject.addLogData(gPhaseProfilers->perceptivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT);
			pprLogObject.addLogData(gPhaseProfilers->predictivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT);
			pprLogObject.addLogData(gPhaseProfilers->reactivePhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT);
			pprLogObject.addLogData(gPhaseProfilers->steeringPhaseProfiler.getAverageExecutionTime()/totalAgentTime * PERCENT);
			pprLogObject.addLogData(totalAgentTime * TO_MILLISECONDS);
			pprLogObject.addLogData(totalAgentTime_5Hz_amortized * TO_MILLISECONDS);
			pprLogObject.addLogData(totalAgentTime_4Hz_amortized * TO_MILLISECONDS);



		if (gShowStats || gShowAllStats)
		{
			std::cout << "--- PROFILE RESULTS (excluding long-term planning) ---\n\n";
		}
		float totalTimeForAllAgents =
			gPhaseProfilers->midTermPhaseProfiler.getTotalTime()+
			gPhaseProfilers->shortTermPhaseProfiler.getTotalTime() +
			gPhaseProfilers->perceptivePhaseProfiler.getTotalTime() +
			gPhaseProfilers->predictivePhaseProfiler.getTotalTime() +
			gPhaseProfilers->reactivePhaseProfiler.getTotalTime() +
			gPhaseProfilers->steeringPhaseProfiler.getTotalTime();

		// TODO: right now this is hacked, later on need to add an arg or access to the engine to get this value correctly:
		if (gShowStats || gShowAllStats)
		{
			std::cerr << " TODO: 20 frames per second is a hard-coded assumption in the following calculations\n";
		}
		float baseFrequency = 20.0f;
		float totalNumberOfFrames = (float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted();

		float average_frequency_mid_term = gPhaseProfilers->midTermPhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency; // << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->midTermPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		float average_frequency_short_term = gPhaseProfilers->shortTermPhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency; //  << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->shortTermPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		float average_frequency_perceptive = gPhaseProfilers->perceptivePhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency; // << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->perceptivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		float average_frequency_predictive = gPhaseProfilers->predictivePhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency; // << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->predictivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		float average_frequency_reactive = gPhaseProfilers->reactivePhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency; // << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->reactivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
		float average_frequency_steering = gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()/totalNumberOfFrames * baseFrequency; // << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()) << " frames)\n";

		if (gShowStats || gShowAllStats)
		{
			std::cout << "\n";

			std::cout << " average frequency mid-term:   " << average_frequency_mid_term << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->midTermPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
			std::cout << " average frequency short-term: " << average_frequency_short_term << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->shortTermPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
			std::cout << " average frequency perceptive: " << average_frequency_perceptive << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->perceptivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
			std::cout << " average frequency predictive: " << average_frequency_predictive << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->predictivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
			std::cout << " average frequency reactive:   " << average_frequency_reactive << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->reactivePhaseProfiler.getNumTimesExecuted()) << " frames)\n";
			std::cout << " average frequency steering:   " << average_frequency_steering << " Hz (skipping " << totalNumberOfFrames/((float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()) << " frames)\n";
			std::cout << "\n";
		}

		pprLogObject.addLogData(average_frequency_mid_term);
		pprLogObject.addLogData(average_frequency_short_term);
		pprLogObject.addLogData(average_frequency_perceptive);
		pprLogObject.addLogData(average_frequency_predictive);
		pprLogObject.addLogData(average_frequency_reactive);
		pprLogObject.addLogData(average_frequency_steering);


		float amortized_percent_mid_term = gPhaseProfilers->midTermPhaseProfiler.getTotalTime()/totalTimeForAllAgents * PERCENT;
		float amortized_percent_short_term = gPhaseProfilers->shortTermPhaseProfiler.getTotalTime()/totalTimeForAllAgents * PERCENT;
		float amortized_percent_perceptive = gPhaseProfilers->perceptivePhaseProfiler.getTotalTime()/totalTimeForAllAgents * PERCENT;
		float amortized_percent_predictive = gPhaseProfilers->predictivePhaseProfiler.getTotalTime()/totalTimeForAllAgents * PERCENT;
		float amortized_percent_reactive = gPhaseProfilers->reactivePhaseProfiler.getTotalTime()/totalTimeForAllAgents * PERCENT;
		float amortized_percent_steering = gPhaseProfilers->steeringPhaseProfiler.getTotalTime()/totalTimeForAllAgents * PERCENT;
		float AVERAGE_PER_AGENT_PER_UPDATE = totalTimeForAllAgents / ((float)gPhaseProfilers->steeringPhaseProfiler.getNumTimesExecuted()) * TO_MILLISECONDS;

		if (gShowStats || gShowAllStats)
		{
			std::cout << " amortized percent mid-term:   " << amortized_percent_mid_term << "\n";
			std::cout << " amortized percent short-term: " << amortized_percent_short_term << "\n";
			std::cout << " amortized percent perceptive: " << amortized_percent_perceptive << "\n";
			std::cout << " amortized percent predictive: " << amortized_percent_predictive << "\n";
			std::cout << " amortized percent reactive:   " << amortized_percent_reactive << "\n";
			std::cout << " amortized percent steering:   " << amortized_percent_steering << "\n";
			std::cout << " AVERAGE PER AGENT PER UPDATE: " << AVERAGE_PER_AGENT_PER_UPDATE << " milliseconds\n";
			std::cout << std::endl;
		}


		pprLogObject.addLogData(amortized_percent_mid_term);
		pprLogObject.addLogData(amortized_percent_short_term);
		pprLogObject.addLogData(amortized_percent_perceptive);
		pprLogObject.addLogData(amortized_percent_predictive);
		pprLogObject.addLogData(amortized_percent_reactive);
		pprLogObject.addLogData(amortized_percent_steering);
		pprLogObject.addLogData(AVERAGE_PER_AGENT_PER_UPDATE);

		_pprLogger->writeLogObject(pprLogObject);

	}

	gPhaseProfilers->aiProfiler.reset();
	gPhaseProfilers->longTermPhaseProfiler.reset();
	gPhaseProfilers->midTermPhaseProfiler.reset();
	gPhaseProfilers->shortTermPhaseProfiler.reset();
	gPhaseProfilers->perceptivePhaseProfiler.reset();
	gPhaseProfilers->predictivePhaseProfiler.reset();
	gPhaseProfilers->reactivePhaseProfiler.reset();
	gPhaseProfilers->steeringPhaseProfiler.reset();
}

void PPRAIModule::finish()
{
	// nothing to do here
	int i = 0;
}

SteerLib::AgentInterface * PPRAIModule::createAgent()
{
	PPRAgent * agent = new PPRAgent;
	agent->_id = gEngine->getAgents().size();

	return agent;
}



PLUGIN_API SteerLib::ModuleInterface * createModule() { return new PPRAIModule; }

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module ) { if (module) delete module; module = NULL; }

