//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SimulationEngine.cpp
/// @brief Implements the SimulationEngine class.
///
/// @todo
///  - clean up init() into several smaller helper functions

#include <iomanip>
#include <algorithm>
#include <string>

#include "simulation/SimulationOptions.h"
#include "simulation/SimulationEngine.h"

#include "modules/RecFilePlayerModule.h"
#include "modules/DummyAIModule.h"
#include "modules/TestCasePlayerModule.h"
#include "modules/SteerBugModule.h"
#include "modules/SteerBenchModule.h"
#include "modules/MetricsCollectorModule.h"
#include "modules/SimulationRecorderModule.h"


// to handle user input properly with GLFW_PRESS and GLFW_RELEASE macros
#include "glfw/include/GL/glfw.h"

using namespace std;
using namespace SteerLib;
using namespace Util;

// #define _DEBUG 1


SimulationEngine::SimulationEngine()
{
	_setupStateMachine();
}

void SimulationEngine::_reset()
{
	_moduleMetaInfoByName.clear();
	_moduleMetaInfoByReference.clear();
	_modulesInExecutionOrder.clear();
	_moduleConflicts.clear();
	_agents.clear();
	_selectedAgents.clear();
	_agentOwners.clear();
	_commands.clear();
	_obstacles.clear();
	//_clock reset ???;
	//_camera reset ???;
	_spatialDatabase = NULL;
	_engineController = NULL;
	_numFramesSimulated = 0;
	_simulationLoaded = false;
	_simulationRunning = false;
	_simulationDone = false;
	_engineStateMachineCallback.setEngine(this);
	_stop = false;
}

void SimulationEngine::stop()
{
	_stop = true;
}

//========================================

void SimulationEngine::init(SimulationOptions * options, SteerLib::EngineControllerInterface * engineController)
{
	_engineState.transitionToState(ENGINE_STATE_INITIALIZING);

	if (engineController == NULL) {
		std::cerr << "WARNING: No engine driver interface was given to the engine.\n";
	}

	_reset();


	_options = options;
	_engineController = engineController;

	Clock::ClockModeEnum clockMode;
	if (_options->engineOptions.clockMode == "fixed-fast") {
		clockMode = Clock::CLOCK_MODE_FIXED_AS_FAST_AS_POSSIBLE;
	}
	else if  (_options->engineOptions.clockMode == "fixed-real-time") {
		clockMode = Clock::CLOCK_MODE_FIXED_REAL_TIME;
	}
	else {
		clockMode = Clock::CLOCK_MODE_VARIABLE_REAL_TIME;
	}
	_clock.setClockMode(clockMode, _options->engineOptions.fixedFPS, _options->engineOptions.minVariableDt, _options->engineOptions.maxVariableDt);

	_camera.reset();
	_camera.setView(_options->guiOptions.cameraPosition, _options->guiOptions.cameraLookAt, _options->guiOptions.cameraUp, _options->guiOptions.cameraFovy);

	float xmin = -(_options->gridDatabaseOptions.gridSizeX / 2.0f);
	float xmax = (_options->gridDatabaseOptions.gridSizeX / 2.0f);
	float zmin = -(_options->gridDatabaseOptions.gridSizeZ / 2.0f);
	float zmax = (_options->gridDatabaseOptions.gridSizeZ / 2.0f);


	if (_options->engineOptions.numThreads != 1) {
		throw GenericException("multi-threading not supported yet");
	}

	_spatialDatabase = new GridDatabase2D(xmin, xmax, zmin, zmax, _options->gridDatabaseOptions.numGridCellsX, _options->gridDatabaseOptions.numGridCellsZ, _options->gridDatabaseOptions.maxItemsPerGridCell, _options->gridDatabaseOptions.drawGrid);



	// load the modules that were requested.
	std::set<std::string>::iterator moduleIter;
	for (moduleIter=_options->engineOptions.startupModules.begin(); moduleIter != _options->engineOptions.startupModules.end(); ++moduleIter) {
		// CAREFUL - _loadModule() is being called, not loadModule().
		_loadModule(*moduleIter,  _options->engineOptions.moduleSearchPath);
	}


	// At this point all modules are loaded but not initialized.
	// The engine is properly initialized, though.  To initialize modules, the engine should
	// be in the READY state, so that modules can load/unload other modules.
	_engineState.transitionToState(ENGINE_STATE_READY);


	// Modules are initialized in execution order, to avoid the possibly random ordering from the comand line, and also the fact that
	// other dependency modules may have been auto-loaded.
	for (unsigned int i=0; i<_modulesInExecutionOrder.size(); i++) {
		SteerLib::ModuleMetaInformation * moduleMetaInfo;
		moduleMetaInfo = _moduleMetaInfoByReference[_modulesInExecutionOrder[i]];

		if (!moduleMetaInfo->isInitialized) {
			moduleMetaInfo->isInitialized = true;  // set this BEFORE calling init.
			_modulesInExecutionOrder[i]->init( _options->getModuleOptions(moduleMetaInfo->moduleName), this);
		}
	}

}



//========================================

