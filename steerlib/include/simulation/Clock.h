//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_CLOCK_H__
#define __STEERLIB_CLOCK_H__

/// @file Clock.h
/// @brief Declares a clock object to keep track of real time and simulation time
///
/// @todo 
///  - update documentation in this file.
///  - fix the coding style in this file.

#include <algorithm>
#include <math.h>
#include "Globals.h"

#include "util/HighResCounter.h"

#ifndef _WIN32
// win32 does not define "std::max", instead they define "max" as a macro.
// because of this, on unix we use "using std::max" so that the code only 
// needs to use "max()" instead of "std::max()".  This way, the code
// works on both win32 and unix.
using std::max;
#endif


namespace SteerLib {

	/**
	 * @brief A clock that maintains real-time and simulation-time separately.
	 *
	 * Uses Util::getHighResCounterValue() and Util::getHighResCounterFrequency() to perform timing-related functions.
	 * Make sure to read the documentation of those functions for more details about the limitations and issues of 
	 * platform-independent timing code.
	 *
	 * There are 4 operating modes for the clock:
	 *
	 *  -# As-fast-as-possible fixed stepping:
	 *     Each simulation frame has a constant time step of 1/(frames-per-second).
	 *     The real-time performance, however, is not constrained, and so the 
	 *     simulation will run as fast as possible.
	 *     To enable this mode:
	 *       - <code>setSimulationClockMode( FIXED_FRAME_RATE );</code>
	 *       - <code>setRealTimeClockMode( AS_FAST_AS_POSSIBLE );</code>
	 *       - <code>setFixedFrameRate( desired-frame-rate) )</code>
	 *  -# Real-time fixed stepping
	 *     Each simulation frame has a fixed time step of 1/(fixed frame rate),
	 *     and the real-time performance is constrained to have the same frame rate.
	 *     To enable this mode:
	 *       - <code>setSimulationClockMode( FIXED_FRAME_RATE );</code>
	 *       - <code>setRealTimeClockMode( TRY_TO_MATCH_SIMULATION_CLOCK );</code>
	 *       - <code>setFixedFrameRate( desired-frame-rate )</code>
	 *  -# Real-time variable stepping
	 *     Both the time step of a frame and the frame rate are variable.
	 *     However, the constraint <code>frames-per-second * time-step = 1.0</code>
	 *     is imposed, so the simulation works in real-time.
	 *     In this case, setting the fixed frame rate is not used, except for dddwhen stepping the clock while it is paused.
	 *     To enable this mode:
	 *       - <code>setSimulationClockMode( VARIABLE_FRAME_RATE );</code>
	 *       - <code>setRealTimeClockMode( don't-care );</code>
	 *       - <code>setFixedFrameRate( desired- )</code>
	 *  -# Paused
	 *
	 * setClockMode() is a convenience function that combines all three of these above functions for setting the clock.
	 *
	 * When the clock is paused, advanceOneStep() will still return, but the new time step (i.e. accessed using getSimulationDt())
	 * will be zero.
	 *//*
	class STEERLIB_API Clock
	{
	public:
		enum SimulationClockModeEnum { FIXED_FRAME_RATE,  VARIABLE_FRAME_RATE };
		enum RealTimeClockModeEnum { AS_FAST_AS_POSSIBLE, TRY_TO_MATCH_SIMULATION_CLOCK };

		Clock();
		Clock(RealTimeClockModeEnum realTimeClockMode, SimulationClockModeEnum simulationClockMode, float fpsIfFixedSimulationClock);
		~Clock();

		/// Resets the clock counters, but does not change its mode.
		void reset();
		/// Advances the clock by one step.
		void advanceOneStep();
		/// Advances one frame, but otherwise puts the clock in the paused state.
		void pauseAndStepOneFrame() { setPausedState(true); _isSingleStepping = true; }  // for advancing one frame at a time


		/// @name Accessor "get" functions
		//@{
		/// Returns the frame rate; Note this is only meaningful if the simulation clock mode is FIXED_FRAME_RATE.
		float getFixedFrameRate() { return _fixedSimulationFrameRate; }
		/// Returns the mode of the simulation clock
		SimulationClockModeEnum getSimulationClockMode() { return _simulationClockMode; }
		/// Returns the mode of the real-time clock
		RealTimeClockModeEnum getRealTimeClockMode() { return _realTimeClockMode; }
		/// Returns true if the simulation is paused, false if not paused.
		bool isPaused() { return _isPaused; }
		/// Returns the total <em>simulation</em> time elapsed from frame 0 to the current frame.
		float getCurrentSimulationTime() { return _systemTimeToSeconds(_totalSimulationTime); }
		/// Returns the <em>simulation</em> time-step of the last frame
		float getSimulationDt() { return _systemTimeToSeconds(_simulationDt); }
		/// Returns the current <em>real</em> time, which is also the total real-time elapsed since first call to getTickCount()
		float getCurrentRealTime() { return _systemTimeToSeconds(_totalRealTime); }
		/// Returns the total <em>real</em> time elapsed in real-time since last frame.
		float getRealDt() { return _systemTimeToSeconds(_realDt); }
		/// Returns the latest measurement of fps, smoothed over several frames for readability.
		float getSmoothedFps() { return _smoothedFps; } 
		/// Returns the current frame number; the first frame is frame 0 before advanceOneStep() is called.
		unsigned int getCurrentFrameNumber() { return _frameNumber; }
		//@}

		/// @name Accessor "set" functions
		//@{
		/// Pauses the simulation if state is true, un-pauses the simulation if state is false.
		void setPausedState(bool state) { _isPaused = state; }
		/// Sets the frame rate to be used when the user is interactively stepping through a simularion, and when the clock mode is FIXED_FRAME_RATE.
		void setFixedFrameRate(float framesPerSecond);
		/// Sets the simulation clock mode to either FIXED_FRAME_RATE or VARIABLE_FRAME_RATE.
		void setSimulationClockMode(SimulationClockModeEnum mode) { _simulationClockMode = mode; }
		/// Sets the real-time clock mode to either AS_FAST_AS_POSSIBLE or TRY_TO_MATCH_SIMULATION_CLOCK.
		void setRealTimeClockMode(RealTimeClockModeEnum mode) { _realTimeClockMode = mode; }
		/// A convenience function that sets the fixed frame rate, simulation clock mode, and real-time clock mode in one function call.
		void setClockMode(RealTimeClockModeEnum realTimeClockMode, SimulationClockModeEnum simulationClockMode, float fpsIfFixedSimulationClock) {
			setRealTimeClockMode(realTimeClockMode);
			setSimulationClockMode(simulationClockMode);
			setFixedFrameRate(fpsIfFixedSimulationClock);
		}
		//@}

	protected:
		void _waitForFrameSync();
		void _updateSmoothedFpsMeasurement();
		inline float _systemTimeToSeconds(unsigned long long ticks) { return (float)ticks / (float)Util::getHighResCounterFrequency(); }

		inline unsigned long long _getTickCount()
		{
			unsigned long long tick = Util::getHighResCounterValue();
			// if this is the first time that this function was called,
			// then we need to setup the zero reference for real time
			if(_baseTick == 0) {
				_baseTick = tick;
				return 0;
			}
			return    tick - _baseTick;
		}

		bool _isPaused;
		bool _isSingleStepping;
		bool _wasPausedOnPreviousFrame;
		SimulationClockModeEnum _simulationClockMode;
		RealTimeClockModeEnum _realTimeClockMode;
		float _fixedSimulationFrameRate;
		float _secondsPerFrame;
		float _smoothedFps;

		unsigned long long _baseTick;
		unsigned long long _totalRealTime;
		unsigned long long _realDt;
		unsigned long long _totalSimulationTime;
		unsigned long long _simulationDt;

		unsigned int _numUpdates;  // the number of times update() has been called
		unsigned int _frameNumber; // Starts at frame 0, but modules never see that, because those are initial conditions.  Also, NOT the same as numUpdates, because update() can be called when clock is paused or stepping.
	};
*/

