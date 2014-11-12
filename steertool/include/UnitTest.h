//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERTOOL_UNIT_TEST_H__
#define __STEERTOOL_UNIT_TEST_H__

/// @file UnitTest.h
/// @brief Declares unit tests for testing some SteerLib components.
///

#include "SteerLib.h"


/// Runs the specific unit test identified by its string name.
void runUnitTest(const std::string & unitTestName);


/**
 * @brief Unit test for the SteerLib::ThreadedTaskManager
 *
 * This test runs the ThreadedTaskManager in a variety of configurations, to make sure
 * that tasks perform correctly and that task-queue/worker-thread synchronization works.  To do this, four
 * different configurations are tested, ranging from 1 to MAX_NUM_THREADS, and each
 * rount of tests is repeated NUM_REPEATS number of times.  The four configurations are
 * a combination of using either a fast/light or slow/heavy task, and the method of waking
 * up worker threads when tasks are added (wake up every task, or one explicit wake after adding all tasks).
 *
 */
class ThreadPoolTest
{
public:
	ThreadPoolTest();
	~ThreadPoolTest();
	void runTest();
protected:
	static void threadPoolFastTestTask( unsigned int threadIndex, void * data );
	static void threadPoolSlowTestTask( unsigned int threadIndex, void * data );

	void _resetOutput();
	void _addAllTasks( bool useSlowTask, bool broadcastWhenTaskAdded );
	void _verifyOutputIsCorrect(unsigned int numThreads, unsigned int testSegment);

	static const unsigned int NUM_TASKS = 5000;
	static const unsigned int MAX_NUM_THREADS = 15;
	static const unsigned int NUM_REPEATS = 5;

	unsigned int * _output;
	Util::ThreadedTaskManager * _taskManager;
};

/**
 * @brief Unit test for HighResCounter and PerformanceProfiler.
 *
 */
class TimingTest
{
public:
	TimingTest() { }
	~TimingTest() { }
	void runTest();
};

/**
 * @brief Unit test for the helper file functions.
 */
class FileUtilTest
{
public:
	FileUtilTest() { }
	~FileUtilTest() { }
	void runTest();
};



/**
 * @brief Unit test for the StateMachine utility class.
 *
 * Not only tests the state machine, but also tests the mechanism of
 * control flow verification used by SimulationEngine.  Note however that
 * SimulationEngine state machine may have changed or been updated since
 * this test was written.
 */
class StateMachineTest
{
public:
	StateMachineTest() { }
	~StateMachineTest() { }
	void runTest();
protected:

	enum TestFSMStateType {
		TEST_FSM_NEW,
		TEST_FSM_INITIALIZING,
		TEST_FSM_READY,
		TEST_FSM_CLEANING_UP,
		TEST_FSM_FINISHED,
		TEST_FSM_LOADING_SIMULATION,
		TEST_FSM_SIMULATION_LOADED,
		TEST_FSM_PREPROCESSING_SIMULATION,
		TEST_FSM_SIMULATION_READY_FOR_UPDATE,
		TEST_FSM_UPDATING_SIMULATION,
		TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED,
		TEST_FSM_POSTPROCESSING_SIMULATION,
		TEST_FSM_SIMULATION_FINISHED,
		TEST_FSM_UNLOADING_SIMULATION,
	};

	void _pretendConstructor();
	void _pretendDestructor();
	void _init();
	void _finish();
	void _loadSim();
	void _unloadSim();
	void _beginSim();
	void _endSim();
	void _updateSim(bool pretendEngineWillQuit);


	class TestFSMCallback : public Util::StateMachineCallbackInterface
	{
	public:
		void setExpectedTransition(unsigned int oldState, unsigned int newState, bool unknown);
		void transitioned(unsigned int previousState, unsigned int currentState);
		unsigned int handleUnknownTransition(unsigned int currentState, unsigned int requestedNewState);
	protected:
		unsigned int _oldState;
		unsigned int _newState;
		bool _unknown;
	};

	TestFSMCallback _testCallback;
	Util::StateMachine _stateMachine;
};

#endif