void SimulationEngine::finish()
{
	_engineState.transitionToState(ENGINE_STATE_CLEANING_UP);

	// In case any modules are sensitive to order of execution, modules are finished and unloaded IN REVERSE ORDER.
	// Reverse order represents that modules with the most dependencies are finished first,
	// so that the data they may require from other modules will not be destroyed prematurely.

	// Also note that unloading will recursively unload dependencies that are no longer used by any 
	// other modules.  Therefore, the safest and fastest way to unload all leftover modules is to always
	// unload the module from the back of the execution array, since no other modules depend on that one.


	// first, call finish on all modules.
	for (int i=_modulesInExecutionOrder.size()-1; i>=0; i--) {
		_modulesInExecutionOrder[i]->finish();
	}

	// if modules did not clean up agents (they should), we can compensate user-friendly here.
	if (_agents.size() != 0) {
		std::vector<SteerLib::AgentInterface*>::iterator agentIterator;
		for (agentIterator = _agents.begin(); agentIterator != _agents.end(); ++agentIterator) {
			_agentOwners[(*agentIterator)]->destroyAgent(*agentIterator);
		}
		_agents.clear();
		_agentOwners.clear();
	}
	_selectedAgents.clear();

	// unload modules in reverse execution order.
	// note that at each iteration, unloadModule removes one or more modules from _modulesInExecutionOrder.
	while (_modulesInExecutionOrder.size() != 0) {
		_unloadModule(_modulesInExecutionOrder.back(), true, true);
	}

	assert(_moduleMetaInfoByName.size() == 0);
	assert(_moduleMetaInfoByReference.size() == 0);
	assert(_modulesInExecutionOrder.size() == 0);
	assert(_moduleConflicts.size() == 0);

	if (_spatialDatabase != NULL) delete _spatialDatabase;
	_commands.clear();
	//_clock cleanup??
	//_camera cleanup??

	_engineState.transitionToState(ENGINE_STATE_FINISHED);

}



//========================================

void SimulationEngine::initializeSimulation()
{
	if (_modulesInExecutionOrder.size() == 0) {
		throw GenericException("There are no modules loaded.\nPlease specify modules from the command line, config file, or user interface.");
	}

	_engineState.transitionToState(ENGINE_STATE_LOADING_SIMULATION);

	_clock.reset();

	// iterate over all modules asking them to initialize.
	std::vector<SteerLib::ModuleInterface*>::iterator iter;
	for ( iter = _modulesInExecutionOrder.begin(); iter != _modulesInExecutionOrder.end();  ++iter ) {
		(*iter)->initializeSimulation();
	}

	_engineState.transitionToState(ENGINE_STATE_SIMULATION_LOADED);
}

//========================================

void SimulationEngine::cleanupSimulation()
{
	_engineState.transitionToState(ENGINE_STATE_UNLOADING_SIMULATION);

	std::vector<SteerLib::ModuleInterface*>::iterator iter;
	for ( iter = _modulesInExecutionOrder.begin(); iter != _modulesInExecutionOrder.end();  ++iter ) {
		(*iter)->cleanupSimulation();
	}

	_clock.reset();

	_engineState.transitionToState(ENGINE_STATE_READY);
}

//========================================

void SimulationEngine::preprocessSimulation()
{
	_engineState.transitionToState(ENGINE_STATE_PREPROCESSING_SIMULATION);

	std::vector<SteerLib::ModuleInterface*>::iterator iter;

	for ( iter = _modulesInExecutionOrder.begin(); iter != _modulesInExecutionOrder.end();  ++iter ) {
		(*iter)->preprocessSimulation();
	}

	_engineState.transitionToState(ENGINE_STATE_SIMULATION_READY_FOR_UPDATE);
}


//========================================

void SimulationEngine::postprocessSimulation()
{
	_engineState.transitionToState(ENGINE_STATE_POSTPROCESSING_SIMULATION);

	std::cout << "Simulated " << _numFramesSimulated << " frames." << std::endl;

	std::vector<SteerLib::ModuleInterface*>::iterator iter;
	for ( iter = _modulesInExecutionOrder.begin(); iter != _modulesInExecutionOrder.end();  ++iter ) {
		(*iter)->postprocessSimulation();
	}

	_engineState.transitionToState(ENGINE_STATE_SIMULATION_FINISHED);
}


//========================================

bool SimulationEngine::update( bool advanceRealTimeOnly )
{
	// @todo
	//   is this fair to place this error check here? or is it more fair to put this inside of _simulateOneStep? 
	if (_engineState.getCurrentState() != SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE) {
		throw GenericException("Cannot run the next simulation step, the engine is not in the correct state.");
	}

	if (!advanceRealTimeOnly) {
		// update real-time aspects of the simulation
		_clock.advanceSimulationAndUpdateRealTime();
		_camera.update(_clock.getCurrentRealTime(), _clock.getRealDt());

		// Run the actual simulation step, taking the appropriate action based on its return value.
		if (_simulateOneStep() == true) {
			return true;
		} else {
			_engineState.transitionToState(ENGINE_STATE_SIMULATION_NO_MORE_UPDATES_ALLOWED);
			return false;
		}
	}
	else {
		// when paused, update only the real-time aspects of the simulation.
		_clock.updateRealTime();
		_camera.update(_clock.getCurrentRealTime(), _clock.getRealDt());
		return true;
	}

	// should not get here
	std::cerr << "INTERNAL ERROR: did not expect to get here in SimulationEngine::update()\n";
	assert(false);
}

//========================================

bool SimulationEngine::_simulateOneStep()
{
	unsigned int numDisabledAgents = 0;
	float currentSimulationTime = _clock.getCurrentSimulationTime();
	float simulatonDt = _clock.getSimulationDt();
	unsigned int currentFrameNumber = _clock.getCurrentFrameNumber();


	// call preprocess for all modules
	std::vector<SteerLib::ModuleInterface*>::iterator moduleIterator;
	for ( moduleIterator = _modulesInExecutionOrder.begin(); moduleIterator != _modulesInExecutionOrder.end();  ++moduleIterator ) {
		(*moduleIterator)->preprocessFrame(currentSimulationTime, simulatonDt, currentFrameNumber);
	}

	// call updateAI for all agents
	std::vector<SteerLib::AgentInterface*>::iterator agentIterator;
	for ( agentIterator = _agents.begin(); agentIterator != _agents.end(); ++agentIterator )
	{
		if ((*agentIterator)->enabled()){
			(*agentIterator)->updateAI(currentSimulationTime, simulatonDt, currentFrameNumber);
		}
		else {
			if((*agentIterator)->finished()) {	//for most AIs, this will in turn call enabled() and duplicate original behavior; ShadowAI overrides this behavior
				numDisabledAgents++;
			}
		}
	}

	// call postprocess for all modules
	for ( moduleIterator = _modulesInExecutionOrder.begin(); moduleIterator != _modulesInExecutionOrder.end();  ++moduleIterator ) {
		(*moduleIterator)->postprocessFrame(currentSimulationTime, simulatonDt, currentFrameNumber);
	}

	_numFramesSimulated++;


	// indicate that we're done (return false) if we've already played the desired number of frames.
	if ((_options->engineOptions.numFramesToSimulate != 0) && (_numFramesSimulated >= _options->engineOptions.numFramesToSimulate))
		return false;

	// indicate that we're done (return false) if all agents were disabled in this frame.
	// Disabling exit when all agents have finished simulating. 
	if (numDisabledAgents == _agents.size())
		return false;

	// Force stop by some other module
	if (_stop)
		return false;

	return true;
}


