//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file ThreadedTaskManager.cpp
/// @brief Implements Util::ThreadTaskManager functionality.

/// @todo
///   - consider using pthreads barriers instead of conditions for in some places.
///   - make it exception-safe:  add a check to allow only the master thread to use the primary public functions.  see pthread_self()
///   - support win32 threads (not so easy to do that without condition structures like pthreads...)
///   - document this file properly

#include <iostream>
#include "util/ThreadedTaskManager.h"
#include "util/GenericException.h"
#include "util/Misc.h"

using namespace Util;

ThreadedTaskManager::ThreadedTaskManager(unsigned int numThreads)
{
#ifdef _WIN32
#ifndef USE_VISTA_THREADS
	throw GenericException("Util::ThreadedTaskManager is currently compiled for Windows XP (not Vista), and\ndoes not support multi-threading.\n\nIf you are using Windows Vista or later, you can use the\nUSE_VISTA_THREADS preprocessor macro and re-compile, and multi-threading will work fine.\n");
#endif
#endif

	
	if (numThreads == 0) {
		throw GenericException("Cannot initialize ThreadedTaskManager with zero threads.");
	}

	_numThreads = numThreads;
	_numTasksLeft = 0;
	_threads.clear();
	_shuttingDown = false;
	_allTasksCompleted = true;

	_initializeSynchronizationObjects();

	// grab the lock
	_lock();

	// create the new threads
	try {
		_createAllThreads();
	}
	catch (std::exception &e) {
		// release the lock and propagate the exception
		_unlock();
		throw e;
	}

	// release the lock.
	_unlock();

}

ThreadedTaskManager::~ThreadedTaskManager()
{
	_lock();
	_shuttingDown = true;
	wakeUpAllSleepingWorkerThreads();
	_unlock();

	_waitForAllThreadsToExit();

#ifdef _WIN32
	DeleteCriticalSection(&_taskManagerLock);
	// conditions variables in win32 do not have any cleanup/finish function to be called...
#else
	pthread_mutex_destroy(&_taskManagerLock);
	pthread_cond_destroy(&_queueHasTasksCondition);
	pthread_cond_destroy(&_allTasksCompletedCondition);
#endif
}



void ThreadedTaskManager::addTask(const Task & newTask, bool broadcastToSleepingWorkerThreads)
{
	// grab the lock
	_lock();

	// add the new task to the queue.
	try {
		_taskQueue.push(newTask);
	}
	catch (std::exception &e) {
		// release the lock and propagate the exception
		_unlock();
		throw e;
	}

	_numTasksLeft++;
	_allTasksCompleted = false;

	// wake up worker threads if the user set this param to true.
	if (broadcastToSleepingWorkerThreads)
		wakeUpAllSleepingWorkerThreads();

	// release the lock
	_unlock();

}

void ThreadedTaskManager::waitForAllTasksToComplete()
{
#ifdef _WIN32
#ifdef USE_VISTA_THREADS
	_lock();
	while (!_allTasksCompleted) {
		SleepConditionVariableCS( &_allTasksCompletedCondition, &_taskManagerLock, INFINITE);
	}
	_unlock();
#else
	assert(false); // we should be throwing an informative GenericException during initialization instead of reaching here.
#endif
#else
	_lock();
	while (!_allTasksCompleted) {
		pthread_cond_wait(&_allTasksCompletedCondition, &_taskManagerLock);
	}
	_unlock();
#endif
}

void ThreadedTaskManager::_runWorkerThread() throw()
{
	unsigned int threadIndex = _getIndexOfCurrentWorkerThread();
	while(true) {

		// acquire the lock
		_lock();
			
		// sleep until there is work to do.
		// If the thread must sleep in _waitUntilQueueHasTasksOrShutdown, then the lock is released,
		// and the thread reacquires the lock when it wakes up.
		_waitUntilQueueHasTasksOrShutdown();


		// if shutting down, then stop this thread
		if (_shuttingDown) {
			_unlock();
			return;
		}

		// Now we know we were not shuttingDown, so if the task queue is empty here, then we have an internal bug.
		assert(!_taskQueue.empty());

		// pop the next task off the queue
		Task nextTask = _taskQueue.front();
		_taskQueue.pop();

		// release the lock
		_unlock();

		// run the task, catching exceptions if they occur.
		try {
			nextTask.function(threadIndex, nextTask.data);
		}
		catch (std::exception &e) {
			std::cerr << "\n\nERROR: exception caught in worker thread #" << threadIndex << ":\n" << e.what() << "\n";
			exit(1);
		}


		// acquire the lock
		_lock();

		// decrement the number of tasks there are.
		_numTasksLeft--;

		// if all threads are completed, broadcast that.
		if (_numTasksLeft == 0) {
			_allTasksCompleted = true;
			_broadcastAllTasksCompleted();
		}

		// release the lock
		_unlock();

	}
}

void ThreadedTaskManager::wakeUpAllSleepingWorkerThreads() throw()
{
#ifdef _WIN32
#ifdef USE_VISTA_THREADS
	WakeAllConditionVariable( &_queueHasTasksCondition);
#else
	assert(false); // we should be throwing an informative GenericException during initialization instead of reaching here.
#endif
#else
	pthread_cond_broadcast(&_queueHasTasksCondition);
#endif
}

