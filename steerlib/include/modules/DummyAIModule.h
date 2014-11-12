//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_DUMMY_AI_MODULE_H__
#define __STEERLIB_DUMMY_AI_MODULE_H__

/// @file DummyAIModule.h
/// @brief Declares the DummyAIModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"

namespace SteerLib {

	class DummyAgent : public SteerLib::AgentInterface
	{
		void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) {

			_position = initialConditions.position;
			_forward = initialConditions.direction;
			_radius = initialConditions.radius;


			if (initialConditions.goals.size() > 0) {
				_currentGoal = initialConditions.goals[0];
			}
			else {
				throw Util::GenericException("No goals were specified!\n");
			}
		}

		/// The DummyAgent does absolutely nothing during each update.
		void updateAI(float timeStamp, float dt, unsigned int frameNumber) { }

		// The dummy Agent is always enabled
		void disable() {}

		void draw() {
#ifdef ENABLE_GUI
			Util::DrawLib::drawAgentDisc(_position, _forward, _radius, Util::gBlack); 
			if (_currentGoal.goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
				Util::DrawLib::drawFlag(_currentGoal.targetLocation);
			}
#endif
		}


		bool enabled() const { return true; }
		Util::Point position() const { return _position; }
		Util::Vector forward() const { return _forward; }
		Util::Vector velocity() const { throw Util::GenericException("velocity() not implemented yet"); }
		size_t id() const { return 0;}
		float radius() const { return _radius; }
		const SteerLib::AgentGoalInfo & currentGoal() const { return _currentGoal; }
		const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const { throw Util::GenericException("agentGoals() not implemented yet"); }
		void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for DummyAgent"); }
		void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for DummyAgent"); }

		bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
		bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
		float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }

		void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) { throw Util::GenericException("insertAgentNeighbor not implemented yet for BenchmarkAgent"); }
		void setParameters(Behaviour behave)
		{
			throw Util::GenericException("setParameters() not implemented yet for this Agent");
		}

	protected:
		Util::Point _position;
		Util::Vector _forward;
		float _radius;
		SteerLib::AgentGoalInfo _currentGoal;
	};



	class DummyAIModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return "testCasePlayer"; }
		std::string getConflicts() { return ""; }
		std::string getData() { return ""; }
		LogData * getLogData() { return new LogData(); }
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) { }
		void finish() { }

		SteerLib::AgentInterface * createAgent() { return new DummyAgent; }
		void destroyAgent( SteerLib::AgentInterface * agent ) { assert(agent!=NULL);  delete agent;  agent = NULL; }
	};

} // end namespace SteerLib

#endif