//========================================

#ifdef ENABLE_GUI
void SimulationEngine::processKeyboardInput(int key, int action)
{
	// the engine does not directly handle any user's keyboard events;
	// it is up to modules or engine drivers to handle it.
	std::vector<SteerLib::ModuleInterface*>::iterator moduleIterator;
	for ( moduleIterator = _modulesInExecutionOrder.begin(); moduleIterator != _modulesInExecutionOrder.end();  ++moduleIterator ) {
		(*moduleIterator)->processKeyboardInput(key, action);
	}
}
#endif // ifdef ENABLE_GUI


//========================================

#ifdef ENABLE_GUI
void SimulationEngine::initGL()
{
	DrawLib::setBackgroundColor(_options->guiOptions.backgroundColor);

	// enable depth buffer clears  - TODO does it "enable" them, or just set the depth value itself?
	glClearDepth(1.0f);

	// select smooth shading
	glShadeModel(GL_SMOOTH);

	// enable and select depth test
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// turn on backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// enable blending and set typical "blend into frame buffer" mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// reset modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// clear accum buffer
	glClearAccum(0.f, 0.f, 0.f, 0.f);

	// setup lighting
	DrawLib::enableLights();


	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);


	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glLineWidth(_options->guiOptions.lineWidth);
}
#endif // ifdef ENABLE_GUI


//========================================

#ifdef ENABLE_GUI
void SimulationEngine::resizeGL(int width, int height)
{
	// save the new width and height
	_options->glfwEngineDriverOptions.windowSizeX = width;
	_options->glfwEngineDriverOptions.windowSizeY = height;

	// set viewport to full window
	glViewport(0, 0, width, height);

	// set perspective transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const GLfloat w = (float)width;
	const GLfloat h = (float)height;
	const GLfloat aspectRatio = (height == 0) ? 1 : w/h;
	const GLfloat fieldOfViewY = _camera.fovy();
	const GLfloat hither = 1;
	const GLfloat yon = 4000;
	
	gluPerspective(fieldOfViewY, aspectRatio, hither, yon);

	// leave in modelview mode
	glMatrixMode(GL_MODELVIEW);
}
#endif  // ifdef ENABLE_GUI


//========================================
#ifdef ENABLE_GUI
void SimulationEngine::_drawEnvironment()
{
	// visualize the grid database
	_spatialDatabase->draw();
}
#endif  // ifdef ENABLE_GUI


//========================================
#ifdef ENABLE_GUI
void SimulationEngine::_drawModules()
{
	std::vector<SteerLib::ModuleInterface*>::iterator moduleIterator;
	for ( moduleIterator = _modulesInExecutionOrder.begin(); moduleIterator != _modulesInExecutionOrder.end();  ++moduleIterator ) {
		(*moduleIterator)->draw();
	}
}
#endif  // ifdef ENABLE_GUI


//========================================
#ifdef ENABLE_GUI
void SimulationEngine::_drawObstacles()
{
	std::set<SteerLib::ObstacleInterface *>::iterator obstacleIter;
	for (obstacleIter = _obstacles.begin(); obstacleIter!=_obstacles.end(); ++obstacleIter) {
		(*obstacleIter)->draw();
	}
}
#endif  // ifdef ENABLE_GUI


//========================================
#ifdef ENABLE_GUI
void SimulationEngine::_drawAgents()
{
	std::vector<SteerLib::AgentInterface*>::iterator agentIterator;
	for ( agentIterator = _agents.begin(); agentIterator != _agents.end(); ++agentIterator ) {
		if ((*agentIterator)->enabled()){
			(*agentIterator)->draw();
		}
	}
}
#endif  // ifdef ENABLE_GUI



//========================================
#ifdef ENABLE_GUI
void SimulationEngine::draw()
{
	_drawEnvironment();
	_drawObstacles();
	_drawAgents();
	_drawModules();
}
#endif  // ifdef ENABLE_GUI


//========================================

SteerLib::ModuleInterface * SimulationEngine::getModule(const std::string & moduleName)
{
	std::map<std::string, ModuleMetaInformation*>::iterator iter;
	iter = _moduleMetaInfoByName.find(moduleName);
	if (iter == _moduleMetaInfoByName.end()) {
		// no module by that name was found.
		return NULL;
	}
	else {
		return ((*iter).second)->module;
	}
}


//========================================

SteerLib::ModuleMetaInformation * SimulationEngine::getModuleMetaInfo(const std::string & moduleName)
{
	std::map<std::string, ModuleMetaInformation*>::iterator iter;
	iter = _moduleMetaInfoByName.find(moduleName);
	if (iter == _moduleMetaInfoByName.end()) {
		// no module by that name was found.
		return NULL;
	}
	else {
		return ((*iter).second);
	}
}


//========================================

SteerLib::ModuleMetaInformation * SimulationEngine::getModuleMetaInfo(SteerLib::ModuleInterface * module)
{
	if (module==NULL) {
		throw GenericException("Cannot get module meta information, the given module pointer was NULL.");
	}

	std::map<SteerLib::ModuleInterface*, SteerLib::ModuleMetaInformation*>::iterator iter;
	iter = _moduleMetaInfoByReference.find(module);
	if (iter == _moduleMetaInfoByReference.end()) {
		// no module with this pointer was found.
		// if the user had the wrong pointer, it should be considered a bug, so throw an exception here.
		throw GenericException("Cannot get module meta information, the engine did not recognize the given pointer as a valid module.");
	}
	else {
		return ((*iter).second);
	}
}


