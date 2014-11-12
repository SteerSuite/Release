//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_HIGH_RES_COUNTER_H__
#define __UTIL_HIGH_RES_COUNTER_H__

/// @file HighResCounter.h
/// @brief Declares timing/counter functionality.
///

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif
#include "Globals.h"


namespace Util {

	#ifndef _WIN32
	/**
	 * @brief A helper class used to store an estimated frequency of the performance counter being used.
	 *
	 * @see
	 *  - Documentation of getHighResCounterValue() for a description of the current situation of high-res timing.
	 */
	class UTIL_API CounterFrequencyEstimator
	{
	 public:
		/// Returns the estimated frequency.
		static inline unsigned long long value() { 
			if (_frequencyComputed) {
				return _freq;
			}
			else {
				_computeFrequencyEstimate();
				return _freq;
			}
			
		}
	 private:
		static void _computeFrequencyEstimate();
		/// The estimated frequency.
		static unsigned long long _freq;
		/// A flag to indicate if the frequency is already computed.
		static bool _frequencyComputed;
	};
	#endif


	/**
	 * @brief Gets a high resolution counter value for timing information.
	 *
	 * If you have problems compiling the code here, read the following details completely.
	 *
	 * This class will eventually use HPET, but it may be a while before that happens in all cases,
	 * because of limited support from operating systems.  HPET is the most recent recommended standard to get
	 * high-performance timing information, rather than reading the high-resolution TSC (Intel's time stamp 
	 * counter that counts # CPU cycles).  In some cases, the TSC may be invalid (e.g. cpu throttles
	 * frequency of the system, or clock skew across different cores), but HPET does not have these problems.
	 *
	 * Note that the return value is a 64-bit unsigned integer.  Most values returned by this counter
	 * will have important bits in the higher order bits, so these values cannot be truncated to 32 bits.
	 *
	 * For Windows, according to http://www.microsoft.com/whdc/system/sysinternals/mm-timer.mspx,
	 * the windows version of timing code transparently chooses between HPET and whatever other timing
	 * options available.
	 *
	 * For Linux, HPET is not widely supported yet.  The main problem is that many linux kernels will not
	 * have the feature compiled into the kernel even if the hardware exists.  Therefore, the Linux implementation
	 * does not use HPET directly.  Instead, the implementation uses Intel's platform-specific timer function, which
	 * will not work on other platforms.  If you need to support Linux on a non-intel machine, you can look at the code
	 * of this function and find two other options that are more platform-independent, but also perform worse.
	 *
	 * Because of this "migration" to HPET, there are three issues to be aware of, if the Intel TSC is used:
	 *  - Timing problems can occur if a process context switches to a different CPU core, which may
	 *    have a different counter value.
	 *  - Timing problems can also occur if any power management techniques are enabled that dynamically change CPU frequencies.
	 *
	 * These problems will not exist on newer hardware and newer versions of Windows, and we will update the Linux
	 * implementation when HPET becomes more widely supported.
	 *
	 */
	static inline unsigned long long getHighResCounterValue()
	{
	#ifdef _WIN32
		unsigned long long tick;
		QueryPerformanceCounter((LARGE_INTEGER *)&tick);
		return tick;
	#elif defined _SOLARIS
		timespec timeInfo;
		clock_gettime(CLOCK_HIGHRES, &timeInfo);
		unsigned long long int nanosecs = ((unsigned long long)timeInfo.tv_sec)*1000000000  +  ((unsigned long long)timeInfo.tv_nsec);
		return nanosecs;
	#else
		// **** NOTE CAREFULLY: ****
		// if you change the option used here, you MUST update the frequency that is computed in getHighResCounterFrequency() as well!

		// OPTION 1: use intel's time stamp counter.  this is the highest resolution, but has problems in the multi-core era,
		// and only works on intel processors from pentium 4 onwards.
		//
		unsigned int lo, hi;
		__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
		return ((unsigned long long)hi<<32) | lo;


		// OPTION 2: use gettimeofday, which is already limited to microsecond resolution (instead of nanoseconds
		// @todo 
		//   - implement this timer option

		// OPTION 3: use GLFW's counter.  this allows us to rely on glfw for platform-independence, but
		// requires us to link glfw into any dynamic plugin module, which is unacceptable.
		//
		// glfwGetTime returns a double, in seconds.
		// Multiply by 1 billion to keep most of the numerical precision before converting it to 64-bit integer.
		//return (unsigned long long)(glfwGetTime()*1000000000.0);


		// OPTION 4: use the "real-time" timer library, but unfortunately, this
		// completely sacrifices high resolution on many systems that could have a platform-dependent
		// high-resolution alternative.
		//
		//timespec timeInfo;
		//clock_gettime(CLOCK_REALTIME, &timeInfo);
		//unsigned long long int nanosecs = ((unsigned long long)timeInfo.tv_sec)*1000000000  +  ((unsigned long long)timeInfo.tv_nsec);
		//return nanosecs;

	#endif
	}


	/**
	 * @brief Returns the frequency of the high resolution counter, in units of "ticks per second."
	 *
	 * @see
	 *  - Documentation of getHighResCounterValue() for a description of the current situation of high-resolution timing.
	 *
	 */
	static inline unsigned long long getHighResCounterFrequency()
	{
	#ifdef _WIN32
		unsigned long long tickFrequency;
		QueryPerformanceFrequency((LARGE_INTEGER *)&tickFrequency);
		return tickFrequency;
	#elif defined _SOLARIS
		// since we are using nanoseconds units, return 1 billion here.
		return 1000000000;
	#else
		return CounterFrequencyEstimator::value();

		// since we are using nanoseconds units, return 1 billion here.
		//return 1000000000;
	#endif
	}


	/// @brief Returns an empirical estimate of smallest amount of time (in seconds) that the counter can measure; this function assumes that getHighResCounterFrequency() returns a reasonably accurate value.
	///
	/// This function takes an average of 5000 trials that try to take two quick successive measurements of the counter.
	///
	/// Note the distinction between resolution of a single tick, and the resolution that can be captured in practice; the resolution of a single tick may be much finer than what can be realistically captured.
	/// This estimate is more representative of what resolution can be captured in practice.
	UTIL_API float getEstimatedCounterResolution();
}

#endif