	class STEERLIB_API Clock
	{
	public:
		enum ClockModeEnum { CLOCK_MODE_FIXED_AS_FAST_AS_POSSIBLE,  CLOCK_MODE_FIXED_REAL_TIME,  CLOCK_MODE_VARIABLE_REAL_TIME };

		Clock();
		Clock(ClockModeEnum clockMode, float fixedFps, float minSimulationDt, float maxSimulationDt);
		~Clock();

		/// Resets the clock counters, but does not change its mode.
		void reset();
		/// Advances the simulation and real-time clocks by one step.
		void advanceSimulationAndUpdateRealTime();
		/// BackTracks the simulation and real-time clocks by one step.
		void backupSimulationAndUpdateRealTime();
		/// Advances only real-time clock, but not the simulation clock.
		inline void updateRealTime() {
			_realDt = _getTickCount() - _totalRealTime;
			_totalRealTime += _realDt;
			_realFrameNumber++;
			_updateFpsMeasurement();
		}


		/// @name Accessor "get" functions
		//@{
		/// Returns the frame rate; Note this is only meaningful if the simulation clock mode is FIXED_FRAME_RATE.
		inline float getFixedFrameRate() { return _fixedSimulationFrameRate; }
		/// Returns the mode of the simulation clock
		inline ClockModeEnum getClockMode() { return _clockMode; }
		/// Returns the total <em>simulation</em> time elapsed from frame 0 to the current frame.
		inline float getCurrentSimulationTime() { return _counterTicksToSeconds(_totalSimulationTime); }
		/// Returns the <em>simulation</em> time-step of the last frame
		inline float getSimulationDt() { return _counterTicksToSeconds(_simulationDt); }
		/// Returns the current <em>real</em> time, which is also the total real-time elapsed since the clock was reset.
		inline float getCurrentRealTime() { return _counterTicksToSeconds(_totalRealTime); }
		/// Returns the total <em>real</em> time elapsed in real-time since last frame.
		inline float getRealDt() { return _counterTicksToSeconds(_realDt); }
		/// Returns the latest measurement of fps.
		inline float getRealFps() { return _measuredFps; }
		/// Returns the current frame number; the first frame is frame 0 before advanceOneStep() is called.
		inline unsigned int getCurrentFrameNumber() { return _simulationFrameNumber; }
		//@}

