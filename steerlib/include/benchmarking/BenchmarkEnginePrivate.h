//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_BENCHMARK_ENGINE_PRIVATE_H__
#define __STEERLIB_BENCHMARK_ENGINE_PRIVATE_H__

/// @file BenchmarkEnginePrivate.h
/// @brief Contains private functionality for the benchmark engine.

#include <string>
#include "Globals.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "interfaces/ObstacleInterface.h"
#include "benchmarking/AgentMetricsCollector.h"
#include "benchmarking/SimulationMetricsCollector.h"
#include "benchmarking/BenchmarkEnginePrivate.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {

	/**
	 * @brief The dummy agent used by the BenchmarkEngine
	 *
	 * @todo
	 *   - finish documenting this class
	 */
	class STEERLIB_API BenchmarkAgent : public SteerLib::AgentInterface
	{
	public:
		void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) {
			throw Util::GenericException("This agent class is not meant to be used outside of benchmarking");
		};
		void updateAI(float timeStamp, float dt, unsigned int frameNumber) {
			throw Util::GenericException("This agent class is not meant to be used outside of benchmarking");
		};

		void disable()
		{
			throw Util::GenericException("This agent class is not meant to be used outside of benchmarking");
		};
		void draw() {
			throw Util::GenericException("This agent class is not meant to be used outside of benchmarking");
		};
		bool enabled() const { return _enabled; }
		Util::Point position() const { return _position; }
		Util::Vector forward() const { return _forward; }
		Util::Vector velocity() const { throw Util::GenericException("velocity() not implemented yet"); }
		size_t id() const { return 0;}
		float radius() const { return _radius; }
		const SteerLib::AgentGoalInfo & currentGoal() const { return _currentGoal; }
		const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const { throw Util::GenericException("agentGoals() not implemented yet"); }
		void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for BenchmarkAgent"); }
		void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for BenchmarkAgent"); }

		bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
		bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
		float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }

		inline void setEnabled(bool newEnabled) { _enabled = newEnabled; }
		inline void setPosition(const Util::Point & newPosition) { _position = newPosition; }
		inline void setForward(const Util::Vector & newForward) { _forward = newForward; }
		inline void setRadius(float newRadius) { _radius = newRadius; }
		inline void setCurrentGoal(const SteerLib::AgentGoalInfo & newGoal) { _currentGoal = newGoal; }
		inline Util::AxisAlignedBox getBounds() { return Util::AxisAlignedBox(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius); }

		void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) { throw Util::GenericException("insertAgentNeighbor not implemented yet for BenchmarkAgent"); }
		void setParameters(Behaviour behave)
		{
			throw Util::GenericException("setParameters() not implemented yet for this Agent");
		}

	protected:
		bool _enabled;
		Util::Point _position;
		Util::Vector _forward;
		float _radius;
		SteerLib::AgentGoalInfo _currentGoal;
	};



	/**
	 * @brief Private functionality for the SteerLib::BenchmarkEngine class.
	 *
	 * This class should not be used directly.  Instead, use the BenchmarkEngine public interface that
	 * inherits from this class.
	 */
	class STEERLIB_API BenchmarkEnginePrivate
	{
	protected:
		bool _done;
		unsigned int _currentFrameNumber;
		SteerLib::GridDatabase2D * _spatialDatabase;
		SteerLib::RecFileReader * _recFileReader;
		std::vector<SteerLib::AgentInterface*> _agents;
		std::vector<SteerLib::ObstacleInterface*> _obstacles;
		SteerLib::SimulationMetricsCollector * _simulationMetricsCollector;
		SteerLib::BenchmarkTechniqueInterface * _benchmarkTechnique;
	};

} // end namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif


