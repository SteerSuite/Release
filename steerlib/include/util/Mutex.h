//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_MUTEX_H__
#define __UTIL_MUTEX_H__

/// @file Mutex.h
/// @brief Declares Util::Mutex, a simple platform-independent wrapper for user-level lock synchronization.

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "Globals.h"

namespace Util {

#ifdef ENABLE_MULTITHREADING
	/**
	 * @brief a basic platform-independent wrapper for user-level lock synchronization.
	 *
	 * Note that "user-level" synchronization means that (to the best of our knowledge) this
	 * class will not enter kernel mode to acquire and release locks.  Entering kernel mode can be
	 * extremely expensive, so avoiding that keeps the locking mechanism lightweight and very fast.
	 *
	 * @todo
	 *   - experiment with win32 spinCount parameter, which allows the thread to spin-wait for some time before sleeping.
	 *     This may be useful if there is a lot of contention for the mutex.
	 *   - consider adding a tryLock() function.
	 */
	class UTIL_API Mutex {
	public:
		/// Initializes the lock
		Mutex() {
#ifdef _WIN32
			InitializeCriticalSection(&_lock);
#else
			pthread_mutex_init(&_lock,NULL);
#endif
		}

		/// Cleans up the lock
		~Mutex() {
#ifdef _WIN32
			DeleteCriticalSection(&_lock);
#else
			pthread_mutex_destroy(&_lock);
#endif
		}

		/// Acquire the lock
		inline void lock() throw() {
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
		}
		/// Release the lock
		inline void unlock() throw() {
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
		}
	protected:

#ifdef _WIN32
		CRITICAL_SECTION _lock;
#else
		pthread_mutex_t _lock;
#endif
	};
#else // ifdef ENABLE_MULTITHREADING

	// dummy no-op functionality if multithreading is disabled.
	class Mutex {
		Mutex() { }
		~Mutex() { }
		inline void lock() throw() { }
		inline void unlock() throw() { }
	};

#endif // ifdef ENABLE_MULTITHREADING


} // namespace Util

#endif
