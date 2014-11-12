//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file Clock.cpp
/// @brief Implements the SteerLib::Clock class.
///
/// @todo
///   - document this file
///   - fix coding style
///   - options are accessible now, is there anything to change because of this?

#include "util/GenericException.h"
#include "util/HighResCounter.h"
#include "util/Misc.h"
#include "simulation/Clock.h"

using namespace SteerLib;
using namespace Util;



Clock::Clock()
{
	// default to a fixed frame rate of 20 fps, running as fast as possible.
	setClockMode(CLOCK_MODE_FIXED_AS_FAST_AS_POSSIBLE, 20.0f, 0.01f, 0.1f);
	reset();
}

Clock::Clock(ClockModeEnum clockMode, float fixedFps, float minSimulationDt, float maxSimulationDt)
{
	setClockMode(clockMode, fixedFps, minSimulationDt, maxSimulationDt);
	reset();
}

Clock::~Clock()
{
}

void Clock::reset()
{
	_baseTick = 0;
	_totalRealTime = 0;
	_realDt = 0;
	_totalSimulationTime = 0;
	_simulationDt = 0;
	_realFrameNumber = 0;
	_simulationFrameNumber = 0; // frame 0 contains the initial conditions, so modules and agents will never see that.  The first number modules and agents will receive is 1.
	_measuredFps = 0;
	_inverseFrequency = 1.0f/((float)Util::getHighResCounterFrequency());
}


void Clock::advanceSimulationAndUpdateRealTime()
{

	// 1. if fixed-real-time mode, then try to wait until real-time matches the clock
	if (_clockMode == CLOCK_MODE_FIXED_REAL_TIME) {
		_waitForFrameSync(_fixedTicksPerFrame);
	}
	else if (_clockMode == CLOCK_MODE_VARIABLE_REAL_TIME) {
		_waitForFrameSync(_minSimulationDt);  // will not wait if elapsed time was already more than this.
	}

	// 2. update real-time ( NOTE: _realDt includes any extra delay due to _waitForFrameSync(). )
	_realDt = _getTickCount() - _totalRealTime;
	_totalRealTime += _realDt;

	// 2. compute the next simulation dt
	if ((_clockMode == CLOCK_MODE_FIXED_AS_FAST_AS_POSSIBLE) || (_clockMode == CLOCK_MODE_FIXED_REAL_TIME)) {
		_simulationDt = _fixedTicksPerFrame;
	}
	else if (_clockMode == CLOCK_MODE_VARIABLE_REAL_TIME) {
		// _simulationDt will match _realDt, but it will be "clamped" to remain between _mainSimulationDt and _maxSimulationDt.
		_simulationDt = (_realDt < _maxSimulationDt) ? _realDt : _maxSimulationDt;
		_simulationDt = (_simulationDt > _minSimulationDt) ? _simulationDt : _minSimulationDt;
	}

	// 3. update total simulation time based on the new simulation dt
	_totalSimulationTime += _simulationDt;


	// 5. measure fps
	_updateFpsMeasurement();

	_simulationFrameNumber++;
	_realFrameNumber++;

}

//For use with the Authoring toolkit to go back and forth in time.
/**
 * Only work for fix-fast timer
 */
void Clock::backupSimulationAndUpdateRealTime()
{
	// 2. update real-time ( NOTE: _realDt includes any extra delay due to _waitForFrameSync(). )
	// This part might not workw so well.
	_realDt = _getTickCount() - _totalRealTime;
	_totalRealTime -= _realDt;

	// 2. compute the next simulation dt
	if ((_clockMode == CLOCK_MODE_FIXED_AS_FAST_AS_POSSIBLE) ||
			(_clockMode == CLOCK_MODE_FIXED_REAL_TIME)) {
		_simulationDt = _fixedTicksPerFrame;
	}
	else if (_clockMode == CLOCK_MODE_VARIABLE_REAL_TIME) {
		// _simulationDt will match _realDt, but it will be "clamped" to remain between _mainSimulationDt and _maxSimulationDt.
		_simulationDt = (_realDt < _maxSimulationDt) ? _realDt : _maxSimulationDt;
		_simulationDt = (_simulationDt > _minSimulationDt) ? _simulationDt : _minSimulationDt;
	}

	// 3. update total simulation time based on the new simulation dt
	_totalSimulationTime -= _simulationDt;


	// 5. measure fps
	// This will also probably not work so well.
	_updateFpsMeasurement();

	_simulationFrameNumber--;
	_realFrameNumber--;

}

void Clock::setClockMode(ClockModeEnum clockMode, float fixedFps, float minSimulationDt, float maxSimulationDt)
{
	_clockMode = clockMode;
	if ((fixedFps == 0.0f) || (fixedFps <= 0.0f)) {
		throw Util::GenericException("Invalid fixed frame rate (" + Util::toString(fixedFps) + " fps) requested. Frame rate must be positive and larger than zero.");
	}
	_fixedSimulationFrameRate = fixedFps;
	_fixedTicksPerFrame = (unsigned long long)(((float)getHighResCounterFrequency()) / fixedFps);

	// minSimulationDt and maxSimulationDt are input as seconds, but internally stored as ticks.
	_minSimulationDt = (unsigned long long)(((float)getHighResCounterFrequency()) * minSimulationDt);
	_maxSimulationDt = (unsigned long long)(((float)getHighResCounterFrequency()) * maxSimulationDt);
}


void Clock::_updateFpsMeasurement()
{
	if(_realDt > 0.f)
		_measuredFps = (1.5f + (1.f  -  1.5f*_counterTicksToSeconds(_realDt)) * _measuredFps); 
}


void Clock::_waitForFrameSync(const unsigned long long & minDesiredTicks)
{

	// tempDt is the time since last update.
	// we only insert a delay if that time was shorter than the desired fixed ticks per frame.
	//
	unsigned long long tempDt = _getTickCount() - _totalRealTime;
	if (tempDt < minDesiredTicks) {
		// figure out when to wake up
		unsigned long long targetTime = _totalRealTime + minDesiredTicks - tempDt;

		// TODO: set up a better non-busy wait, as long as it uses accurate timing.
		// busy-wait
		while(_getTickCount() < targetTime) {
			// do nothing
		}
	}
}


