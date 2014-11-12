//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file UnitTest.cpp
/// @brief Implements unit tests for testing some SteerLib components.

#include <algorithm>
#include <cctype>

#include "UnitTest.h"

using namespace SteerLib;
using namespace Util;
using namespace std;



void runUnitTest(const std::string & unitTestName)
{
	std::string caseInsensitiveTestName = unitTestName;
	std::transform(caseInsensitiveTestName.begin(), caseInsensitiveTestName.end(), caseInsensitiveTestName.begin(), (int(*)(int))tolower);
	if (caseInsensitiveTestName == "threadpool") {
		ThreadPoolTest threadPoolTest;
		threadPoolTest.runTest();
	}
	else if (caseInsensitiveTestName == "timing") {
		TimingTest timingTest;
		timingTest.runTest();
	}
	else if (caseInsensitiveTestName == "fileutil") {
		FileUtilTest fileTest;
		fileTest.runTest();
	}
	else if (caseInsensitiveTestName == "statemachine") {
		StateMachineTest FSMTest;
		FSMTest.runTest();
	}
	else {
		throw GenericException("Unknown name for unit test, \"" + unitTestName + "\"");
	}
}



ThreadPoolTest::ThreadPoolTest()
{
	_output = new unsigned int [NUM_TASKS*16];
}

ThreadPoolTest::~ThreadPoolTest()
{
	delete _output;
}


void ThreadPoolTest::_resetOutput()
{
	for (unsigned int i=0; i<NUM_TASKS; i++) {
		_output[i*16+0] = i;
		_output[i*16+1] = i;
		_output[i*16+2] = i;
		_output[i*16+3] = i;
		_output[i*16+4] = i;
		_output[i*16+5] = i;
		_output[i*16+6] = i;
		_output[i*16+7] = i;
		_output[i*16+8] = i;
		_output[i*16+9] = i;
		_output[i*16+10] = i;
		_output[i*16+11] = i;
		_output[i*16+12] = i;
		_output[i*16+13] = i;
		_output[i*16+14] = i;
		_output[i*16+15] = i;
	}
}

void ThreadPoolTest::threadPoolFastTestTask( unsigned int threadIndex, void * data )
{
	unsigned int temp = ((unsigned int *)data)[0];
	((unsigned int *)data)[0] -= temp;
	((unsigned int *)data)[1] -= temp;
	((unsigned int *)data)[2] -= temp;
	((unsigned int *)data)[3] -= temp;
	((unsigned int *)data)[4] -= temp;
	((unsigned int *)data)[5] -= temp;
	((unsigned int *)data)[6] -= temp;
	((unsigned int *)data)[7] -= temp;
	((unsigned int *)data)[8] -= temp;
	((unsigned int *)data)[9] -= temp;
	((unsigned int *)data)[10] -= temp;
	((unsigned int *)data)[11] -= temp;
	((unsigned int *)data)[12] -= temp;
	((unsigned int *)data)[13] -= temp;
	((unsigned int *)data)[14] -= temp;
	((unsigned int *)data)[15] -= temp;
}

void ThreadPoolTest::threadPoolSlowTestTask( unsigned int threadIndex, void * data )
{
	for (unsigned int i=0; i<10000; i++) {
		unsigned int j = i % 16;
		if (((unsigned int *)data)[j] != 0) {
			((unsigned int *)data)[j] /= 10;
		}
	}
}

void ThreadPoolTest::_addAllTasks( bool useSlowTask, bool broadcastWhenTaskAdded )
{
	for (unsigned int i=0; i < NUM_TASKS; i++) {
		Task newTask;
		newTask.data = &_output[i*16];
		if (useSlowTask) {
			newTask.function = ThreadPoolTest::threadPoolSlowTestTask;
		}
		else {
			newTask.function = ThreadPoolTest::threadPoolFastTestTask;
		}
		_taskManager->addTask(newTask, broadcastWhenTaskAdded);
	}
}

void ThreadPoolTest::_verifyOutputIsCorrect(unsigned int numThreads, unsigned int testSegment)
{
	for (unsigned int i=0; i < NUM_TASKS*16; i++) {
		if (_output[i] != 0) {
			std::cerr << "FAILED: output[" << i << "] is " << _output[i] << ", for " << numThreads << " threads on test #" << testSegment << ".\n";
			throw GenericException("Unit test for ThreadedTaskManager failed.");
		}
	}
}

void ThreadPoolTest::runTest()
{
	PerformanceProfiler pp1, pp2, pp3, pp4;
	pp1.reset();
	pp2.reset();
	pp3.reset();
	pp4.reset();


	// test the thread pool running from 1 to MAX_NUM_THREADS threads
	for (unsigned int numThreads=1; numThreads<MAX_NUM_THREADS; numThreads++) {

		std::cout << "Testing ThreadedTaskManager with " << numThreads << " threads:" << std::endl;
		// allocate new task manager
		_taskManager = new ThreadedTaskManager(numThreads);
		pp1.reset();
		pp2.reset();
		pp3.reset();
		pp4.reset();

		for (unsigned int testCount=0; testCount<NUM_REPEATS; testCount++) {
			//======================================================================
			// test segment #1:  fast task, broadcast once after all tasks are added
			//======================================================================
			// initialize the data
			_resetOutput();

			pp1.start();

			// add tasks to queue
			_addAllTasks(false,false);
			_taskManager->wakeUpAllSleepingWorkerThreads();
			// wait for all tasks
			_taskManager->waitForAllTasksToComplete();

			pp1.stop();

			// verify the answers are correct
			_verifyOutputIsCorrect(numThreads, 1);


			//======================================================================
			// test segment #2:  fast task, broadcast every time a task is added
			//======================================================================
			// initialize the data
			_resetOutput();

			pp2.start();

			// add tasks to queue
			_addAllTasks(false,true);
			// wait for all tasks
			_taskManager->waitForAllTasksToComplete();

			pp2.stop();

			// verify the answers are correct
			_verifyOutputIsCorrect(numThreads, 2);
			
			//======================================================================
			// test segment #3:  slow task, broadcast once after all tasks are added
			//======================================================================
			// initialize the data
			_resetOutput();

			pp3.start();

			// add tasks to queue
			_addAllTasks(true,false);
			_taskManager->wakeUpAllSleepingWorkerThreads();
			// wait for all tasks
			_taskManager->waitForAllTasksToComplete();

			pp3.stop();

			// verify the answers are correct
			_verifyOutputIsCorrect(numThreads, 3);

			//======================================================================
			// test segment #4:  slow task, broadcast every time a task is added
			//======================================================================
			// initialize the data
			_resetOutput();

			pp4.start();

			// add tasks to queue
			_addAllTasks(true,true);
			// wait for all tasks
			_taskManager->waitForAllTasksToComplete();

			pp4.stop();

			// verify the answers are correct
			_verifyOutputIsCorrect(numThreads, 4);
			
		}

		delete _taskManager;
		std::cout << "   Success!\n";
		std::cout << "   avg time using fast tasks with one batched broadcast: " << pp1.getAverageExecutionTime() << "\n";
		std::cout << "   avg time using fast tasks with a broadcast per task:  " << pp2.getAverageExecutionTime() << "\n";
		std::cout << "   avg time using slow tasks with one batched broadcast: " << pp3.getAverageExecutionTime() << "\n";
		std::cout << "   avg time using slow tasks with a broadcast per task:  " << pp4.getAverageExecutionTime() << "\n";
		
	}
	
}




void TimingTest::runTest()
{
	unsigned long long ticksPerSecond = getHighResCounterFrequency();
	float resolution = getEstimatedCounterResolution();

	std::cout << "Assuming that counter frequency is:\n";
	std::cout << "     " << ticksPerSecond << " ticks per second (" << ((float)ticksPerSecond) / 1000000.0f << " MHz)\n";
	std::cout << "     (this frequency may not be same as CPU Frequency).\n\n";

	std::cout << "Smallest interval of time that the counter can generally measure:\n";
	std::cout << "     " << resolution << " seconds" << " (" << resolution*1000000.0f << " microseconds)\n\n";


	/// @todo fill in the rest of the timing test unit test
}

void FileUtilTest::runTest()
{
	if (!pathExists(".")) {
		throw GenericException("FAILED:  pathExists() indicates that . does not exist.");
	}

	if (!isExistingDirectory(".")) {
		throw GenericException("FAILED:  isExistingDirectory() indicates that . is not an existing directory.");
	}

	std::vector<std::string> fileNames;

	fileNames.clear();
	getFilesInDirectory(".", "", fileNames);
	std::cout << "Found " << fileNames.size() << " files in current directory:\n";
	for (unsigned int i=0; i<fileNames.size(); i++) {
		std::cout << "  " << fileNames[i] << "\n";

		if (!pathExists(fileNames[i])) {
			throw GenericException("FAILED: getFilesInDirectory() indicates that " + fileNames[i] + " exists, but pathExists() indicates that " + fileNames[i] + " does not exist.");
		}
		if (isExistingDirectory(fileNames[i]) == isExistingFile(fileNames[i])) {
			throw GenericException("FAILED: " + fileNames[i] + " is considered both a file and a directory.\n");
		}
	}

	fileNames.clear();
	getFilesInDirectory(".", ".foo", fileNames);
	std::cout << fileNames.size() << " files with a .foo extension\n";
	for (unsigned int i=0; i<fileNames.size(); i++) {
		std::cout << "  " << fileNames[i] << "\n";
	}

	fileNames.clear();
	getFilesInDirectory(".", ".o", fileNames);
	std::cout << fileNames.size() << " files with a .o extension\n";
	for (unsigned int i=0; i<fileNames.size(); i++) {
		std::cout << "  " << fileNames[i] << "\n";
	}

	fileNames.clear();
	getFilesInDirectory(".", ".dll", fileNames);
	std::cout << fileNames.size() << " files with a .dll extension\n";
	for (unsigned int i=0; i<fileNames.size(); i++) {
		std::cout << "  " << fileNames[i] << "\n";
	}

}


void StateMachineTest::runTest()
{
	std::cout << "Test 1: correct usage, no simulation loaded...\n";
	_pretendConstructor();
	{
		_init();
		_finish();
		_init();
		_finish();
	}
	_pretendDestructor();

	std::cout << "Test 2: correct usage, simulation loaded but not run...\n";
	_pretendConstructor();
	{
		_init();
		{
			_loadSim();
			_unloadSim();
			_loadSim();
			_unloadSim();
		}
		_finish();
		_init();
		{
			_loadSim();
			_unloadSim();
			_loadSim();
			_unloadSim();
		}
		_finish();
	}
	_pretendDestructor();

	std::cout << "Test 3: correct usage, simulation loaded and used with control over when to stop...\n";
	_pretendConstructor();
	{
		_init();
		{
			_loadSim();
			{
				_beginSim();
				_endSim();
			}
			_unloadSim();
			_loadSim();
			{
				_beginSim();
				_updateSim(false);
				_updateSim(false);
				_updateSim(false);
				_endSim();
			}
			_unloadSim();
		}
		_finish();
		_init();
		{
			_loadSim();
			{
				_beginSim();
				_updateSim(false);
				_updateSim(false);
				_endSim();
			}
			_unloadSim();
			_loadSim();
			{
				_beginSim();
				for (unsigned int i=0; i<107; i++) {
					_updateSim(false);
				}
				_endSim();
			}
			_unloadSim();
		}
		_finish();
	}
	_pretendDestructor();

	std::cout << "Test 4: correct usage, simulation loaded and used with engine deciding when to stop...\n";
	_pretendConstructor();
	{
		_init();
		{
			_loadSim();
			{
				_beginSim();
				_updateSim(true);
				_endSim();
			}
			_unloadSim();
			_loadSim();
			{
				_beginSim();
				_updateSim(false);
				_updateSim(false);
				_updateSim(true);
				_endSim();
			}
			_unloadSim();
		}
		_finish();
	}
	_pretendDestructor();

	std::cout << "Test 5: correct usage, making sure that it works without a callback as well...\n";
	_stateMachine.setCallback(NULL);
	_pretendConstructor();
	{
		assert(_stateMachine.getCurrentState() == TEST_FSM_NEW);
		_init();
		assert(_stateMachine.getCurrentState() == TEST_FSM_READY);
		_finish();
		assert(_stateMachine.getCurrentState() == TEST_FSM_FINISHED);

		_init();
		{
			assert(_stateMachine.getCurrentState() == TEST_FSM_READY);
			_loadSim();
			assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_LOADED);
			_unloadSim();
			assert(_stateMachine.getCurrentState() == TEST_FSM_READY);

			// check if canTransitionToState works properly...
			assert(_stateMachine.canTransitionToState(TEST_FSM_LOADING_SIMULATION));
			assert(_stateMachine.canTransitionToState(TEST_FSM_CLEANING_UP));
			assert(!_stateMachine.canTransitionToState(TEST_FSM_NEW));
			assert(!_stateMachine.canTransitionToState(TEST_FSM_POSTPROCESSING_SIMULATION));
			assert(!_stateMachine.canTransitionToState(TEST_FSM_READY));


			_loadSim();
			{
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_LOADED);
				_beginSim();
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_READY_FOR_UPDATE);
				_updateSim(false);
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_READY_FOR_UPDATE);
				_endSim();
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_FINISHED);
			}
			_unloadSim();
			assert(_stateMachine.getCurrentState() == TEST_FSM_READY);
			_loadSim();
			{
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_LOADED);
				_beginSim();
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_READY_FOR_UPDATE);
				_updateSim(false);
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_READY_FOR_UPDATE);
				_updateSim(false);
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_READY_FOR_UPDATE);
				_updateSim(true);
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED);
				_endSim();
				assert(_stateMachine.getCurrentState() == TEST_FSM_SIMULATION_FINISHED);
			}
			_unloadSim();
			assert(_stateMachine.getCurrentState() == TEST_FSM_READY);
		}
		_finish();
		assert(_stateMachine.getCurrentState() == TEST_FSM_FINISHED);
	}
	_pretendDestructor();

	std::cout << "Test 6: incorrect usages, making sure that exceptions get caught...\n";
	std::cout << "(Test 6 not implemented yet)\n";

	std::cout << "State machine passed all tests!\n";
}