//========================================

void SimulationEngine::loadModule(const std::string & moduleName, const std::string & searchPath, const std::string & options)
{
	_engineState.transitionToState(ENGINE_STATE_LOADING_MODULE);

	_loadModule(moduleName, searchPath);

	// modify the list of options (overrides command line)
	_options->mergeModuleOptions(moduleName, options);

	// other modules (dependencies) may have been auto-loaded, so we must iterate
	// over all modules and initialized any un-initialized modules.
	SteerLib::ModuleMetaInformation * tempModuleMetaInfo = NULL;
	std::vector<SteerLib::ModuleInterface*>::iterator moduleIter;
	for (moduleIter = _modulesInExecutionOrder.begin(); moduleIter != _modulesInExecutionOrder.end(); ++moduleIter) {
		tempModuleMetaInfo = _moduleMetaInfoByReference[(*moduleIter)];
		if (!tempModuleMetaInfo->isInitialized) {
			tempModuleMetaInfo->isInitialized = true; // set this BEFORE calling init.
			(*moduleIter)->init(_options->getModuleOptions(tempModuleMetaInfo->moduleName), this);
		}
	}

	_engineState.transitionToState(ENGINE_STATE_READY);
}

//========================================

SteerLib::ModuleMetaInformation * SimulationEngine::_loadModule(const std::string & moduleName, const std::string & searchPath)
{
	// check that the requested moduleName is not already "blacklisted" as a conflict from some other loaded module.
	if (_moduleConflicts.find(moduleName) != _moduleConflicts.end()) {
		std::stringstream conflictingNames;
		std::multimap<std::string, std::string>::iterator iter;
		pair< std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator > bounds;
		bounds = _moduleConflicts.equal_range(moduleName);
		for ( iter = bounds.first; iter != bounds.second; ++iter ) {
			conflictingNames << "  " << (*iter).second << "\n";
		}
		throw GenericException("Cannot load module \"" + moduleName + "\", because it conflicts with the following modules:\n" + conflictingNames.str());
	}

	ModuleMetaInformation * newMetaInfo = NULL;
	std::map<std::string, ModuleMetaInformation*>::iterator iter;

	// THREE POSSIBLE STATES OF THE MODULE when we are trying to load it:
	//  1. metaInfo not allocated
	//  2. metaInfo allocated, but module not loaded
	//  3. metaInfo allocated and module loaded.
	iter = _moduleMetaInfoByName.find(moduleName);
	if (iter == _moduleMetaInfoByName.end()) {
		// CASE #1: module was not allocated yet.
		// This is the usual case, and module loading can proceed normally.
		// action: go ahead and allocate it here, and then finish loading it below.
		newMetaInfo = new ModuleMetaInformation;
		newMetaInfo->isLoaded = false;
		newMetaInfo->isInitialized = false;
		// note, this syntax inserts the newMetaInfo when it does not already exist (which is the case here).
		_moduleMetaInfoByName[moduleName] = newMetaInfo;
	}
	else {
		newMetaInfo = ((*iter).second);
		if (!newMetaInfo->isLoaded) {
			// CASE #2: module is allocated, not loaded.
			// That means the module was recursively loading dependencies, and somehow 
			// ended up trying to re-load itself. In other words, it is a cyclic dependency.
			// action: throw an exception.
			/// @todo make a more informative error message here.
			throw GenericException("Detected a cyclic dependency while loading a module.  Please examine a stack trace to see more details.");
		}
		else {
			// CASE #4: module is already loaded.
			// action: just return.
			return newMetaInfo;
		}
	}


	// at this point we are ready to "load" the module.
	// "loading" the module means that we can initialize everything in the ModuleMetaInformation struct
	// and update all the engine's data structures that have knowledge of the module.  Essentially,
	// everything except establishing execution order and calling init().

	ModuleInterface * newModule = NULL;
	DynamicLibrary * newModuleLib = NULL;

	// first, check if the requested module is built-in, and create it if it was built in.
	newModule = _createBuiltInModule(moduleName);

	if (newModule == NULL) {
		// In this case, the module was not built-in.
#ifdef _WIN32
		std::string extension = ".dll";
#else
		std::string extension = ".o";
#endif

		std::string moduleFileName = searchPath + moduleName + extension;
		if (!isExistingFile(moduleFileName)) {
			moduleFileName = _options->engineOptions.moduleSearchPath + moduleName + extension;  // if module wasn't found in the searchPath directory, try with the default search path.
			if (!isExistingFile(moduleFileName)) {
				moduleFileName = moduleName + extension;  // if it still wasnt found, try without the search path
				if (!isExistingFile(moduleFileName)) {
					// if it still didn't work, then cause an error.
					throw GenericException("Could not find the module named \"" + moduleName + "\".\n" +
						"  tried user-specified search path: " + searchPath + moduleName + extension +"\n" +
						"  tried engine's search path: " + _options->engineOptions.moduleSearchPath + moduleName + extension +"\n" +
						"  tried the current directory: " + moduleName + extension +"\n");
				}
			}
		}


		// load the dynamic library
		newModuleLib = new DynamicLibrary(moduleFileName);

		// get the "createModule" function from the dynamic library
		typedef ModuleInterface* (*createModuleFuncPtr)();
		createModuleFuncPtr createModule = (createModuleFuncPtr) newModuleLib->getSymbol("createModule", true);

		// create the module itself
		newModule = createModule();
		if (newModule == NULL) {
			throw GenericException("Could not create module \"" + moduleName + "\", createModule() returned NULL.");
		}
	}


	// the next several chunks of code initialize newMetaInfo:

	// get the dependencies and conflicts from the module, and parse them.
	std::istringstream conflicts(newModule->getConflicts());
	std::istringstream dependencies(newModule->getDependencies());
	std::string token;
	while (conflicts >> token) {
		// the token is one of the conflicts declared by the module
		// make sure it does not yet exist in the set of loaded modules
		// and add it to the list of conflicting modules
		if (_moduleMetaInfoByName.find(token) != _moduleMetaInfoByName.end()) {
			// if the token is found here, that means we have a conflict.
			throw GenericException("Cannot load module \"" + moduleName + "\", because it conflicts with already loaded module \"" + (*_moduleMetaInfoByName.find(token)).second->moduleName);
		}
		newMetaInfo->conflicts.insert(token);
		_moduleConflicts.insert( std::pair<std::string, std::string>(token , moduleName));
	}


	// initialize the simpler data of meta information here
	newMetaInfo->moduleName = moduleName;
	newMetaInfo->module = newModule;
	newMetaInfo->dll = newModuleLib;  // note, this will be NULL for a built-in library.

	newMetaInfo->isLoaded = true;

	// at this point, everything in newMetaInfo should be initialized except for:
	//  - isLoaded is still false until we finish loading below
	//  - isInitialized is still false until we call init() later, outside this function
	//  - list of dependencies is not populated until we load the dependencies
	//  - list of modulesDependentOnThis is initialized at this point, but its contents may change, 
	//    if other modules are loaded that are dependent on this.

	// next, add the newMetaInfo and module to the engine's organizational data structures.
	// _moduleMetaInfoByName is updated earlier
	// _modulesInExecutionOrder is updated after loading all dependencies, so that execution order is maintained
	_moduleMetaInfoByReference[newModule] = newMetaInfo;

	//================
	// everything is loaded except for dependencies; recursively load all dependencies here.
	while (dependencies >> token) {
		ModuleMetaInformation * dependency = _loadModule(token,searchPath);
		dependency->modulesDependentOnThis.insert(newMetaInfo);
		newMetaInfo->dependencies.insert(dependency);
	}

	//================
	// If an entry in the moduleOptionsDatabase did not yet exist, then create one.
	// recall that the [] operator for an STL map will create a new entry if one did not exist by that key already,
	_options->moduleOptionsDatabase[newMetaInfo->moduleName];

	//================
	// if all went well up to this point, the module and its dependencies is loaded, so add it to the end of the list of modules
	// (i.e. it executes after all its dependencies) and return!
	_modulesInExecutionOrder.push_back(newModule);
	std::cout << "loaded module " << newMetaInfo->moduleName << "\n";

	return newMetaInfo;
}


