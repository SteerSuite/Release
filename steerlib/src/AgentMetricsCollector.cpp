//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file AgentMetricsCollector.cpp
/// @brief Implements the SteerLib::AgentMetricsCollector class

#include <ios>
#include <iostream>
#include <iomanip>

//#include <algorithm>
//#include <string>

// this causes linker error in hammerhead 
#include <stdint.h>

#include "benchmarking/AgentMetricsCollector.h"

// #define _DEBUG_1 1

using namespace std;
using namespace SteerLib;
using namespace Util;

// TODO this class should use the pointer it has to the agent. It will have the most up to date data
AgentMetricsCollector::AgentMetricsCollector(SteerLib::AgentInterface * agent)
{
	_agentBeingAnalyzed = agent;

	reset();
}

void AgentMetricsCollector::reset()
{
	_numFramesMeasured = 0;

	// initialize the agent info that we track
	_radius = _agentBeingAnalyzed->radius();
	_enabled = _agentBeingAnalyzed->enabled();
	_currentPosition = _agentBeingAnalyzed->position();
	_previousPosition = _agentBeingAnalyzed->position();
	_currentDirection = _agentBeingAnalyzed->forward();
	_previousDirection = _agentBeingAnalyzed->forward();

	// initialize collision stats and metrics
	_currentCollidingObjects.clear();
	_pastCollisions.clear();
	_metrics.reset();
}

/// @todo move this function to a better place.
void AgentMetrics::reset()
{
	// NOTE: all "max", values start at 0.0f, not -INFINITY, because their values will always be positive.
	// "min" values start at positive INFINITY.
	totalNumFramesEnabled = 0;
	totalTimeEnabled = 0.0f;

	collisionScore = 0;
	numUniqueCollisions = 0;
	maxCollisionPenetration = 0.0f;
	maxTimeSpentInCollision = 0.0f;

	instantaneousAngularSpeed = 0.0f;
	totalDegreesTurned = 0.0f;
	maxAngularSpeed = 0.0f;
	totalDegreesTurnedOverWindow = 0.0f;
	avgAngularSpeedOverWindow = 0.0f;
	minDegreesTurnedOverWindow = INFINITY;
	maxDegreesTurnedOverWindow = 0.0f;

	instantaneousSpeed = 0.0f;
	totalDistanceTraveled = 0.0f;
	maxInstantaneousSpeed = 0.0f;
	totalDistanceTraveledOverWindow = 0.0f;
	avgSpeedOverWindow = 0.0f;
	minDistanceTraveledOverWindow = INFINITY;
	maxDistanceTraveledOverWindow = 0.0f;

	instantaneousChangeInSpeed = 0.0f;
	totalChangeInSpeed = 0.0f;
	maxChangeInSpeed = 0.0f;
	totalChangeInSpeedOverWindow = 0.0f;
	avgChangeInSpeedOverWindow = 0.0f;
	minChangeInSpeedOverWindow = INFINITY;
	maxChangeInSpeedOverWindow = 0.0f;

	instantaneousAcceleration = Vector(0.0f, 0.0f, 0.0f);
	sumTotalOfInstantaneousAcceleration = 0.0f;
	totalAcceleration = 0.0f;
	maxAcceleration = 0.0f;
	totalAccelerationOverWindow = 0.0f;
	avgAccelerationOverWindow = 0.0f;
	minAccelerationOverWindow = INFINITY;
	maxAccelerationOverWindow = 0.0f;

	numTimesAccelerationChangedSignOverWindow = 0;
	numTimesVelocityChangedSignOverWindow = 0;

	instantaneousKineticEnergy = 0.0f;
	sumTotalOfInstantaneousKineticEnergies = 0.0f;

	pleEnergy = 0.0f; 
	_totalPenetration = 0.0f;

	// spatial location metrics:
	// distanceToNearestObstacle = 0.0f;
	// distanceToNearestAgent = 0.0f;
	// distanceToOptimalPath = 0.0f;
	// avgDistanceToNearestObstacleOverWindow = 0.0f;
	// avgDistanceToNearestAgentOverWindow = 0.0f;
	// avgDistanceToOptimalPathOverWindow = 0.0f;

	displacementOverWindow = 0.0f;

	//numTimesAngularSpeedChangedSignOverWindow = 0;


	integralOfKineticEnergy = 0.0f;
	averageKineticEnergy = 0.0f;

}


unsigned int AgentMetricsCollector::getNumThresholdedCollisions(float penetrationThreshold, float timeDurationThreshold)
{
	assert(_metrics.numUniqueCollisions == _pastCollisions.size()+_currentCollidingObjects.size());
	unsigned int numThresholdedCollisions = 0;

	// if there is no thresholding, its OK to just return the total number of unique collisions.
	if (penetrationThreshold <= 0.0f && timeDurationThreshold <= 0.0f) {
		assert(_metrics.numUniqueCollisions == _pastCollisions.size() + _currentCollidingObjects.size());
		return _metrics.numUniqueCollisions;
	}

	// otherwise, go through the entire history of collisions.
	for (unsigned int i=0; i<_pastCollisions.size(); i++) {
		if ((_pastCollisions[i].maxPenetration > penetrationThreshold) && (_pastCollisions[i].timeDuration > timeDurationThreshold)) {
			numThresholdedCollisions++;
		}
	}

	for (unsigned int i=0; i<_currentCollidingObjects.size(); i++) {
		if ((_currentCollidingObjects[i].maxPenetration > penetrationThreshold) && (_currentCollidingObjects[i].timeDuration > timeDurationThreshold)) {
			numThresholdedCollisions++;
		}
	}

	return numThresholdedCollisions;
}


void AgentMetricsCollector::_checkAndUpdateOneCollision(uintptr_t collisionKey, float penetration, float currentTimeStamp)
{
	if (penetration > 0.0f + COLLISION_EPSILON )
	{
		_metrics.collisionScore++;
		// std::cout << "Found new collision" << std::endl;
#ifdef _DEBUG_1
			std::cout << "Collision: " << std::endl;
#endif
		if (_currentCollidingObjects.find(collisionKey) == _currentCollidingObjects.end()){
			//
			// existing collision with this object not found, so it is a new collision
			//
			CollisionInfo newCollision;
			newCollision.collisionKey = collisionKey;
			newCollision.maxPenetration = penetration;
			newCollision.startTime = currentTimeStamp;
			newCollision.endTime = currentTimeStamp;
			newCollision.timeDuration = 0.0f;
			// the [] operator actually inserts the new collisionInfo using key collisionKey, because it is not found.  (see STL map documentation)
			_currentCollidingObjects[collisionKey] = newCollision;
			_metrics.numUniqueCollisions++;
		}
		else {
			//
			// update the existing collision
			//
			_currentCollidingObjects[collisionKey].maxPenetration = max(_currentCollidingObjects[collisionKey].maxPenetration, penetration);
			_currentCollidingObjects[collisionKey].endTime = currentTimeStamp;
			_currentCollidingObjects[collisionKey].timeDuration = _currentCollidingObjects[collisionKey].endTime - _currentCollidingObjects[collisionKey].startTime;
		}
		float e_c = 10; // J / (Kg * m * s)
		_metrics._totalPenetration += ( penetration * e_c );
	}
	else {
		//
		// remove the collision, if it exists -- i.e. exited from a collision.
		// that way if it collides again its considered a new collision.
		// 
		// at the same time, update the agent's stats on max penetration and max time duration
		//
		if (_currentCollidingObjects.find(collisionKey) != _currentCollidingObjects.end()){
			CollisionInfo oldCollision = _currentCollidingObjects[collisionKey];
			_currentCollidingObjects.erase(collisionKey);
			oldCollision.endTime = currentTimeStamp;
			oldCollision.timeDuration = oldCollision.endTime - oldCollision.startTime;
			_pastCollisions.push_back(oldCollision);

			if (_metrics.maxCollisionPenetration < oldCollision.maxPenetration) _metrics.maxCollisionPenetration = oldCollision.maxPenetration;
			if (_metrics.maxTimeSpentInCollision < oldCollision.timeDuration) _metrics.maxTimeSpentInCollision = oldCollision.timeDuration;
		}
	}
}