void StateMachineTest::_pretendConstructor()
{
	// the state machine has 3 forks, and is otherwise a straightforward cycle through the states:
	//
	// whenever the engine is "ready", the user can:
	//   - OPTION 1: clean up, and then re-initialize so the engine is "ready" again.
	//   - OPTION 2: load a simulation
	//     - after loading, the user has two options:
	//       - OPTION A: run the simulation (preprocess, updates, postprocess)
	//         - while running the simulation, the user has two choices
	//           - OPTION i: keep updating the engine until it indicates it is done, and then the user must postprocess the simulation
	//           - OPTION ii: update the engine however many times, but the user decides to stop, and post-processes the simulation, before the engine indicated to stop.
	//         - after postprocessing the user must unload the simulation
	//       - option B: simply unload the simulation without ever running.
	//

	// The engine always starts new, and must initialize to become ready.
	_stateMachine.setStartState( TEST_FSM_NEW );
	_stateMachine.addTransition( TEST_FSM_NEW, TEST_FSM_INITIALIZING );
	_stateMachine.addTransition( TEST_FSM_INITIALIZING, TEST_FSM_READY );

	// Once ready, the user can choose to jump straight to cleanup, or to load a simulation.
	_stateMachine.addTransition( TEST_FSM_READY, TEST_FSM_CLEANING_UP );
	_stateMachine.addTransition( TEST_FSM_READY, TEST_FSM_LOADING_SIMULATION );

	// If the user decided to load a simulation, then the simulation becomes loaded, and the user has the choice
	// whether to start the simulation by initiating preprocessing, or to unload the simulation before ever starting it.
	_stateMachine.addTransition( TEST_FSM_LOADING_SIMULATION, TEST_FSM_SIMULATION_LOADED );
	_stateMachine.addTransition( TEST_FSM_SIMULATION_LOADED, TEST_FSM_UNLOADING_SIMULATION );
	_stateMachine.addTransition( TEST_FSM_SIMULATION_LOADED, TEST_FSM_PREPROCESSING_SIMULATION );

	// If the user decided to run the simulation (by initiating the preprocessing), then the engine is ready for updates after preprocessing.
	_stateMachine.addTransition( TEST_FSM_PREPROCESSING_SIMULATION, TEST_FSM_SIMULATION_READY_FOR_UPDATE );

	// When the engine is ready for updates, the user can either update the simulation
	// or finish the simulation by initiating post-processing.
	_stateMachine.addTransition( TEST_FSM_SIMULATION_READY_FOR_UPDATE, TEST_FSM_UPDATING_SIMULATION );
	_stateMachine.addTransition( TEST_FSM_SIMULATION_READY_FOR_UPDATE, TEST_FSM_POSTPROCESSING_SIMULATION );

	// After one update, the engine will either be ready for more updates, or indicate that no more updates are allowed.
	// if no more updates are allowed, then the user must initiate post-processing next.
	_stateMachine.addTransition( TEST_FSM_UPDATING_SIMULATION, TEST_FSM_SIMULATION_READY_FOR_UPDATE );
	_stateMachine.addTransition( TEST_FSM_UPDATING_SIMULATION, TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED );
	_stateMachine.addTransition( TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED, TEST_FSM_POSTPROCESSING_SIMULATION );

	// After the user initiates a postprocess, the simulation becomes finished, and then the user
	// must unload the simulation, after which the engine is ready to load another simulation or to finish.
	_stateMachine.addTransition( TEST_FSM_POSTPROCESSING_SIMULATION, TEST_FSM_SIMULATION_FINISHED );
	_stateMachine.addTransition( TEST_FSM_SIMULATION_FINISHED, TEST_FSM_UNLOADING_SIMULATION );
	_stateMachine.addTransition( TEST_FSM_UNLOADING_SIMULATION, TEST_FSM_READY );

	// If the user initiated the engine to cleanup, then the engine becomes finished, after which
	// the engine can be either safely de-allocated, or re-initialized.
	_stateMachine.addTransition( TEST_FSM_CLEANING_UP, TEST_FSM_FINISHED );
	_stateMachine.addTransition( TEST_FSM_FINISHED, TEST_FSM_INITIALIZING );

	_stateMachine.setCallback(&_testCallback);
	_stateMachine.start();
}

