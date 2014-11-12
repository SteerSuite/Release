//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * SocialForces_Parameters.h
 *
 *  Created on: 2013-04-11
 *      Author: glenpb
 */

#ifndef SocialForces_PARAMETERS_H_
#define SocialForces_PARAMETERS_H_

// #include "testcaseio/Behaviour.h"

#define MAX_SPEED 2.6f
#define PERFERED_SPEED 1.33 // TODO not added to parameters yet.


#define ACCELERATION 0.5 // = v/A
#define PERSONAL_SPACE_THRESHOLD 0.3 // not defined in HiDAC papaer
#define AGENT_REPULSION_IMPORTANCE 0.3 // in HiDAC
#define QUERY_RADIUS 3.0f // not defined in paper
#define BODY_FORCE 1500.0f // K (big K) 120000 / 80
#define AGENT_BODY_FORCE 1500.0f
#define SLIDING_FRICTION_FORCE 3000.0f // k (small k) 240000 / 80 = 3000
#define AGENT_B 0.08f // inverse proximity force importance
#define AGENT_A 25.0f // 2000 / 80 Yep its just called A... inverse proximity force importance
#define WALL_B 0.08f //  inverse proximity force importance
#define WALL_A 25.0f //  proximity force importance
#define FURTHEST_LOCAL_TARGET_DISTANCE 45


#define MASS 1
// #define WAYPOINT_THRESHOLD_MULTIPLIER 2.5
// #define GOAL_THRESHOLD_MULTIPLIER 10.5
#define WAYPOINT_THRESHOLD_MULTIPLIER 1
#define GOAL_THRESHOLD_MULTIPLIER 2.5

#define USE_PLANNING 1
// #define DRAW_ANNOTATIONS 1

// #define _DEBUG_ 1
namespace SocialForcesGlobals {

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
	extern float sf_acceleration;
	extern float sf_personal_space_threshold;
	extern float sf_agent_repulsion_importance;
	extern float sf_query_radius;
	extern float sf_body_force;
	extern float sf_agent_body_force;
	extern float sf_sliding_friction_force;
	extern float sf_agent_b;
	extern float sf_agent_a;
	extern float sf_wall_b;
	extern float sf_wall_a;
	extern float sf_max_speed;



	extern PhaseProfilers * gPhaseProfilers;
}


class SocialForcesParameters
{
public:
	// Adding a bunch of parameters so they can be changed via input
	float sf_acceleration;
	float sf_personal_space_threshold;
	float sf_agent_repulsion_importance;
	float sf_query_radius;
	float sf_body_force;
	float sf_agent_body_force;
	float sf_sliding_friction_force;
	float sf_agent_b;
	float sf_agent_a;
	float sf_wall_b;
	float sf_wall_a;
	float sf_max_speed;

	void setParameters(SteerLib::Behaviour behavior)
	{
		// std::cout << "Setting parameters from behaviour" << std::endl;
		int i;
		for ( i = 0; i < behavior.getParameters().size(); i++)
		{
			std::string p_key = behavior.getParameters().at(i).key;
			std::stringstream value(behavior.getParameters().at(i).value);
			// std::cout << "key: " << p_key << ", value: " << value << std::endl;
			if (p_key == "sf_acceleration")
			{
				value >> sf_acceleration;
				// std::cout << "set sf acceleration to " << sf_acceleration << std::endl;
			}
			else if (p_key == "sf_personal_space_threshold")
			{
				value >> sf_personal_space_threshold;
			}
			else if (p_key == "sf_agent_repulsion_importance")
			{
				value >> sf_agent_repulsion_importance;
			}
			else if (p_key == "sf_query_radius")
			{
				value >> sf_query_radius;
			}
			else if (p_key == "sf_body_force")
			{
				value >> sf_body_force;
			}
			else if (p_key == "sf_agent_body_force")
			{
				value >> sf_agent_body_force;
			}
			else if (p_key == "sf_sliding_friction_force")
			{
				value >> sf_sliding_friction_force;
				// std::cout << "*************** set sf_sliding_friction_force to " << sf_sliding_friction_force << std::endl;
			}
			else if (p_key == "sf_agent_b")
			{
				value >> sf_agent_b;
			}
			else if (p_key == "sf_agent_a")
			{
				value >> sf_agent_a;
			}
			else if (p_key == "sf_wall_b")
			{
				value >> sf_wall_b;
			}
			else if (p_key == "sf_wall_a")
			{
				value >> sf_wall_a;
			}
			else if (p_key == "sf_max_speed")
			{
				value >> sf_max_speed;
			}
		}
		
	}
};

inline std::ostream &operator<<(std::ostream & out, const SocialForcesParameters & p)
{ // methods used here must be const
	out << "sf_acceleration: " << p.sf_acceleration << std::endl;
	out << "sf_personal_space_threshold: " << p.sf_personal_space_threshold << std::endl;
	out << "sf_agent_repulsion_importance: " << p.sf_agent_repulsion_importance << std::endl;
	out << "sf_query_radius: " << p.sf_query_radius << std::endl;
	out << "sf_body_force: " << p.sf_body_force << std::endl;
	out << "sf_sliding_friction_force: " << p.sf_sliding_friction_force << std::endl;
	out << "sf_agent_b: " << p.sf_agent_b << std::endl;
	out << "sf_agent_a: " << p.sf_agent_a << std::endl;
	out << "sf_wall_b: " << p.sf_wall_b << std::endl;
	out << "sf_wall_a: " << p.sf_wall_a << std::endl;
	out << "sf_max_speed: " << p.sf_max_speed;

	return out;
}



#endif /* SocialForces_PARAMETERS_H_ */
