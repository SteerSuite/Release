//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_REC_FILE_PLAYER_MODULE_H__
#define __STEERLIB_REC_FILE_PLAYER_MODULE_H__

/// @file RecFilePlayerModule.h
/// @brief Declares the RecFilePlayerModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"
#include "obstacles/BoxObstacle.h"

namespace SteerLib
{

	class ReplayAgent : public SteerLib::AgentInterface
	{
	public:
		void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) { }
		void updateAI(float timeStamp, float dt, unsigned int frameNumber) { }
		void disable() {};
		void draw();

		bool enabled() const { return _enabled; }
		Util::Point position() const { return _position; }
		Util::Vector forward() const { return _forward; }
		Util::Vector velocity() const { return _velocity; }
		size_t id() const { return 0;}
		float radius() const { return _radius; }
		const SteerLib::AgentGoalInfo & currentGoal() const { return _currentGoal; }
		const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const { throw Util::GenericException("agentGoals() not implemented yet"); }
		void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for ReplayAgent"); }
		void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for ReplayAgent"); }

		bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
		bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
		float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }

		void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) { throw Util::GenericException("insertAgentNeighbor not implemented yet for BenchmarkAgent"); }
		void setParameters(Behaviour behave)
		{
			throw Util::GenericException("setParameters() not implemented yet for this Agent");
		}

		// native functionality
		void setPosition(const Util::Point & newPosition) { _position = newPosition; }
		void setForward(const Util::Vector & newForward) { _forward = newForward; }
		void setEnabled(const bool newEnabled) { _enabled = newEnabled; }
		void setRadius(const float newRadius) { _radius = newRadius; }
		void setColor(const Util::Color newColor) { _color = newColor; }
		void setVelocity(const Util::Vector & newVelocity)
		{
			_velocity = newVelocity;
		}
		void setCurrentGoal(const SteerLib::AgentGoalInfo & newGoal) { _currentGoal = newGoal; }


	protected:
		Util::Point _position;
		Util::Vector _forward;
		Util::Vector _velocity;
		Util::Color _color;
		bool _enabled;
		float _radius;
		SteerLib::AgentGoalInfo _currentGoal;

// #define DRAW_HISTORIES 1

#ifdef DRAW_HISTORIES
		std::deque<Util::Point> __oldPositions;
#endif

	};



	class RecFilePlayerModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return ""; }
		std::string getConflicts() { return "testCasePlayer"; }
		std::string getData() { return ""; }
		LogData * getLogData() { return new LogData(); }
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
		void finish();
		void processKeyboardInput(int key, int action );
		void initializeSimulation();
		void cleanupSimulation();
		void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	protected:
		SteerLib::EngineInterface * _engine;
		SteerLib::RecFileReader * _simulationReader;
		double _playbackSpeed;
		double _simulationStartTime;
		double _simulationStopTime;
		double _currentTimeToPlayback;

		double _fixedTimeStep;

		std::vector<SteerLib::BoxObstacle *> _obstacles;
		std::string _recFilename;

	};

} // end namespace SteerLib

#endif
