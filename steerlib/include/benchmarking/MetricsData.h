//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_METRICS_DATA_H__
#define __STEERLIB_METRICS_DATA_H__

/// @file MetricsData.h
/// @brief Declares data structures for metrics used in benchmarking.
///
/// @todo
///   - get the full templated Metric class, to be used in the AgentMetrics struct.
///   - fill-in EnvironmentMetrics
///   - finish documenting the classes in this file
///   - where does CollisionInfo belong?
///

#include "Globals.h"
#include "util/Geometry.h"
#include "util/Misc.h"


namespace SteerLib {


	struct STEERLIB_API CollisionInfo
	{
		uintptr_t collisionKey;
		float maxPenetration;
		float startTime;
		float endTime;
		float timeDuration;
	};


	/**
	 * @brief Contains all metrics for an agent.
	 *
	 * This class contains all time varying metrics for an agent.
	 *
	 * Note carefully our definition of an <b>instantaneous metric</b>:  a measurement
	 * that has a meaningful value at any instant in time; i.e. a function of time.  So,
	 * for example, "total acceleration" is an instantaneous metric; even though it represents the 
	 * integral of instantaneous acceleration, it does also have a meaningful value
	 * at any instant in time.
	 *
	 */
	class STEERLIB_API AgentMetrics
	{
	public:

		void reset();

		/// @name timing metrics
		//@{
		/// units: number of frames
		unsigned int totalNumFramesEnabled;
		/// units: seconds
		float totalTimeEnabled;
		//@}

		/// @name collision metrics:
		//@{
		/// a score that indicates how badly collisions occurred; increments by one for every collision that is found in every frame.
		unsigned int collisionScore;
		/// a unique collision is one instance of a collision from the point of overlap to the point of exit; units: number of collisions
		unsigned int numUniqueCollisions;
		/// max penetration of any single collision over all collisions;  units: meters
		float maxCollisionPenetration;
		/// max duration of any single unique collision; units: seconds
		float maxTimeSpentInCollision;
		//@}

		/// @name turning metrics:
		//@{
		float instantaneousAngularSpeed;
		float totalDegreesTurned;  // units: degrees
		float maxAngularSpeed;     // units: degrees/second
		float totalDegreesTurnedOverWindow;
		float avgAngularSpeedOverWindow;
		float minDegreesTurnedOverWindow;
		float maxDegreesTurnedOverWindow;    // units: degrees
		// unsigned int numTimesAngularSpeedChangedSignOverWindow;
		//@}

		/// @name distance/scalar speed metrics:
		//@{
		float instantaneousSpeed;     // units: meters/second
		float totalDistanceTraveled;  // units: meters
		float maxInstantaneousSpeed;  // units: meters/second
		float totalDistanceTraveledOverWindow;
		float avgSpeedOverWindow;
		float minDistanceTraveledOverWindow;      // units: meters
		float maxDistanceTraveledOverWindow;      // units: meters
		//@}

		/// @name scalar change in speed metrics (different from acceleration):
		//@{
		float instantaneousChangeInSpeed;
		float totalChangeInSpeed;     // units: meters/second
		float maxChangeInSpeed;       // units: meters/(second^2)
		float totalChangeInSpeedOverWindow;
		float avgChangeInSpeedOverWindow;
		float minChangeInSpeedOverWindow;
		float maxChangeInSpeedOverWindow; // units: meters/second
		//@}

		/// @name acceleration metrics:
		//@{
		Util::Vector instantaneousAcceleration;
		float sumTotalOfInstantaneousAcceleration; // units: meters/(second^2)   NOTE CAREFULLY: this value is not the integral of acceleration, i.e. not "time-independent".
		float totalAcceleration;      // units: meters/second    NOTE CAREFULLY:  this is the estimate of integral of acceleration over an interval.
		float maxAcceleration;        // units: meters/(second^2)
		float totalAccelerationOverWindow;
		float avgAccelerationOverWindow;
		float minAccelerationOverWindow;
		float maxAccelerationOverWindow;  // units: 
		unsigned int numTimesAccelerationChangedSignOverWindow;
		unsigned int numTimesVelocityChangedSignOverWindow;
		//@}

		/// @name energy metrics:
		//@{
		float instantaneousKineticEnergy;  // units: Kg * (m/s)^2 (mass assumed to be 1.0)
		float sumTotalOfInstantaneousKineticEnergies; // units:  Kg * (m/s)^2   (mass is assumed to be 1.0 for all agents)
		//@}

		/// @name spatial location metrics:
		//@{
		// float distanceToNearestObstacle;
		// float distanceToNearestAgent;
		// float distanceToOptimalPath;
		// float avgDistanceToNearestObstacleOverWindow;
		// float avgDistanceToNearestAgentOverWindow;
		// float avgDistanceToOptimalPathOverWindow;
		float displacementOverWindow;
		//@}

		/// @name energy metrics:
		//@{
		float integralOfKineticEnergy;
		float averageKineticEnergy;
		//@}


		// MUBBASIR 2011-07
		float pleEnergy;

	};


	/**
	 * @brief Contains all metrics related to the environment.
	 *
	 *  Currently there is no data here.  Eventually there will be
	 *  measurements such as density, average velocity, etc.
	 */
	class STEERLIB_API EnvironmentMetrics {
		void reset() { }
	};




#define WINDOW_SIZE 60

	/**
	* @brief Implements a history window for a metric over 60 frames.
	*
	* @todo
	*  - need to migrate to a time interval rather than specific number of frames for window analysis.
	*    the time interval should probably be 3 seconds.
	*
	*/
	template <class T> class STEERLIB_API windowArray {
	public:
		windowArray() {
			start = 0;
			end = WINDOW_SIZE - 1;
		}
		int size() {
			return WINDOW_SIZE;
		}
		void advanceByOne(T newValue) {
			start = start+1;
			end = (start+WINDOW_SIZE-1) % WINDOW_SIZE;
			values[end] = newValue;
		}
		T& operator[] (int index) { 
			if ((index < 0) || (index >= WINDOW_SIZE)) {
				throw Util::GenericException("windowArray[" + Util::toString(index) + "] out of range.  Valid range is 0 to " + Util::toString(WINDOW_SIZE-1) + ".");
			}
			int realIndex = (start+index)%WINDOW_SIZE;
			return values[realIndex];
		}
	private:
		unsigned int start, end;
		T values[WINDOW_SIZE];
	};


} // end namespace SteerLib

#endif