void SimulationEngine::unloadModule( SteerLib::ModuleInterface * moduleToDestroy, bool recursivelyUnloadDependencies )
{
	_engineState.transitionToState(ENGINE_STATE_UNLOADING_MODULE);

	// this wrapper is necessary because the last boolean parameter should not be exposed to the user.
	_unloadModule(moduleToDestroy, recursivelyUnloadDependencies, true);

	// note that the engine state machine will still check whether this is a valid transition, which is a good thing.
	_engineState.transitionToState(ENGINE_STATE_READY);
}

bool SimulationEngine::_unloadModule( SteerLib::ModuleInterface * moduleToDestroy, bool recursivelyUnloadDependencies, bool errorIfCannotUnload )
{
	// grab the reference to its meta information; this implicitly makes sure the module actually exists, too
	SteerLib::ModuleMetaInformation * moduleMetaInfoToDestroy = getModuleMetaInfo(moduleToDestroy);

	// if modules are dependent on this module, then we cannot unload it.
	bool cannotUnload = (moduleMetaInfoToDestroy->modulesDependentOnThis.size() != 0);
	if (cannotUnload) {
		if (errorIfCannotUnload) {
			throw GenericException("Cannot unload module, other loaded modules are dependent on it.\n");
		}
		else {
			// if it wasn't considered an error that we couldn't unload, then just return.
			return false;
		}
	}

	// tell the dependency we are no longer dependent on it, and possibly recursively unload dependencies.
	std::set<ModuleMetaInformation*>::iterator dependencyIter;
	for (dependencyIter = moduleMetaInfoToDestroy->dependencies.begin(); dependencyIter != moduleMetaInfoToDestroy->dependencies.end(); ++dependencyIter) {
		(*dependencyIter)->modulesDependentOnThis.erase(moduleMetaInfoToDestroy);
		if (recursivelyUnloadDependencies) {
			// If the user indicated so, recursively remove the module's dependencies
			// note that if the dependency cannot be unloaded yet, its not an error.
			_unloadModule((*dependencyIter)->module, true, false );
		}
	}

	// at this point we really can begin to remove the module:
	// first, remove this module from the engine's data structures
	_moduleMetaInfoByName.erase(moduleMetaInfoToDestroy->moduleName);
	_moduleMetaInfoByReference.erase(moduleToDestroy);
	std::vector<SteerLib::ModuleInterface*>::iterator moduleExecIter = _modulesInExecutionOrder.begin();
	while ((*moduleExecIter) != moduleToDestroy) { ++moduleExecIter; }
	_modulesInExecutionOrder.erase(moduleExecIter);
	std::set<std::string>::iterator conflictsIter;
	for (conflictsIter = moduleMetaInfoToDestroy->conflicts.begin(); conflictsIter != moduleMetaInfoToDestroy->conflicts.end(); ++conflictsIter) {
		_moduleConflicts.erase(*conflictsIter);
	}

	// then destroy the module, two different ways to do this depending on if the module was built-in or dynamic.
	if (moduleMetaInfoToDestroy->dll == NULL) {
		// for a built-in library we can directly delete the module
		assert(moduleToDestroy != NULL);
		delete moduleToDestroy;
		moduleToDestroy = NULL;
	}
	else {
		// for a dynamic library, get the destroyModule function from the dll and use it to de-allocate the module
		typedef void (*destroyModuleFuncPtr)(SteerLib::ModuleInterface* module);
		destroyModuleFuncPtr destroyModule = (destroyModuleFuncPtr) moduleMetaInfoToDestroy->dll->getSymbol("destroyModule",true);
		destroyModule(moduleToDestroy);
		moduleToDestroy = NULL;
	}

	// finally unload the dll and de-allocate the corresponding moduleMetaInformation structure.
	if (moduleMetaInfoToDestroy->dll != NULL) {
		// dll is NULL if it is a built-in library, we enter this block only if it was a dynamic library.
		moduleMetaInfoToDestroy->dll->unload();
		delete moduleMetaInfoToDestroy->dll;
		moduleMetaInfoToDestroy->dll = NULL;
	}
	delete moduleMetaInfoToDestroy;
	moduleMetaInfoToDestroy = NULL;

	return true;
}


