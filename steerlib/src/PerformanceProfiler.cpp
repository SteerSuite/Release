//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file PerformanceProfiler.cpp
/// @brief Implements the Util::PerformanceProfiler class.

#include "util/HighResCounter.h"
#include "util/PerformanceProfiler.h"
#include <iostream>
#include <math.h>

#define MILLS 1000.0f

using namespace Util;

PerformanceProfiler::PerformanceProfiler()
{
	reset();
}

void PerformanceProfiler::reset()
{
	_startTick = 0;
	_endTick = 0;
	_totalTicksAccumulated = 0;
	_maxTicks = 0;
	_minTicks = 0;
	_numTimesCalled = 0;
	_minTime = 0.0f; 
	_maxTime = 0.0f;
	_averageTime = 0.0f;
	_totalTime = 0.0f;
	_tickFrequencyInHz = ((float)getHighResCounterFrequency());
	isStopped = true;
}

long long PerformanceProfiler::getNumTimesExecuted()
{
	return _numTimesCalled;
}

float PerformanceProfiler::getMinExecutionTime()
{
	_minTime =  ( (float)_minTicks / _tickFrequencyInHz);
	return _minTime;
}

float PerformanceProfiler::getMaxExecutionTime()
{
	_maxTime =  ( (float)_maxTicks / _tickFrequencyInHz );
	return _maxTime;
}

float PerformanceProfiler::getAverageExecutionTime()
{
	if (_numTimesCalled == 0) return 0.0f;
	_averageTime =  ( ( (float)_totalTicksAccumulated / (float)_numTimesCalled ) / _tickFrequencyInHz );
	return _averageTime;
}

float PerformanceProfiler::getTotalTime()
{
	_totalTime =  ( (float)_totalTicksAccumulated / _tickFrequencyInHz );
	return _totalTime;
}

long long PerformanceProfiler::getTotalTicksAccumulated()
{
	return _totalTicksAccumulated;
}

long long PerformanceProfiler::getMinTicks()
{
	return _minTicks;
}

long long PerformanceProfiler::getMaxTicks()
{
	return _maxTicks;
}

// In MHz
float PerformanceProfiler::getTickFrequency()
{
	return (_tickFrequencyInHz / 1000000.0f);
}

float PerformanceProfiler::getMinExecutionTimeMills()
{
	return getMinExecutionTime() * MILLS;
}

float PerformanceProfiler::getMaxExecutionTimeMills()
{
	return getMaxExecutionTime() * MILLS;
}
float PerformanceProfiler::getAverageExecutionTimeMills()
{
	return getAverageExecutionTime() * MILLS;
}


void PerformanceProfiler::displayStatistics(std::ostream & out)
{
	out << "   Number of times executed: " << getNumTimesExecuted() << std::endl;
	out << "    Total ticks accumulated: " << getTotalTicksAccumulated() << std::endl;
	out << " Shortest execution (ticks): " << getMinTicks() << std::endl;
	out << "  Longest execution (ticks): " << getMaxTicks() << std::endl;
	out << "       Tick Frequency (MHz): " << getTickFrequency() << std::endl;
	out << "          Fastest execution: " << getMinExecutionTimeMills() << " milliseconds" << std::endl;
	out << "          Slowest execution: " << getMaxExecutionTimeMills() << " milliseconds" << std::endl;
	out << "      Average time per call: " << getAverageExecutionTimeMills() <<  " milliseconds" << std::endl;
	out << "    Total time of all calls: " << getTotalTime() << " seconds" << std::endl;
}

void PerformanceProfiler::_updateStatistics()
{
	unsigned long long ticksForOneStep = _endTick - _startTick;

	_numTimesCalled++;

	if ( ticksForOneStep > _maxTicks )
		_maxTicks = ticksForOneStep;

	if ( ticksForOneStep < _minTicks || _minTicks == 0)
		_minTicks = ticksForOneStep;

	_totalTicksAccumulated += ticksForOneStep;
}