void AgentMetricsCollector::_updateCollisionStats(SpatialDataBaseInterface * gridDB, AgentInterface * updatedAgent, float currentTimeStamp)
{
	//
	// check for collisions with other agents and obstacles.
	//
	// when analyzing a recording, the spatial database will be populated with AgentMetricsCollector objects instead of agents.
	//

	std::set<SpatialDatabaseItemPtr> neighbors;
	std::set<SpatialDatabaseItemPtr>::iterator neighbor;
	gridDB->getItemsInRange(neighbors, _currentPosition.x - _agentBeingAnalyzed->radius(), _currentPosition.x + _agentBeingAnalyzed->radius(), _currentPosition.z - _agentBeingAnalyzed->radius(), _currentPosition.z + _agentBeingAnalyzed->radius(), updatedAgent);


	for (neighbor = neighbors.begin(); neighbor != neighbors.end(); ++neighbor) {
		
		// this way, collisionKey will be unique across all objects in the spatial database.

		// MUBBASIR -- THIS CAUSES A COMPILE PROBLEM ON HAMMERHEAD
		//unsigned int collisionKey = reinterpret_cast<unsigned int>((*neighbor));
		//int collisionKey = reinterpret_cast<uintptr_t >((*neighbor));
		uintptr_t collisionKey = (uintptr_t)(*neighbor);

		// this crashes with 2 agents 
		//unsigned int* collisionKeyptr = reinterpret_cast<unsigned int*>((*neighbor));
        //unsigned int collisionKey = *collisionKeyptr;
        //delete collisionKeyptr;		

		float penetration = 0.0f;
		penetration = (*neighbor)->computePenetration(_currentPosition, _radius);

		// check for a collision, and update stats if appropriate.
		_checkAndUpdateOneCollision(collisionKey, penetration, currentTimeStamp);
	}
}


void AgentMetricsCollector::_updateAgentInformation(SteerLib::AgentInterface * updatedAgent)
{
	if (_agentBeingAnalyzed != updatedAgent) {
		std::cerr << "WARNING: AgentMetricsCollector received a different AgentInterface pointer for updating than the one given during initialization.  This is unexpected and may be a bug in the way the metrics collector is used.";
	}

	// if already disabled, don't update anything here.
	if (!_enabled)
		return;

	_enabled = updatedAgent->enabled();
	_radius = updatedAgent->radius();
	_previousPosition = _currentPosition;
	_previousDirection = _currentDirection;
	_currentPosition = updatedAgent->position();
	_currentDirection = updatedAgent->forward();
}