void StateMachineTest::_pretendDestructor()
{
	_stateMachine.stop();
}


void StateMachineTest::_init()
{
	if ((_stateMachine.getCurrentState() != TEST_FSM_NEW) && (_stateMachine.getCurrentState() != TEST_FSM_FINISHED)) {
		throw GenericException("state machine reached invalid state, detected when init() was called.");
	}
	_testCallback.setExpectedTransition(_stateMachine.getCurrentState(), TEST_FSM_INITIALIZING, false);
	_stateMachine.transitionToState(TEST_FSM_INITIALIZING);


	// test some invalid transitons, to see if the state machine handles them properly;
	_testCallback.setExpectedTransition(TEST_FSM_INITIALIZING, TEST_FSM_INITIALIZING, true);
	_stateMachine.transitionToState(TEST_FSM_INITIALIZING);
	_testCallback.setExpectedTransition(TEST_FSM_INITIALIZING, TEST_FSM_CLEANING_UP, true);
	_stateMachine.transitionToState(TEST_FSM_CLEANING_UP);


	_testCallback.setExpectedTransition(TEST_FSM_INITIALIZING, TEST_FSM_READY, false);
	_stateMachine.transitionToState(TEST_FSM_READY);
}

void StateMachineTest::_finish()
{
	_testCallback.setExpectedTransition(TEST_FSM_READY, TEST_FSM_CLEANING_UP, false);
	_stateMachine.transitionToState(TEST_FSM_CLEANING_UP);


	_testCallback.setExpectedTransition(TEST_FSM_CLEANING_UP, TEST_FSM_FINISHED, false);
	_stateMachine.transitionToState(TEST_FSM_FINISHED);

}

void StateMachineTest::_loadSim()
{
	_testCallback.setExpectedTransition(TEST_FSM_READY, TEST_FSM_LOADING_SIMULATION, false);
	_stateMachine.transitionToState(TEST_FSM_LOADING_SIMULATION);


	// test some invalid transitons, to see if the state machine handles them properly;
	_testCallback.setExpectedTransition(TEST_FSM_LOADING_SIMULATION, TEST_FSM_LOADING_SIMULATION, true);
	_stateMachine.transitionToState(TEST_FSM_LOADING_SIMULATION);
	_testCallback.setExpectedTransition(TEST_FSM_LOADING_SIMULATION, TEST_FSM_CLEANING_UP, true);
	_stateMachine.transitionToState(TEST_FSM_CLEANING_UP);



	_testCallback.setExpectedTransition(TEST_FSM_LOADING_SIMULATION, TEST_FSM_SIMULATION_LOADED, false);
	_stateMachine.transitionToState(TEST_FSM_SIMULATION_LOADED);
}

void StateMachineTest::_unloadSim()
{
	if ((_stateMachine.getCurrentState() != TEST_FSM_SIMULATION_LOADED) && (_stateMachine.getCurrentState() != TEST_FSM_SIMULATION_FINISHED)) {
		throw GenericException("state machine reached invalid state, detected when unloadsim() was called.");
	}
	_testCallback.setExpectedTransition(_stateMachine.getCurrentState(), TEST_FSM_UNLOADING_SIMULATION, false);
	_stateMachine.transitionToState(TEST_FSM_UNLOADING_SIMULATION);



	_testCallback.setExpectedTransition(TEST_FSM_UNLOADING_SIMULATION, TEST_FSM_READY, false);
	_stateMachine.transitionToState(TEST_FSM_READY);
}