//========================================
void SimulationEngine::getListOfKnownBuiltInModules(std::vector<std::string> & moduleNames)
{
	moduleNames.push_back("dummyAI");
	moduleNames.push_back("testCasePlayer");
	moduleNames.push_back("recFilePlayer");
	moduleNames.push_back("simulationRecorder");
	moduleNames.push_back("metricsCollector");
	moduleNames.push_back("steerBench");
	moduleNames.push_back("steerBug");
}


//========================================
void SimulationEngine::getListOfKnownPlugInModules(std::vector<std::string> & moduleNames, const std::string & searchPath)
{

#ifdef _WIN32
	std::string extension = ".dll";
#else
	std::string extension = ".o";
#endif


	if (isExistingDirectory(searchPath)) {

		std::vector<std::string> fileNames;

		// get a list of all files with the dynamic library extension.
		getFilesInDirectory(searchPath, extension, fileNames);

		// for each dynamic library, test to see if it is really a SteerSuite module.
		for (unsigned int i=0; i<fileNames.size(); i++) {

			std::string moduleName = Util::basename(fileNames[i], extension);

			if (getModule(moduleName) == NULL) {
				// if the module is not already loaded, then
				// load the dll to see if it really is a module.
				DynamicLibrary dll(searchPath+"/"+fileNames[i]);
				if (dll.getSymbol("createModule",false) != NULL) {
					// Then this dynamic library is a module
					moduleNames.push_back(moduleName);
				}
				dll.unload();
			}
			else {
				// if the module was already loaded, then of course it is a module...
				moduleNames.push_back(moduleName);
			}
		}
	}
	else {
		// the module search path is not a valid directory, so no modules will be found
		// which means there is nothing to list here.
	}
}

SteerLib::ModuleInterface * SimulationEngine::_createBuiltInModule(const std::string & moduleName)
{
	if (moduleName == "dummyAI") {
		return new DummyAIModule();
	}
	else if (moduleName == "testCasePlayer" ) {
		return new TestCasePlayerModule();
	}
	else if (moduleName == "recFilePlayer" ) {
		return new RecFilePlayerModule();
	}
	else if (moduleName == "simulationRecorder" ) {
		return new SimulationRecorderModule();
	}
	else if (moduleName == "metricsCollector" ) {
		return new MetricsCollectorModule();
	}
	else if (moduleName == "steerBench" ) {
		return new SteerBenchModule();
	}
	else if (moduleName == "steerBug" ) {
		return new SteerBugModule();
	}
	else {
		return NULL;
	}
}


//========================================

SteerLib::AgentInterface * SimulationEngine::createAgent(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::ModuleInterface * owner)
{
	SteerLib::AgentInterface * newAgent = owner->createAgent();

	if (newAgent != NULL) {
		newAgent->reset(initialConditions,this);
		_agents.push_back(newAgent);
		_agentOwners[newAgent] = owner;
	}

	return newAgent;
}

//========================================

void SimulationEngine::destroyAgent(SteerLib::AgentInterface * agentToDestroy)
{
	/// @todo
	///   - this removal method is not entirely robust, and makes un-wise assumptions
	///     fix it after changing _agents data type from std::vector to std::set
	if (agentToDestroy != NULL)
	{

		// find the module that owns this agent; this also implicitly makes sure agent actually was known to the engine.
		SteerLib::ModuleInterface * module =  NULL;
		std::map<SteerLib::AgentInterface*, SteerLib::ModuleInterface*>::iterator moduleIter;
		moduleIter = _agentOwners.find(agentToDestroy);
		if (moduleIter == _agentOwners.end())
		{
			throw GenericException("Cannot destroy agent because the engine did not have a record of the agent.  Are you sure you used SimulationEngine::createAgent() or SimulationEngine::addAgent()?");
		}
		else {
			module = (*moduleIter).second;
		}

		// remove the agent from the list of _agents
		// this is unnecessarily costly because we use std::vector, and we must iterate through the entire list to find it
		// but for now it seems like an acceptable trade-off because this is not in a performace-critical place, and 
		// the ability to index into _agents is important in some other places.
		std::vector<SteerLib::AgentInterface*>::iterator agentIter;
		agentIter = std::find( _agents.begin(), _agents.end(), agentToDestroy);
		// if this assertion happens, we have a bug, since we expected that the agent will be in both _agentOwners and _agents.
		assert(agentIter != _agents.end());

		// the swap-n-pop method avoids a linear-time cost for removing something in the array
		// but does not preserve the order of agents.
		swap((*agentIter), _agents.back());
		_agents.pop_back();


		// remove the agent from the list of owners
		_agentOwners.erase(agentToDestroy);

		// destroy the agent
		module->destroyAgent(agentToDestroy);
	}
}

//========================================

void SimulationEngine::destroyAllAgentsFromModule(SteerLib::ModuleInterface * owner)
{
	/// @todo: there may be a better way to implement this?
	// 0x9d97330
#ifdef _DEBUG
	std::cout << "about to destroyAllAgents and ModuleInterface is " << owner << "\n";
	std::cout << "agents.size() = " << _agents.size() << "\n";
#endif
	for (int i = _agents.size()-1; i >= 0; i--)
	{
		if (_agentOwners[_agents[i]] == owner)
		{
#ifdef _DEBUG
	std::cout << "Destroying agent\n";
#endif
			destroyAgent(_agents[i]);
		}
	}
}