void AgentMetricsCollector::update(SpatialDataBaseInterface * gridDB, SteerLib::AgentInterface * updatedAgent, float currentTimeStamp, float timePassedSinceLastFrame)
{
	// this function should not be called if agent is disabled.
	// std::cout << "collecting metrics for agent " << updatedAgent << " enabled " << updatedAgent->enabled() << std::endl;
	assert(updatedAgent->enabled());

	_updateAgentInformation(updatedAgent);

	_numFramesMeasured++;

	if (timePassedSinceLastFrame == 0.0f)
	{
		throw GenericException("timePassedSinceLastFrame should not be 0.");
	}

	_metrics.totalNumFramesEnabled++;
	_metrics.totalTimeEnabled += timePassedSinceLastFrame;

	// update collision statistics for this frame
	_updateCollisionStats(gridDB, updatedAgent, currentTimeStamp);


	Vector changeInPosition = _currentPosition - _previousPosition;                        // units = meters
	Vector instantaneousVelocity = changeInPosition / timePassedSinceLastFrame;            // units = meters/second
	float distanceTraveledSinceLastFrame = changeInPosition.length();                      // units = meters
	_metrics.instantaneousSpeed = distanceTraveledSinceLastFrame / timePassedSinceLastFrame;  // units = meters/second


	// the following values will be computed further below
	// assume that "nothing changed" for default values
	float angleTurnedSinceLastFrame = 0.0f;                     // units = degrees
	float changeInSpeedSinceLastFrame = 0.0f;                   // units = meters/second  ..... *NOT* meters/(second^2)
	Vector changeInVelocitySinceLastFrame(0.0f, 0.0f, 0.0f);    // units = meters/second  ..... *NOT* meters/(second^2)
	_metrics.instantaneousChangeInSpeed = 0.0f;                    // units = meters/(second^2)
	_metrics.instantaneousAcceleration = Vector(0.0f, 0.0f, 0.0f); // units = meters/(second^2)
	_metrics.instantaneousAngularSpeed = 0.0f;                     // units = decress/second
	if ((_currentDirection.length() != 0.0f) && (_previousDirection.length() != 0.0f)) {
		_currentDirection = normalize(_currentDirection);
		_previousDirection = normalize(_previousDirection);
// #define _DEBUG 1
#ifdef _DEBUG1
	std::cout << "_currentDirection: " << _currentDirection << ", previousDirections: " << _previousDirection << "\n";
#endif
		float cosTheta = dot(_currentDirection,_previousDirection);
		if (fabsf(cosTheta) < 1.0f) {
			// at cosTheta==1.0f, acosf() is undefined.
			angleTurnedSinceLastFrame = 180.0f * acosf(cosTheta) / M_PI;
		}
		else if (cosTheta <= -1.0f) {
			// NOTE CAREFULLY THE <= sign --> this is to avoid floating point error incorrectness
			angleTurnedSinceLastFrame = 180.0f; // the agent flipped around completely
		}
		else if (cosTheta >= 1.0f) {
			// NOTE CAREFULLY THE >= sign --> this is to avoid floating point error incorrectness
			angleTurnedSinceLastFrame = 0.0f; // the agent did not change direction since last frame.
		}
		else {
			std::cerr << "INTERNAL ERROR: did not expect to reach here in the code: " << __FILE__ << ", line " << __LINE__ << "\n";
			std::cerr << "cosTheta should be between -1.0 and 1.0... its value is " << std::setprecision(10) << cosTheta << "\n";
			std::cerr << "Most likely your AI just did a 180 in one timestep." << "\n"; // Glen
			
			// MUBBASIR TODO -- SHOULD NOT BE DOING THIS -- CORY ??! !!! 
			
			//assert(false);
		}
	} else {
		angleTurnedSinceLastFrame = 0.0f;
		//std::cerr << "TODO, should throw an exception here, zero-value directions are not allowed because it results in inaccurate benchmarking.\n";
		// this error can be a source of cheating on benchmarks, too
		//exit(1);
	}

	_metrics.instantaneousAngularSpeed = angleTurnedSinceLastFrame / timePassedSinceLastFrame;


	if (_numFramesMeasured < WINDOW_SIZE) {
		// in this case, the history windows have not yet been filled
		// simply update the Agent's windows, don't analyze them until we have a full window.
		_positionWindow[_numFramesMeasured] = _currentPosition;
		_distanceWindow[_numFramesMeasured] = distanceTraveledSinceLastFrame;
		_turnWindow[_numFramesMeasured]     = angleTurnedSinceLastFrame;
		_velocityWindow[_numFramesMeasured] = instantaneousVelocity;
		if (_numFramesMeasured > 1) {
			changeInSpeedSinceLastFrame = fabsf((_velocityWindow[_numFramesMeasured]).length() - (_velocityWindow[_numFramesMeasured-1]).length());
			changeInVelocitySinceLastFrame = _velocityWindow[_numFramesMeasured] - _velocityWindow[_numFramesMeasured-1];
		}
	} else {
		_positionWindow.advanceByOne(_currentPosition);
		_distanceWindow.advanceByOne(distanceTraveledSinceLastFrame);
		_turnWindow.advanceByOne(angleTurnedSinceLastFrame);
		_velocityWindow.advanceByOne(instantaneousVelocity);

		// analyze the window histories
		_metrics.totalDistanceTraveledOverWindow = 0.0f;
		_metrics.totalDegreesTurnedOverWindow = 0.0f;
		_metrics.numTimesVelocityChangedSignOverWindow = 0;
		for (int i=0; i<WINDOW_SIZE; i++) {
			_metrics.totalDistanceTraveledOverWindow += _distanceWindow[i];
			_metrics.totalDegreesTurnedOverWindow += _turnWindow[i];
			if ( (i>0) && (dot(_velocityWindow[i-1],_velocityWindow[i]) < 0.0f) ) {
				_metrics.numTimesVelocityChangedSignOverWindow++;
			}
		}
		if (_metrics.maxDistanceTraveledOverWindow < _metrics.totalDistanceTraveledOverWindow) _metrics.maxDistanceTraveledOverWindow = _metrics.totalDistanceTraveledOverWindow;
		if (_metrics.minDistanceTraveledOverWindow > _metrics.totalDistanceTraveledOverWindow) _metrics.minDistanceTraveledOverWindow = _metrics.totalDistanceTraveledOverWindow;
		if (_metrics.maxDegreesTurnedOverWindow  < _metrics.totalDegreesTurnedOverWindow) _metrics.maxDegreesTurnedOverWindow  = _metrics.totalDegreesTurnedOverWindow;
		if (_metrics.minDegreesTurnedOverWindow  > _metrics.totalDegreesTurnedOverWindow) _metrics.minDegreesTurnedOverWindow  = _metrics.totalDegreesTurnedOverWindow;

		changeInSpeedSinceLastFrame = fabsf((_velocityWindow[WINDOW_SIZE-1]).length() - (_velocityWindow[WINDOW_SIZE-2]).length());
		changeInVelocitySinceLastFrame = _velocityWindow[WINDOW_SIZE-1] - _velocityWindow[WINDOW_SIZE-2];
	}
	_metrics.instantaneousChangeInSpeed = changeInSpeedSinceLastFrame / timePassedSinceLastFrame;
	_metrics.instantaneousAcceleration = changeInVelocitySinceLastFrame / timePassedSinceLastFrame;

	if (_numFramesMeasured < WINDOW_SIZE) {
		// TIME DEPENDENT?  TODO: why did you use time-dependent here?  it should have been an integral?
		//_changeInSpeedWindow[_numFramesMeasured] = _metrics.instantaneousChangeInSpeed;
		//_accelerationWindow[_numFramesMeasured] = _metrics.instantaneousAcceleration.length();
		_changeInSpeedWindow[_numFramesMeasured] = changeInSpeedSinceLastFrame;
		_accelerationWindow[_numFramesMeasured] = changeInVelocitySinceLastFrame.length();
		_instantaneousAccelerationWindow[_numFramesMeasured] = _metrics.instantaneousAcceleration;
	} else {
		// TIME DEPENDENT?  TODO: why did you use time-dependent here?  it should have been an integral?
		//_changeInSpeedWindow.advanceByOne(_metrics.instantaneousChangeInSpeed);
		//_accelerationWindow.advanceByOne(_metrics.instantaneousAcceleration.length());
		_changeInSpeedWindow.advanceByOne(changeInSpeedSinceLastFrame);
		_accelerationWindow.advanceByOne(changeInVelocitySinceLastFrame.length());
		_instantaneousAccelerationWindow.advanceByOne(_metrics.instantaneousAcceleration);

		// analyze the window histories
		_metrics.totalChangeInSpeedOverWindow = 0.0f;
		_metrics.totalAccelerationOverWindow = 0.0f;
		_metrics.numTimesAccelerationChangedSignOverWindow = 0;
		for (int i=0; i<WINDOW_SIZE; i++) {
			_metrics.totalChangeInSpeedOverWindow += _changeInSpeedWindow[i];
			_metrics.totalAccelerationOverWindow += _accelerationWindow[i];

			if ( (i>0) && (dot(_instantaneousAccelerationWindow[i-1],_instantaneousAccelerationWindow[i]) < 0.0f) ) {
				_metrics.numTimesAccelerationChangedSignOverWindow++;
			}
		}
		if (_metrics.maxChangeInSpeedOverWindow < _metrics.totalChangeInSpeedOverWindow) _metrics.maxChangeInSpeedOverWindow = _metrics.totalChangeInSpeedOverWindow;
		if (_metrics.minChangeInSpeedOverWindow > _metrics.totalChangeInSpeedOverWindow) _metrics.minChangeInSpeedOverWindow = _metrics.totalChangeInSpeedOverWindow;
		if (_metrics.maxAccelerationOverWindow  < _metrics.totalAccelerationOverWindow)  _metrics.maxAccelerationOverWindow  = _metrics.totalAccelerationOverWindow;
		if (_metrics.minAccelerationOverWindow  > _metrics.totalAccelerationOverWindow)  _metrics.minAccelerationOverWindow  = _metrics.totalAccelerationOverWindow;
	}



	// WARNING!!! THESE ASSUME THAT WE HAVE A 60-FRAME WINDOW THAT IS 3 SECONDS INTERVAL (20 FPS).
	_metrics.avgAngularSpeedOverWindow = _metrics.totalDegreesTurnedOverWindow / 3.0f;
	_metrics.avgSpeedOverWindow = _metrics.totalDistanceTraveledOverWindow / 3.0f;
	_metrics.avgChangeInSpeedOverWindow = _metrics.totalChangeInSpeedOverWindow / 3.0f;
	_metrics.avgAccelerationOverWindow = _metrics.totalAccelerationOverWindow / 3.0f;
	_metrics.displacementOverWindow = (_positionWindow[WINDOW_SIZE-1] - _positionWindow[0]).length();



	// add to the total distance traveled
	_metrics.totalDistanceTraveled +=  changeInPosition.length();

	// update the max instantaneous speed
	if (_metrics.maxInstantaneousSpeed < _metrics.instantaneousSpeed) _metrics.maxInstantaneousSpeed = _metrics.instantaneousSpeed;

	// add to the total angle turned
	_metrics.totalDegreesTurned += angleTurnedSinceLastFrame;

	// update max angular speed
	if (_metrics.maxAngularSpeed < _metrics.instantaneousAngularSpeed) _metrics.maxAngularSpeed = _metrics.instantaneousAngularSpeed;

	// update total change in speed
	// TODO: why did you have instantaneous here?  that is time dependent and not really the integral total
	//_metrics.totalChangeInSpeed += _metrics.instantaneousChangeInSpeed;
	_metrics.totalChangeInSpeed += changeInSpeedSinceLastFrame;

	// update max change in speed
	if (_metrics.maxChangeInSpeed < _metrics.instantaneousChangeInSpeed)
	{
		_metrics.maxChangeInSpeed = _metrics.instantaneousChangeInSpeed;
	}

	// update the total acceleration
	// the "totalAcceleration" is the integral of velocity over a window;
	_metrics.sumTotalOfInstantaneousAcceleration += _metrics.instantaneousAcceleration.length();
	_metrics.totalAcceleration += changeInVelocitySinceLastFrame.length();

	// update the total kinetic energy
	// NOTE CAREFULLY: physically correct kinetic energy should include angular energy expeniture as well.
	// However, in AI, we do not want to unfairly judge against algorithms that instantly flip directions, 
	// causing a huge angular velocity.  Therefore, we do not include it.  If you do include it, then
	// ANGULAR_PRIORITY could be scaled to limit the amount of contribution that turning makes to total kinetic energy.
	// ******** TODO: ******** this is time-dependent, and therefore probably not correct?
	// ******** TODO: ******** this is time-dependent, and therefore probably not correct?
	// ******** TODO: ******** this is time-dependent, and therefore probably not correct?
	// ******** TODO: ******** this is time-dependent, and therefore probably not correct?
	_metrics.sumTotalOfInstantaneousKineticEnergies += (0.5f * instantaneousVelocity.lengthSquared());
		//+ ANGULAR_PRIORITY * (0.5f * _radius * _radius * (instantaneousAngularSpeed * M_PI / 180.0f)*(instantaneousAngularSpeed * M_PI / 180.0f));

	// update max instantaneous acceleration
	if (_metrics.maxAcceleration < _metrics.instantaneousAcceleration.length()) _metrics.maxAcceleration = _metrics.instantaneousAcceleration.length();

	// MUBBASIR COMPUTING PLE (mass = ?)
	// Glen Assume uniform mass for unbias comparison of steering algorithms
	_metrics.pleEnergy +=  MASS * (E_S + E_W * instantaneousVelocity.lengthSquared()) * timePassedSinceLastFrame;


	// update "effort" metrics:
	float tempAverageMomentum;
	tempAverageMomentum = _metrics.totalDistanceTraveled / _metrics.totalTimeEnabled;

	_metrics.instantaneousKineticEnergy = 0.5f * _metrics.instantaneousSpeed * _metrics.instantaneousSpeed;
	_metrics.integralOfKineticEnergy += 2.0f * _metrics.instantaneousKineticEnergy *  timePassedSinceLastFrame;

	_metrics.averageKineticEnergy = 0.5f * _metrics.integralOfKineticEnergy / _metrics.totalTimeEnabled;

	// collision statistics and window statistics for distance, turning, speed change, and acceleration are actually updated above.

	// std::cout << "This thing get to have all the fun, position size: " << _positionWindow.size() << std::endl;

}