void StateMachineTest::_beginSim()
{
	_testCallback.setExpectedTransition(TEST_FSM_SIMULATION_LOADED, TEST_FSM_PREPROCESSING_SIMULATION, false);
	_stateMachine.transitionToState(TEST_FSM_PREPROCESSING_SIMULATION);


	_testCallback.setExpectedTransition(TEST_FSM_PREPROCESSING_SIMULATION, TEST_FSM_SIMULATION_READY_FOR_UPDATE, false);
	_stateMachine.transitionToState(TEST_FSM_SIMULATION_READY_FOR_UPDATE);
}

void StateMachineTest::_endSim()
{
	if ((_stateMachine.getCurrentState() != TEST_FSM_SIMULATION_READY_FOR_UPDATE) && (_stateMachine.getCurrentState() != TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED)) {
		throw GenericException("state machine reached invalid state, detected when endSim() was called.");
	}
	_testCallback.setExpectedTransition(_stateMachine.getCurrentState(), TEST_FSM_POSTPROCESSING_SIMULATION, false);
	_stateMachine.transitionToState(TEST_FSM_POSTPROCESSING_SIMULATION);



	_testCallback.setExpectedTransition(TEST_FSM_POSTPROCESSING_SIMULATION, TEST_FSM_SIMULATION_FINISHED, false);
	_stateMachine.transitionToState(TEST_FSM_SIMULATION_FINISHED);
}

void StateMachineTest::_updateSim(bool pretendEngineWillQuit)
{
	_testCallback.setExpectedTransition(TEST_FSM_SIMULATION_READY_FOR_UPDATE, TEST_FSM_UPDATING_SIMULATION, false);
	_stateMachine.transitionToState(TEST_FSM_UPDATING_SIMULATION);



	if (pretendEngineWillQuit) {
		_testCallback.setExpectedTransition(TEST_FSM_UPDATING_SIMULATION, TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED, false);
		_stateMachine.transitionToState(TEST_FSM_SIMULATION_NO_MORE_UPDATES_ALLOWED);
	}
	else {
		_testCallback.setExpectedTransition(TEST_FSM_UPDATING_SIMULATION, TEST_FSM_SIMULATION_READY_FOR_UPDATE, false);
		_stateMachine.transitionToState(TEST_FSM_SIMULATION_READY_FOR_UPDATE);
	}
}



void StateMachineTest::TestFSMCallback::setExpectedTransition(unsigned int oldState, unsigned int newState, bool unknown)
{
	_oldState = oldState;
	_newState = newState;
	_unknown = unknown;
}

void StateMachineTest::TestFSMCallback::transitioned(unsigned int previousState, unsigned int currentState)
{
	if (_unknown == true) {
		throw GenericException("FAILED: expected transition " + toString(_oldState) + " to "
			+ toString(_newState) + " to be invalid, but instead we received a valid transition "
			+ toString(previousState) + " to " + toString(currentState)+ ".\n" );
	}

	if (previousState != _oldState) {
		throw GenericException("FAILED: unexpected value for previousState, while handling a valid transition.  Expected: " + toString(_oldState) + ", Received: " + toString(previousState) + ".\n");
	}

	if (currentState != _newState) {
		throw GenericException("FAILED: unexpected value for currentState, while handling a valid transition.  Expected: " + toString(_newState) + ", Received: " + toString(currentState) + ".\n");
	}


}

unsigned int StateMachineTest::TestFSMCallback::handleUnknownTransition(unsigned int currentState, unsigned int requestedNewState)
{
	if (_unknown == false) {
		throw GenericException("FAILED: expected transition " + toString(_oldState) + " to "
			+ toString(_newState) + " to be valid, but instead we received an unknown transition "
			+ toString(currentState) + " to " + toString(requestedNewState) + ".\n" );
	}

	if (currentState != _oldState) {
		throw GenericException("FAILED: unexpected value for previousState, while handling an unknown transition.  Expected: " + toString(_oldState) + ", Received: " + toString(currentState) + ".\n");
	}

	if (requestedNewState != _newState) {
		throw GenericException("FAILED: unexpected value for currentState, while handling an unknown transition.  Expected: " + toString(_newState) + ", Received: " + toString(requestedNewState) + ".\n");
	}

	return currentState;
}
