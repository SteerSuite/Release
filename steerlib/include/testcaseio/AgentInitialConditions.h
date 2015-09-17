//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_AGENT_INITIAL_CONDITIONS_H__
#define __STEERLIB_AGENT_INITIAL_CONDITIONS_H__

/// @file AgentInitialConditions.h
/// @brief Declares the data structures relevant to an agent's initial conditions.

#include "Globals.h"
#include "util/Geometry.h"
#include "util/Color.h"
#include "testcaseio/Behaviour.h"


namespace SteerLib {

	/**
	 * @brief Describes the type of goal for an agent.
	 *
	 * This enum is one part of describing a goal for an agent.
	 * Refer to AgentGoalInfo to see all data that fully specifies a goal.
	 */
	enum AgentGoalTypeEnum {
		/// Seek a fixed target location.
		GOAL_TYPE_SEEK_STATIC_TARGET, 
		/// Flee a fixed target location.
		GOAL_TYPE_FLEE_STATIC_TARGET, 
		/// Seek (chase, pursue) a dynamic target.
		GOAL_TYPE_SEEK_DYNAMIC_TARGET, 
		/// Flee a dynamic target.
		GOAL_TYPE_FLEE_DYNAMIC_TARGET, 
		/// Generally progress in a certain direction.
		GOAL_TYPE_FLOW_STATIC_DIRECTION, 
		/// Follow a flow that dynamically changes, such as velocity field advection, boids, crowd following, etc.
		GOAL_TYPE_FLOW_DYNAMIC_DIRECTION, 
		/// Remain roughly stationary.
		GOAL_TYPE_IDLE,
	    /// Group goal that defines a target rectangular region.
		GOAL_TYPE_GROUP_TARGET,
		/// Axis Aligned region for goal.    Anyway in box counts as reached goal
		GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL
	};


	/**
	 * @brief The data structure that fully describes one goal of an agent.
	 *
	 * This data structure fully specifies an agent's goal.  This type of
	 * goal specification is used by SteerSuite XML test cases as well.
	 * It is, in some sense, a forward-looking attempt to provide a good interface between
	 * cognition and steering in a virtual agent.
	 *
	 * Note that no attempt has been made to define what it means to "achieve" a goal.  This
	 * choice was made because "success" criteria can vary significantly depending on the context.
	 *
	 */
	struct AgentGoalInfo {
		/// The type of goal
		AgentGoalTypeEnum goalType;
		/// Indicates whether the goal is random, or specified by the specific goal data.
		bool targetIsRandom;
		/// How much maximum time to spend on this goal, before proceeding to the next goal.
		float timeDuration;
		/// The desired speed the agent would try to maintain during this goal
		float desiredSpeed;

		/// @name Specific goal data
		/// @brief Only one of these is valid at any time, depending on the value of goalType and targetIsRandom.
		//@{
		/// The location of a static target for GOAL_TYPE_SEEK_STATIC_TARGET and GOAL_TYPE_FLEE_STATIC_TARGET.
		Util::Point targetLocation;
		/// The name of the dynamic target for GOAL_TYPE_SEEK_DYNAMIC_TARGET and GOAL_TYPE_FLEE_DYNAMIC_TARGET.
		std::string targetName;
		/// The desired direction to flow for GOAL_TYPE_FLOW_STATIC_DIRECTION.
		Util::Vector targetDirection;
		/// The type of dynamic flow to use for GOAL_TYPE_FLOW_DYNAMIC_DIRECTION.
		std::string flowType;
		/// The rectangular region of a target (used instead of a single point).
		Util::AxisAlignedBox targetRegion;
		/// holds the discription of the desired beavior to use to get to this goal
		Behaviour targetBehaviour;
		//@}
	};


	/** 
	 * @brief The initial conditions of a single agent based on the input test case.
	 *
	 * @see
	 *  - documentation of AgentGoalInfo.
	 */
	struct AgentInitialConditions {
		/// The (optional) name that identifies an agent; useful for describing the role of an agent, or naming it as a dynamic target.
		std::string name;
		/// The agent's initial position.
		Util::Point position;
		/// The agent's initial forward-facing direction.
		Util::Vector direction;
		/// The radius of the agent
		float radius;
		/// The initial speed of the agent (not the same as the desiredSpeed that is part of each goal)
		float speed;
		/// An ordered list of goals that the agent should try to complete.
		std::vector<AgentGoalInfo> goals;
		/// The color of the agent
		Util::Color color;
		/// false by default set to true if testcase specifies a color for the agent
		bool colorSet;
		bool fromRandom;
		Util::AxisAlignedBox randBox;

		AgentInitialConditions() : colorSet(false) {} // this is apparently the best method for initilization
	};

	static inline std::ostream &operator<<(std::ostream &out, const AgentInitialConditions &ai)
	{
		out << "Agent Initial Conditions" << std::endl <<
				"position: " << ai.position << "," <<
				" direction " << ai.direction << "," <<
				" radius " << ai.radius <<
				" number of goals " << ai.goals.size() << ")";
		return out;
	}

} // end namespace SteerLib

#endif
