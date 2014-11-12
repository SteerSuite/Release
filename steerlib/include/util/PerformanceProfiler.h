//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_PERFORMANCE_PROFILER_H__
#define __UTIL_PERFORMANCE_PROFILER_H__

/// @file PerformanceProfiler.h
/// @brief Declares some helpful performance logging functionality.

#include <ostream>
#include "Globals.h"
#include "util/HighResCounter.h"

namespace Util {

	/**
	 * @brief A generic profiler that measures performance statistics for blocks of code.
	 *
	 * This class can be used to profile the performance of a block of code.
	 *
	 * To use this class, use #start() at the beginning of the block you wish to profile,
	 * and use #stop() at the end of the block.
	 *
	 */
	class UTIL_API PerformanceProfiler
	{
	public:
		/// Calls reset() indirectly.
		PerformanceProfiler();
		/// Resets the profiler
		void reset();
		/// Marks the beginning of a block of code you want to profile
		inline void start() { isStopped = false; _startTick = getHighResCounterValue(); }
		/// Marks the end of a block of code you want to profile
		inline void stop() { _endTick = getHighResCounterValue();  _updateStatistics(); isStopped = true; }

		/// Returns the number of times the block(s) of code being profiled is called.
		long long getNumTimesExecuted();
		/// Returns the fastest single execution time of the block of code being profiled, measured in seconds.
		float getMinExecutionTime();
		/// Returns the slowest single execution time of the block of code being profiled, measured in seconds.
		float getMaxExecutionTime();
		/// Returns the average execution time of the block of code being profiled, measured in seconds.
		float getAverageExecutionTime();
		/// Returns the total time (in seconds) spent in the block of code being profiled.
		float getTotalTime();
		// Returns the total number of ticks spent in the block of code.
		long long getTotalTicksAccumulated();
		// Returns the min number of ticks for a call
		long long getMinTicks();
		// Returns the max number of ticks for a call.
		long long getMaxTicks();
		// Returns the tick frequency in MHz.
		float getTickFrequency();

		float getMinExecutionTimeMills();
		float getMaxExecutionTimeMills();
		float getAverageExecutionTimeMills();

		/// Outputs a human-readable form of these statistics.
		void displayStatistics(std::ostream & out);
		/// Checks if the timer is started or not
		inline bool stopped(void) { return isStopped; }

	private:
		void _updateStatistics();

		// running internal state
		unsigned long long _startTick;
		unsigned long long _endTick;
		unsigned long long _totalTicksAccumulated;
		unsigned long long _minTicks;
		unsigned long long _maxTicks;
		float _tickFrequencyInHz;
		bool isStopped;

		// statistics:
		long long _numTimesCalled;
		float _minTime, _maxTime, _averageTime, _totalTime;
	};


	/**
	 * @brief A wrapper class that uses the PerformanceProfiler to profile a function with only one line of code.
	 *
	 * This class can be used to profile a function using only one line of code.
	 * Simply instantiate the class where you would like to begin profiling the function. (This
	 * should not be a pointer that you allocate explicitly, but a direct instance that gets
	 * placed on the stack.)  When the function exits, this function's destructor gets called 
	 * automatically.  The constructor essentially calls PerformanceProfiler::start(),
	 * and the destructor essentially calls PerformanceProfiler::stop().
	 *
	 * An added advantage to using this wrapper is being less prone to error.  Without this, you 
	 * would need to place a call to PerformanceProfiler::stop() on each code path that returns.
	 * With this class, PerformanceProfiler::stop() is automatically invoked when the function 
	 * returns, no matter where the function returns from.
	 *
	 */
	class UTIL_API AutomaticFunctionProfiler
	{
	public:
		AutomaticFunctionProfiler(PerformanceProfiler * pp) { _pp = pp; _pp->start(); }
		~AutomaticFunctionProfiler() { _pp->stop(); }
	private:
		PerformanceProfiler * _pp;
	};

} // end namespace Util

#endif