/// @todo
///   - double-check if this is dumping out all metrics in a reasonable grouping/order.
///   - merge "overall statistics" and "current statistics" so they are both dumped out.
void AgentMetricsCollector::printFormattedCurrentStatistics(std::ostream & out)
{
	// prints instantaneous and window statistics which are valid for a current frame

	out << "           instantaneous angular speed: " << _metrics.instantaneousAngularSpeed << "\n";
	out << "      total degrees turned over window: " << _metrics.totalDegreesTurnedOverWindow << "\n";
	out << "     average angular speed over window: " << _metrics.avgAngularSpeedOverWindow << "\n";
	//out << "     # times ang speed +/- over window: " << _metrics.numTimesAngularSpeedChangedSignOverWindow << "\n";

	out << "                   instantaneous speed: " << _metrics.instantaneousSpeed << "\n";
	out << "   total distance traveled over window: " << _metrics.totalDistanceTraveledOverWindow << "\n";
	out << "             average speed over window: " << _metrics.avgSpeedOverWindow << "\n";

	out << "         instantaneous change in speed: " << _metrics.instantaneousChangeInSpeed << "\n";
	out << "     total change in speed over window: " << _metrics.totalChangeInSpeedOverWindow << "\n";
	out << "   average change in speed over window: " << _metrics.avgChangeInSpeedOverWindow << "\n";

	out << "            instantaneous acceleration: " << _metrics.instantaneousAcceleration << "\n";
	out << "        total acceleration over window: " << _metrics.totalAccelerationOverWindow << "\n";
	out << "      average acceleration over window: " << _metrics.avgAccelerationOverWindow << "\n";
	out << "        # times veloc. +/- over window: " << _metrics.numTimesVelocityChangedSignOverWindow << "\n";
	out << "        # times accel. +/- over window: " << _metrics.numTimesAccelerationChangedSignOverWindow << "\n";

	out << "              displacement over window: " << _metrics.displacementOverWindow << "\n";

	out << std::endl;
}


