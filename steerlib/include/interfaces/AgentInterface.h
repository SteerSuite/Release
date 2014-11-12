//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_AGENT_INTERFACE_H__
#define __STEERLIB_AGENT_INTERFACE_H__

/// @file AgentInterface.h
/// @brief Declares the SteerLib::AgentInterface virtual interface.

#include <queue>
#include "Globals.h"
#include "testcaseio/AgentInitialConditions.h"
#include "griddatabase/GridDatabase2D.h"
#include "util/Geometry.h"

namespace SteerLib {

	// forward declaration
	class STEERLIB_API EngineInterface;


	/**
	 * @brief The virtual interface for AI agents.
	 *
	 * This class is the virtual interface for a single AI agent, usually instantiated by a module.  Inherit and 
	 * implement this virtual interface to create your own custom AI agents.  Generally, an instance of SteerLib::ModuleInterface
	 * is responsible for allocating and de-allocating your agent class that inherits this interface.
	 *
	 * During reset(), the agent is given a reference to the engine, which remains valid as long as the agent exists.  Among
	 * other functionality provided by the engine, the agent has access to a spatial database which allows the agent to query
	 * about the environment and other agents.
	 * If you want your implementation to take advantage of this database, then <b>it is your responsibility</b> to add this
	 * agent to the spatial database, to update the database as your agents move around, and to remove the agents from the
	 * database during cleanup.
	 *
	 * @see
	 *  - The SteerLib::EngineInterface provides functionality to modules and agents.
	 *  - The SteerLib::ModuleInterface is the main class to inherit and implement for creating a module
	 *  - The SteerLib::GridDatabase2D is a spatial database that agents may want to use to interact with other agents, such as
	 *    nearest-neighbor queries or ray tracing.
	 *
	 */
	class STEERLIB_API AgentInterface : public SteerLib::SpatialDatabaseItem
	{
	public:
		virtual ~AgentInterface() { }
		/// @name Core functionality
		//@{
		/// Resets an agent, could be called several times, even during the simulation.
		virtual void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) = 0;
		// disable the current agent, i.e. remove it from the simulation.
		virtual void disable() = 0;
		/// Called once per frame by the engine, update the agent here.
		virtual void updateAI(float timeStamp, float dt, unsigned int frameNumber) = 0;
		/// Called once per frame by the engine, use openGL to draw an agent here.
		virtual void draw() = 0;
		//@}

		/// @name Accessors to query info about the agent
		//@{
		/// Returns true if the agent is active/enabled, false if it is inactive/disabled.
		virtual bool enabled() const = 0;
		/// Returns a point representing the position of the agent.
		virtual Util::Point position() const = 0;
		/// Returns a vector that points in the direction the agent is facing; this can potentially be different than the direction the agent is moving.
		virtual Util::Vector forward() const = 0;
		/// Returns the velocity of the Agent.
		virtual Util::Vector velocity() const = 0;
		/// Returns the radius of the Agent.
		virtual float radius() const = 0;
		// Returns the id number associated with this agent
		virtual size_t id() const = 0;
		/// Returns information about the current goal.
		virtual const SteerLib::AgentGoalInfo & currentGoal() const = 0;
		// Get the list of all the goals
		// I don't know how you can give someone a queue and not have them need to modify it?
		virtual const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const = 0;
		//@}

		/// @name Some convenience functions so users can manipulate agents more explicitly
		//@{
		/// Adds a goal to the agent's existing list of goals
		virtual void addGoal(const SteerLib::AgentGoalInfo & newGoal) = 0;
		/// Clears the agent's existing list of goals.
		virtual void clearGoals() = 0;
		//@}

		/// @name The SpatialDatabaseItem interface
		/// @brief Some defaults are given, but can be overridden if desired.
		//@{
		/// Returns true if the agent is finished, for simulations with staggered agent presence
		virtual bool finished(void) { return enabled(); }
		virtual bool isAgent() { return true; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0; }
		virtual bool intersects(const Util::Ray &r, float &t) = 0;
		virtual bool overlaps(const Util::Point & p, float radius) = 0;
		virtual float computePenetration(const Util::Point & p, float radius) = 0;

		virtual void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) = 0;
		virtual void setParameters(SteerLib::Behaviour behave) = 0;
		//@}

		/*
		 * Is a1 closer to me than a2
		 */
		bool compareDistance(SteerLib::AgentInterface * a1, SteerLib::AgentInterface * a2 )
		{
			return ( (position() - a1->position()).length() < (position() - a2->position()).length() );
		}

		virtual bool collidesAtTimeWith(const Util::Point & p1, const Util::Vector & directiontToRightOfAgent, float otherAgentRadius, float timeStamp, float otherAgentFootX, float otherAgentFootZ)
		{// This is to support footstepsAI, but prediction would be cool.
			// std::cout << "not supported by this agent" << ", time " << timeStamp << std::endl;
			if ( (computePenetration( p1, otherAgentRadius) > 0.0001) ||
					(computePenetration( Util::Point(otherAgentFootX, 0.0, otherAgentFootZ), otherAgentRadius) > 0.0001)
					// (computePenetration( p1+directiontToRightOfAgent, otherAgentRadius) > 0.0001) ||
					// (computePenetration( p1-directiontToRightOfAgent, otherAgentRadius) > 0.0001)
					)
			{
				return true;
			}
			return false;
		}


		static SteerLib::AgentInitialConditions getAgentConditions(SteerLib::AgentInterface * agentInterface)
		{
			SteerLib::AgentInitialConditions initialConditions;
			initialConditions.speed = agentInterface->velocity().length();
			initialConditions.direction = agentInterface->forward();
			std::vector<SteerLib::AgentGoalInfo> goals;
			std::queue<SteerLib::AgentGoalInfo> tmp_goals(agentInterface->agentGoals());
			// for (int i = 0; i < agentInterface->)

			// TODO HUGE PROBLEM HERE
			/**
			 * Because AgentInterface does not support access to goal list.
			 */
			size_t g;
			for ( g=0; g < tmp_goals.size(); g++)
			{
				goals.push_back(tmp_goals.front());
				// cycle through goals;
				tmp_goals.push(tmp_goals.front());
				tmp_goals.pop();
			}
			// goals.push_back(agentInterface->currentGoal());
			initialConditions.goals = goals;
			initialConditions.radius = agentInterface->radius();
			std::stringstream name;
			name << "agent" << agentInterface->id();
			initialConditions.name = name.str(); // TODO AgentInterface does not support name
			initialConditions.position = agentInterface->position();
			//initialConditions.
			return initialConditions;
		}

		/*
		std::ostream & operator<<(std::ostream &out)
		{ // methods used here must be const
			out << "agent" << velocity() << std::endl;
			return out;
		}*/


	};


	inline std::ostream &operator<<(std::ostream &out, const AgentInterface &a)
	{ // methods used here must be const
		out << "agent# " << a.id() << " at " << a.position();
		return out;
	}

} // end namespace SteerLib

#endif