		/// @name Accessor "set" functions
		//@{
		/// Sets the frame rate to be used when the user is interactively stepping through a simularion, and when the clock mode is FIXED_FRAME_RATE.
		void setFixedFrameRate(float framesPerSecond);
		/// Sets the frame rate to be used when the user is interactively stepping through a simularion, and when the clock mode is FIXED_FRAME_RATE.
		void setMaxTimeBetweenFrames(float framesPerSecond);
		void setClockMode(ClockModeEnum clockMode, float fixedFps, float minSimulationDt, float maxSimulationDt);
		//@}

	protected:
		/// @name Protected helper functions
		//@{
		void _waitForFrameSync(const unsigned long long & minDesiredTicks);
		void _updateFpsMeasurement();
		inline float _counterTicksToSeconds(unsigned long long ticks) {
			return (float)ticks * _inverseFrequency;
		}

		inline unsigned long long _getTickCount()
		{
			unsigned long long tick = Util::getHighResCounterValue();
			// if this is the first time that this function was called,
			// then we need to setup the zero reference for real time
			if(_baseTick == 0) {
				_baseTick = tick;
				return 0;
			}
			return    tick - _baseTick;
		}
		//@}

		unsigned int _realFrameNumber;
		unsigned int _simulationFrameNumber;

		unsigned long long _baseTick;
		unsigned long long _totalRealTime;
		unsigned long long _realDt;
		unsigned long long _totalSimulationTime;
		unsigned long long _simulationDt;

		unsigned long long _fixedTicksPerFrame;
		unsigned long long _minSimulationDt;
		unsigned long long _maxSimulationDt;

		ClockModeEnum _clockMode;
		float _fixedSimulationFrameRate;
		float _measuredFps;
		float _inverseFrequency;

	};

} // namespace SteerLib
	
#endif
	