void AgentMetricsCollector::printFormattedOverallStatistics(std::ostream & out)
{
	// prints totals, max, and mins, which are valid after the analysis finished.
	// TODO: add "total averages", too, but they don't need to be stored in metrics, they can be computed on the fly?

	//out << "------ Agent " << _agentIndex << " ------\n";
	out << "         total number of frames active: " << _metrics.totalNumFramesEnabled << "\n";
	out << "                     total time active: " << _metrics.totalTimeEnabled << "\n";

	out << "                       collision score: " << _metrics.collisionScore << "\n";
	out << "             max collision penetration: " << _metrics.maxCollisionPenetration << " (not implemented yet)\n";
	out << "           max time in collision state: " << _metrics.maxTimeSpentInCollision << " (not implemented yet)\n";

	out << "                  total degrees turned: " << _metrics.totalDegreesTurned << "\n";
	out << "                     max angular speed: " << _metrics.maxAngularSpeed << "\n";
	out << "               max turning in a window: " << _metrics.maxDegreesTurnedOverWindow << "\n";
	out << "               min turning in a window: " << _metrics.minDegreesTurnedOverWindow << "\n";

	out << "               total distance traveled: " << _metrics.totalDistanceTraveled << "\n";
	out << "               max instantaneous speed: " << _metrics.maxInstantaneousSpeed << "\n";
	out << "       max distance traveled in window: " << _metrics.maxDistanceTraveledOverWindow << "\n";
	out << "       min distance traveled in window: " << _metrics.minDistanceTraveledOverWindow << "\n";

	out << "                    total speed change: " << _metrics.totalChangeInSpeed << "\n";
	out << "                      max speed change: " << _metrics.maxChangeInSpeed << "\n";
	out << "            max speed change in window: " << _metrics.maxChangeInSpeedOverWindow << "\n";
	out << "            min speed change in window: " << _metrics.minChangeInSpeedOverWindow << "\n";

	out << "         total (integral) acceleration: " << _metrics.totalAcceleration << "\n";
	out << "    sum of instantaneous accelerations: " << _metrics.sumTotalOfInstantaneousAcceleration << "\n";
	out << "        max instantaneous acceleration: " << _metrics.maxAcceleration << "\n";
	out << "            max acceleration in window: " << _metrics.maxAccelerationOverWindow << "\n";
	out << "            min acceleration in window: " << _metrics.minAccelerationOverWindow << "\n";
	out << " sum of instantaneous kinetic energies: " << _metrics.sumTotalOfInstantaneousKineticEnergies << "\n";

	out << "    total (integral) of kinetic energy: " << _metrics.integralOfKineticEnergy << "\n";
	out << "                average kinetic energy: " << _metrics.averageKineticEnergy << "\n";

	out << std::endl;
}



