//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/*
 * RVO2D_Parameters.h
 *
 *  Created on: 2013-04-11
 *      Author: glenpb
 */

#ifndef RVO2D_PARAMETERS_H_
#define RVO2D_PARAMETERS_H_

#define NEIGHBOR_DISTANCE 10.0f
#define MAX_NEIGHBORS 10
#define TIME_HORIZON 2.0f
#define MAX_SPEED 1.33f
// #define PREFERRED_SPEED 1.33f
#define	 TIME_HORIZON_OBSTACLES 2.0f

#define NEXT_WAYPOINT_DISTANCE 5

#define REACHED_WAYPOINT_MULTIPLIER 1
#define REACHED_GOAL_MULTIPLIER 2.5


#define USE_PLANNING 1
#define DRAW_ANNOTATIONS 1


class RVO2DParameters
{
public:
	// Adding a bunch of parameters so they can be changed via input
	float rvo_neighbor_distance;
	float rvo_time_horizon;
	float rvo_max_speed;
	float rvo_preferred_speed;
	float rvo_time_horizon_obstacles;
	int rvo_max_neighbors;
	int next_waypoint_distance;

	void setParameters(SteerLib::Behaviour behavior)
	{
		// std::cout << "Setting parameters from behaviour" << std::endl;
		int i;
		for ( i = 0; i < behavior.getParameters().size(); i++)
		{
			std::string p_key = behavior.getParameters().at(i).key;
			std::stringstream value(behavior.getParameters().at(i).value);
			// std::cout << "key: " << p_key << ", value: " << value.str() << std::endl;
			if (p_key == "rvo_neighbor_distance")
			{
				value >> rvo_neighbor_distance;
			}
			else if (p_key == "rvo_time_horizon")
			{
				value >> rvo_time_horizon;
			}
			else if (p_key == "rvo_max_speed")
			{
				value >> rvo_max_speed;
				// std::cout << "Setting rvo_max_speed to: " << rvo_max_speed << std::endl;
			}
			else if (p_key == "rvo_preferred_speed")
			{
				value >> rvo_preferred_speed;
			}
			else if (p_key == "rvo_time_horizon_obstacles")
			{
				// std::cout << "Setting rvo_time_horizon_obstacles to: " << value.str() << std::endl;
				value >> rvo_time_horizon_obstacles;
			}
			else if (p_key == "rvo_max_neighbors")
			{
				value >> rvo_max_neighbors;
			}
			else if (p_key == "next_waypoint_distance")
			{
				// std::cout << "Setting next_waypoint_distance to: " << value.str() << std::endl;
				value >> next_waypoint_distance;
			}
		}
	}
};



#endif /* RVO2D_PARAMETERS_H_ */