//========================================

void SimulationEngine::addAgent(SteerLib::AgentInterface * newAgent, SteerLib::ModuleInterface * owner)
{
	// make sure the agent does not already exist in the engine's data structures.
	if (_agentOwners.find(newAgent) != _agentOwners.end()) {
		throw GenericException("Cannot add agent, agent already exists.\n");
	}

	_agents.push_back(newAgent);
	_agentOwners[newAgent] = owner;
}

//========================================

void SimulationEngine::removeAgent(SteerLib::AgentInterface * agentToRemove)
{
	std::vector<SteerLib::AgentInterface*>::iterator agentIter;
	agentIter = std::find( _agents.begin(), _agents.end(), agentToRemove);
	if (agentIter == _agents.end()) {
		throw GenericException("Cannot remove agent because the engine did not have a record of the agent.  Are you sure you used SimulationEngine::createAgent() or SimulationEngine::addAgent()?");
	}

	// the swap-n-pop method avoids a linear-time cost for removing something in the array
	// but does not preserve the order of agents.
	swap((*agentIter), _agents.back());
	_agents.pop_back();

	// remove the agent from the list of owners
	_agentOwners.erase(agentToRemove);
}

/*
//========================================

SteerLib::ObstacleInterface * SimulationEngine::createObstacle( const SteerLib::ObstacleInitialConditions & initialConditions, SteerLib::ModuleInterface * module)
{
	throw GenericException("engine createObstacle() not implemented yet!");
}

//========================================

void SimulationEngine::destroyObstacle(SteerLib::ObstacleInterface * obstacleToDestroy)
{
	throw GenericException("engine destroyObstacle() not implemented yet!");
}

//========================================

void SimulationEngine::destroyAllObstaclesFromModule(SteerLib::ModuleInterface * owner)
{
	throw GenericException("engine destroyAllObstaclesFromModule() not implemented yet!");
}
*/

//========================================

void SimulationEngine::addObstacle(SteerLib::ObstacleInterface * newObstacle)
{
	_obstacles.insert(newObstacle);
}


//========================================

void SimulationEngine::removeObstacle(SteerLib::ObstacleInterface * obstacleToRemove)
{
	_obstacles.erase(obstacleToRemove);
}


//========================================

bool SimulationEngine::hasCommand(const std::string & commandName)
{
	return (_commands.find(commandName) != _commands.end());
}


//========================================

void SimulationEngine::addCommand(const std::string & commandName, SteerLib::CommandFunctionPtr commandFunction)
{
	if (hasCommand(commandName)) {
		throw GenericException("Command \"" +commandName + "\" already exists in the list of commands.");
	}
	else {
		// the following STL [] operator actually ADDS the element since it does not exist.
		_commands[commandName] = commandFunction;
	}
}


//========================================

void SimulationEngine::removeCommand(const std::string & commandName)
{
	if (!hasCommand(commandName)) {
		throw GenericException("Command \""+commandName+"\" was not found, nothing to remove.");
	}
	else {
		_commands.erase(commandName);
	}
}


//========================================

void SimulationEngine::runCommand(const std::string & commandLine)
{
	std::istringstream ss(commandLine);
	std::string commandName;
	ss >> commandName;
	if (!hasCommand(commandName)) {
		throw GenericException("Command \"" + commandName + "\" was not found, cannot run the command.");
	}
	else {
		_commands[commandName](commandLine);
	}
}



//========================================
void SimulationEngine::_setupStateMachine()
{
	// The engine always starts new, and must initialize to become ready.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_NEW, SimulationEngine::ENGINE_STATE_INITIALIZING );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_INITIALIZING, SimulationEngine::ENGINE_STATE_READY );

	// Once ready, the user can choose to (1) jump straight to cleanup, (2) to load a simulation, (3) to load a module, or (4) to unload a module.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_READY, SimulationEngine::ENGINE_STATE_CLEANING_UP );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_READY, SimulationEngine::ENGINE_STATE_LOADING_SIMULATION );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_READY, SimulationEngine::ENGINE_STATE_LOADING_MODULE );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_READY, SimulationEngine::ENGINE_STATE_UNLOADING_MODULE );

	// if a module was being loaded or unloaded, the state will return to "ready" after the load/unload operation is finished.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_LOADING_MODULE, SimulationEngine::ENGINE_STATE_READY );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_UNLOADING_MODULE, SimulationEngine::ENGINE_STATE_READY );

	// If the user decided to load a simulation, then the simulation becomes loaded, and the user has the choice
	// whether to start the simulation by initiating preprocessing, or to unload the simulation before ever starting it.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_LOADING_SIMULATION, SimulationEngine::ENGINE_STATE_SIMULATION_LOADED );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_LOADED, SimulationEngine::ENGINE_STATE_UNLOADING_SIMULATION );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_LOADED, SimulationEngine::ENGINE_STATE_PREPROCESSING_SIMULATION );

	// If the user decided to run the simulation (by initiating the preprocessing), then the engine is ready for updates after preprocessing.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_PREPROCESSING_SIMULATION, SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE );

	// When the engine is ready for updates, the user can either update the simulation or finish the simulation by initiating post-processing.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE, SimulationEngine::ENGINE_STATE_UPDATING_SIMULATION );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE, SimulationEngine::ENGINE_STATE_SIMULATION_NO_MORE_UPDATES_ALLOWED);
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE, SimulationEngine::ENGINE_STATE_POSTPROCESSING_SIMULATION );

	// After one update, the engine will either be ready for more updates, or indicate that no more updates are allowed.
	// if no more updates are allowed, then the user must initiate post-processing next.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_UPDATING_SIMULATION, SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_UPDATING_SIMULATION, SimulationEngine::ENGINE_STATE_SIMULATION_NO_MORE_UPDATES_ALLOWED );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_NO_MORE_UPDATES_ALLOWED, SimulationEngine::ENGINE_STATE_POSTPROCESSING_SIMULATION );

	// After the user initiates a postprocess, the simulation becomes finished, and then the user
	// must unload the simulation, after which the engine is ready to load another simulation or to finish.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_POSTPROCESSING_SIMULATION, SimulationEngine::ENGINE_STATE_SIMULATION_FINISHED );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_SIMULATION_FINISHED, SimulationEngine::ENGINE_STATE_UNLOADING_SIMULATION );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_UNLOADING_SIMULATION, SimulationEngine::ENGINE_STATE_READY );

	// If the user initiated the engine to cleanup, then the engine becomes finished, after which
	// the engine can be either safely de-allocated, or re-initialized.
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_CLEANING_UP, SimulationEngine::ENGINE_STATE_FINISHED );
	_engineState.addTransition( SimulationEngine::ENGINE_STATE_FINISHED, SimulationEngine::ENGINE_STATE_INITIALIZING );

	// define the starting state and start the state machine.
	_engineState.setStartState( SimulationEngine::ENGINE_STATE_NEW );
	_engineState.setCallback(&_engineStateMachineCallback);
	_engineState.start();
}