void ThreadedTaskManager::_broadcastAllTasksCompleted() throw()
{
#ifdef _WIN32
#ifdef USE_VISTA_THREADS
	WakeAllConditionVariable(&_allTasksCompletedCondition);
#else
	assert(false); // we should be throwing an informative GenericException during initialization instead of reaching here.
#endif
#else
	pthread_cond_broadcast(&_allTasksCompletedCondition);
#endif
}

void ThreadedTaskManager::_waitForAllThreadsToExit() throw()
{
#ifdef _WIN32
	for(unsigned int i=0; i < _numThreads; i++) {
		WaitForSingleObject(_threads[i],INFINITE);
		CloseHandle(_threads[i]);
	}
#else
	for(unsigned int i=0; i < _numThreads; i++) {
		pthread_join( _threads[i], NULL);
	}
#endif
}
	
void ThreadedTaskManager::_waitUntilQueueHasTasksOrShutdown() throw()
{
#ifdef _WIN32
#ifdef USE_VISTA_THREADS
	while ((_taskQueue.size() == 0) && (!_shuttingDown)) {
		SleepConditionVariableCS(&_queueHasTasksCondition, &_taskManagerLock, INFINITE);
	}
#else
	assert(false); // we should be throwing an informative GenericException during initialization instead of reaching here.
#endif
#else
	while ((_taskQueue.size() == 0) && (!_shuttingDown)) {
		pthread_cond_wait(&_queueHasTasksCondition, &_taskManagerLock);
	}
#endif
}


unsigned int ThreadedTaskManager::_getIndexOfCurrentWorkerThread() throw()
{
#ifdef _WIN32
#ifdef USE_VISTA_THREADS
	// compute the threadIndex of this thread.
	unsigned int threadIndex = 0;
	DWORD threadSelfId = GetCurrentThreadId();
	while ( (GetThreadId( _threads[threadIndex]) != threadSelfId)) {
		threadIndex++;
		// this thread must exist in the _threads list, or else we made an internal error.
		assert(threadIndex < _threads.size());
	}
	// at this point, threadIndex is a unique integer ID between 0 and _numThreads-1
	return threadIndex;
#else
	assert(false); // we should be throwing an informative GenericException during initialization instead of reaching here.
	return 0;
#endif
#else
	// compute the threadIndex of this thread.
	pthread_t threadSelf = pthread_self();
	unsigned int threadIndex = 0;
	while ( ! pthread_equal(threadSelf, _threads[threadIndex])) {
		threadIndex++;
		// this thread must exist in the _threads list, or else we made an internal error.
		assert(threadIndex < _threads.size());
	}
	// at this point, threadIndex is a unique integer ID between 0 and _numThreads-1
	return threadIndex;
#endif
}


void ThreadedTaskManager::_initializeSynchronizationObjects()
{
#ifdef _WIN32
	OSVERSIONINFO windowsVersion;
	windowsVersion.dwBuildNumber = 0;
	windowsVersion.dwMajorVersion = 0;
	windowsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&windowsVersion);
	if (windowsVersion.dwMajorVersion < 6) {
		// not expected to reach here, but we'll leave this run-time check here, anyway.
		throw GenericException("Util::ThreadedTaskManager requires Windows Vista or later; Windows XP and earlier do not have condition variables.");
	}

	InitializeCriticalSection(& _taskManagerLock);
#ifdef USE_VISTA_THREADS
	InitializeConditionVariable( & _queueHasTasksCondition );
	InitializeConditionVariable( & _allTasksCompletedCondition );
#endif
#else
	unsigned int returnValue;

	returnValue = pthread_mutex_init(&_taskManagerLock, NULL);
	if (returnValue != 0) {
		throw GenericException("pthread_mutex_init failed with return value " + toString(returnValue));
	}
	
	returnValue = pthread_cond_init(&_queueHasTasksCondition,NULL);
	if (returnValue != 0) {
		throw GenericException("pthread_cond_init failed with return value " + toString(returnValue));
	}

	returnValue = pthread_cond_init(&_allTasksCompletedCondition,NULL);
	if (returnValue != 0) {
		throw GenericException("pthread_cond_init failed with return value " + toString(returnValue));
	}
#endif
}


void ThreadedTaskManager::_createAllThreads()
{
#ifdef _WIN32
	for (unsigned int i=0; i<_numThreads; i++) {
		HANDLE newThread;
		newThread = CreateThread(NULL, 0, _startWorkerThread, this, 0, NULL);
		if (newThread == NULL) {
			throw GenericException("CreateThread failed with error code " + toString(GetLastError()));
		}
		_threads.push_back(newThread);
	}
#else
	for (unsigned int i=0; i<_numThreads; i++) {
		pthread_t newThread;
		unsigned int returnValue = pthread_create( &newThread, NULL, _startWorkerThread, this );
		if (returnValue != 0) {
			throw GenericException("pthread_create failed with return value " + toString(returnValue));
		}
		_threads.push_back(newThread);
	}
#endif
}
