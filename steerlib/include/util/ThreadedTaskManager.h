//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_THREADED_TASK_MANAGER_H__
#define __UTIL_THREADED_TASK_MANAGER_H__

/// @file ThreadedTaskManager.h
/// @brief Declares Util::ThreadedTaskManager, a simple platform-independent thread pool.

// For Windows, the ThreadedTaskManager is disabled by default because Windows XP does not support
// condition variables.  However, the code is tested and works on Windows Vista.  If you want to
// use the ThreadedTaskManager in Windows Vista, un-comment this line.
// CAREFUL: If you un-comment this for Windows XP you may not get any informative errors at all, programs will just
// seem to run and do nothing.
// #define USE_VISTA_THREADS



#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <vector>
#include <queue>

#include "Globals.h"


#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif


namespace Util {

	/// Pointer to a function that will be run by the Util::ThreadedTaskManager.
	typedef void (*TaskFunctionPtr)(unsigned int threadIndex, void * data);
	
	/// The data structure used to add a task to the Util::ThreadedTaskManager queue of tasks.
	struct Task {
		/// Pointer to the function that is called for this task.
		TaskFunctionPtr function;
		/// Pointer to data that is given to the function for this task.
		void * data;
	};

	/**
	 * @brief A simple platform-independent thread pool to run tasks.
	 *
	 * To use this class, simply instantiate it, specifying the number of threads in the thread pool.
	 * Then, as needed, add tasks to the queue using #addTask(), and if necessary, at any point, you can
	 * wait for all tasks to complete (#waitForAllTasksToComplete()).
	 *
	 * Of course, <b>you will need to make your tasks thread-safe!</b>
	 *
	 * <h3>Notes</h3>
	 * The throw() syntax, with nothing inside the parentheses, means that the function cannot throw exceptions.
	 *
	 * @todo
	 *  - This class has not been tested thoroughly yet, and may still need to be debugged.
	 *  - Use Windows Vista conditions
	 *
	 * @see
	 *  - Util::Mutex, a platform-independent wrapper for pthreads/win32 user-mode locks.
	 */
	class UTIL_API ThreadedTaskManager {
	public:
		/// During initialization, specify the number of threads you want in the thread pool.
		ThreadedTaskManager(unsigned int numThreads);
		/// The destructor waits for all threads to complete their current task, and then properly terminates the threads.
		~ThreadedTaskManager();
		/// Adds a task to the queue that will be executed by one of the threads; if adding many tasks at the same time, it is only necessary to broadcast on the last task to save considerable overhead.
		void addTask(const Task & newTask, bool broadcastToSleepingWorkerThreads);
		/// Wakes up all sleeping worker threads, called automatically by addTask if broadcastToSleepingWorkerThreads is true, or can be explcitly called once after adding many many tasks to save considerable overhead.
		void wakeUpAllSleepingWorkerThreads() throw();
		/// Waits (if needed, the current thread sleeps) until all existing tasks are complete.
		void waitForAllTasksToComplete();
	protected:
		/// The main function executed by every worker thread; loops infinitely taking tasks off the queue until the ThreadedTaskManager is destroyed.
		void _runWorkerThread() throw();
		/// Acquires the ThreadedTaskManager's private lock
		inline void _lock() throw() {
#ifdef _WIN32
			EnterCriticalSection(&_taskManagerLock);
#else
			pthread_mutex_lock(&(_taskManagerLock));
#endif
		}
		/// Releases the ThreadedTaskManager's private lock
		inline void _unlock() throw() {
#ifdef _WIN32
			LeaveCriticalSection(&_taskManagerLock);
#else
			pthread_mutex_unlock(&(_taskManagerLock));
#endif
		}
		/// Wakes up any sleeping (non-worker) threads that are waiting for all existing tasks in the queue to finish. <em>Assumes lock is already acquired when called</em>.
		void _broadcastAllTasksCompleted() throw();
		/// Used by worker threads; sleeps until there are tasks on the queue; <em>Assumes lock is already acquired when called</em>.
		void _waitUntilQueueHasTasksOrShutdown() throw();
		/// Used in the destructor; waits for all threads to terminate; <em>Lock CANNOT be acquired when called</em>.
		void _waitForAllThreadsToExit() throw();
		/// Returns the index of the current worker thread in the vector of threads, <em>not</em> the same as the thread's win32 or pthreads ID.
		unsigned int _getIndexOfCurrentWorkerThread() throw();
		/// Initializes locks, conditions, barriers, etc, before threads are created; this function is allowed to throw exceptions.
		void _initializeSynchronizationObjects();
		/// Creates all threads as the last step of initialization; allowed to throw exceptions.
		void _createAllThreads();

		/// The queue of tasks.
		std::queue<Util::Task> _taskQueue;
		/// The number of worker threads
		unsigned int _numThreads;
		/// Counter to keep track of whether all existing tasks are completed or not; this is <em>NOT the same</em> as the size of _taskQueue.
		unsigned int _numTasksLeft;
		/// Flag to indicate if worker threads should shut-down next time they are awoken.
		bool _shuttingDown;
		/// Flag to indicate if all existing tasks are completed.
		bool _allTasksCompleted;

		/// @name platform-specific data
		/// @brief The following are platform-dependent declarations.
		//@{
#ifdef _WIN32
		/// The static C wrapper for #_runWorkerThread().
		static DWORD WINAPI _startWorkerThread( void * context) throw() { ((ThreadedTaskManager*)context)->_runWorkerThread(); return NULL; }
		std::vector<HANDLE> _threads;
		CRITICAL_SECTION _taskManagerLock;
#ifdef USE_VISTA_THREADS
		CONDITION_VARIABLE _queueHasTasksCondition;
		CONDITION_VARIABLE _allTasksCompletedCondition;
#endif
#else
		/// The static C wrapper for #_runWorkerThread().
		static void* _startWorkerThread(void * context) throw() { ((ThreadedTaskManager*)context)->_runWorkerThread(); return NULL; }
		std::vector<pthread_t> _threads;
		pthread_mutex_t _taskManagerLock;
		pthread_cond_t _queueHasTasksCondition;
		pthread_cond_t _allTasksCompletedCondition;
#endif
		//@}
		
	};

} // namespace Util


#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