//========================================

void SimulationEngine::_dumpModuleDataStructures()
{
	std::cerr << "------------------\n";
	std::cerr << "Modules by name:\n";
	std::cerr << "------------------\n";
	std::map<std::string, SteerLib::ModuleMetaInformation*>::iterator i;
	for (i=_moduleMetaInfoByName.begin(); i!=_moduleMetaInfoByName.end(); ++i) {

		// MUBBASIR THIS DOES NOT COMPILE ON HAMMERHEAD 
		//std::cerr << "  " << (*i).first << " --> " << hex << (unsigned int)(*i).second->module << "\n";

		std::cerr << "  " << (*i).first << " --> " << hex << (uintptr_t)(*i).second->module << "\n";
	}

	std::cerr << "\n----------------------\n";
	std::cerr << "Modules by reference:\n";
	std::cerr << "----------------------\n";
	std::map<SteerLib::ModuleInterface*, SteerLib::ModuleMetaInformation*>::iterator j;
	for (j=_moduleMetaInfoByReference.begin(); j!=_moduleMetaInfoByReference.end(); ++j) {
		
		// MUBBASIR THIS DOES NOT COMPILE ON HAMMERHEAD 
		//std::cerr << "  " << (unsigned int)(*j).first << " --> " << (*j).second->moduleName << "\n";

		std::cerr << "  " << (uintptr_t)(*j).first << " --> " << (*j).second->moduleName << "\n";
	}

	std::cerr << "\n-------------------\n";
	std::cerr << "Module conflicts:\n";
	std::cerr << "-------------------\n";
	std::multimap<std::string, std::string>::iterator m;
	for (m=_moduleConflicts.begin(); m!=_moduleConflicts.end(); ++m) {
		std::cerr << "  " << (*m).first << " --> " << (*m).second << "\n";
	}

	std::cerr << "\n--------------------------------------------\n";
	std::cerr << "Module details, listed in execution order:\n";
	std::cerr << "--------------------------------------------\n";
	for (unsigned int k=0; k<_modulesInExecutionOrder.size(); ++k) {
		ModuleMetaInformation * meta = _moduleMetaInfoByReference[_modulesInExecutionOrder[k]];
		std::cerr << "MODULE " << meta->moduleName << "\n";
		std::cerr << "   module type: " << (meta->dll==NULL ? "built-in" : "dynamic") << "\n";
		std::cerr << "        loaded: " << (meta->isLoaded? "true" : "false") << "\n";
		std::cerr << "   initialized: " << (meta->isInitialized ? "true" : "false") << "\n";
		std::set<ModuleMetaInformation*>::iterator iter;
		std::cerr << "  dependencies:";
		for (iter=meta->dependencies.begin(); iter!=meta->dependencies.end(); ++iter) {
			std::cerr << " " << (*iter)->moduleName;
		}
		std::cerr << "\n";
		std::cerr << "       used by:";
		for (iter=meta->modulesDependentOnThis.begin(); iter!=meta->modulesDependentOnThis.end(); ++iter) {
			std::cerr << " " << (*iter)->moduleName;
		}
		std::cerr << "\n\n";
	}

}

//========================================

void SimulationEngine::EngineStateMachineCallback::transitioned(unsigned int previousState, unsigned int currentState)
{
	switch (currentState) {
		case SimulationEngine::ENGINE_STATE_READY:
			_engine->_simulationLoaded = false;
			_engine->_simulationRunning = false;
			_engine->_simulationDone = false;
			break;
		case SimulationEngine::ENGINE_STATE_SIMULATION_LOADED:
			_engine->_simulationLoaded = true;
			_engine->_simulationRunning = false;
			_engine->_simulationDone = false;
			break;
		case SimulationEngine::ENGINE_STATE_PREPROCESSING_SIMULATION:
			_engine->_simulationLoaded = true;
			_engine->_simulationRunning = true;
			_engine->_simulationDone = false;
			break;
		case SimulationEngine::ENGINE_STATE_SIMULATION_FINISHED:
			_engine->_simulationLoaded = true;
			_engine->_simulationRunning = false;
			_engine->_simulationDone = true;
			break;
		default:
			// nothing to do on other state transitions.
			break;
	}
}

unsigned int SimulationEngine::EngineStateMachineCallback::handleUnknownTransition(unsigned int currentState, unsigned int requestedNewState)
{
	/// @todo
	///   - make a much more meaningful, specific error message depending on the requested transition that is an error.  It may even be possible to transparently tolerate
	///     some of the errors, with only a printed warning.
	throw GenericException("Engine functions were not called in the proper way; tried to transition from state " + toString(currentState) + " to state " + toString(requestedNewState) + ".\nSee documentation for how to use the engine; eventually this error message will be expanded to give a meaningful specific error message.");

}

