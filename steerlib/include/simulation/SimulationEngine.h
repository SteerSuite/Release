//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_SIMULATION_ENGINE_H__
#define __STEERLIB_SIMULATION_ENGINE_H__

/// @file SimulationEngine.h
/// @brief Declares the SimulationEngine class.
///
/// @todo
///   - typedef all the messy complicated STL containers; good for readability and portability of code.
///   - add support/safety for a module to unload itself

#include "interfaces/EngineInterface.h"
#include "util/StateMachine.h"

#define KEY_PRESSED 1

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {

	/**
	 * @brief The main class that handles the simulation.
	 *
	 * This is the central class for the <b>steersim</b> executable, but it can also be instantiated
	 * by users in their own programs.  The main purpose of this
	 * class is to run a simulation; this includes loading and managing any number of
	 * modules, maintaining information about the simulation, and providing functionality
	 * for controlling/interacting with the simulation.
	 *
	 * <h3>Engine drivers</h3>
	 * In our terminology, the SimulationEngine is controlled by an "engine driver", which provides an
	 * interface between the engine and the rest of the program.  Through this interface, the driver can
	 * (optionally) make the engine draw openGL, handle user input, and receive command-line 
	 * options.  SteerSuite provides some existing engine drivers, such as the CommandLineEngineDriver
	 * (for pure command-line execution with no GUI), the GLFWEngineDriver (an openGL window
	 * for visualization and basic user interaction), and a QtEngineDriver for a more advanced GUI.
	 * You can easily create your own engine driver for other purposes, for example, if you want to
	 * run a full-featured engine directly from a different application.
	 *
	 * Note that some places in code we use the term "engine controller".  This is the same thing, and
	 * we will eventually fix the terminology to use only one term.
	 *
	 * <h3>Modules</h3>
	 * The SimulationEngine by itself does not simulate anything meaningful.  The actual contents
	 * of the simulation are determined by whatever modules are loaded.  While SimulationEngine
	 * was originally designed for steering simulations only, it has evolved into a fairly generic
	 * simulator and it is possible to use this engine for other purposes with the right modules,
	 * including animations, physics, etc.
	 *
	 * The engine provides a subset of its functionality to all modules, through 
	 * the SteerLib::EngineInterface virtual interface.  Refer to the documentation of this
	 * virtual interface for more information about how modules can use the SimulationEngine.
	 *
	 * <h3>How to use this engine in your own programs</h3>
	 * For engine drivers, use the engine as follows:
	 *   -# Instantiate the engine, and call init().  This will load any requested modules that do the real work of the simulation.
	 *   -# You can run any number of simulations as follows:
	 *      -# initialize by calling initializeSimulation().
	 *      -# the simulation starts by calling preprocessSimulation().
	 *      -# for each step of the simulation, call update(false).  if update() returns false, then that means
	 *         a module triggered the simulation to finish, and update() should not be called anymore.  Note that
	 *         update(true) can be used when the simulation is "paused".  It will update real-time clock and camera, but
	 *         it will not update any modules, and is not considered a "simulation step".
	 *      -# When updateSimulation() returns false, or when you decide to finish the simulation, call postprocessSimulation().
	 *      -# clean up the simulation by calling cleanupSimulation().
	 *   -# Once you are done, call finish() to clean up the engine, and de-allocate it.
	 *
	 * In addition to this core simulation usage, the engine can render the simulation using openGL by calling draw(), handle keyboard events
	 * with processKeyboardInput(), and provides much more through the SteerLib::EngineInterface functionality.
	 *
	 * For modules, only the SteerLib::EngineInterface functionality is exposed.  Refer to the SteerLib::EngineInterface
	 * documentation for more information.
	 *
	 * <h3>Notes</h3>
	 *   - When a simulation is "paused", then update(true) will still update the real-time clock and camera.
	 *     This way, the engine can still update camera movements and possibly other real-time aspects that
	 *     may arise in the future.  In this case, the simulation clock and modules will not be updated.
	 *
	 * @see
	 *   - SteerLib::EngineInterface is the virtual interface that is given to modules
	 *   - SteerLib::ModuleInterface is the virtual interface implemented by modules
	 *   - CommandLineEngineDriver
	 *   - GLFWEngineDriver
	 *
	 * @todo
	 *   - determine what changes are necessary so that it is possible to pass data across modules using commands.
	 *   - add management of obstacles, similar to agents
	 */
	class STEERLIB_API SimulationEngine : public EngineInterface {
	public:
		SimulationEngine();
		~SimulationEngine() { }
		/**
		 * @brief Enum that describes the various states the engine can be in.
		 *
		 * States that are adjective descriptions are the main states.
		 * States that begin with a verb that ends in "-ing" (ignoring that they are technically adjectives in English grammar, too)
		 * are intermediate states that occur while the engine is performing a task.
		 *
		 * To completely understand the role of these states and their transitions, see the comments in
		 * the protected function _setupStateMachine().
		 */
		enum EngineStateEnum {
			ENGINE_STATE_NEW,
			ENGINE_STATE_INITIALIZING,
			ENGINE_STATE_READY,
			ENGINE_STATE_CLEANING_UP,
			ENGINE_STATE_FINISHED,
			ENGINE_STATE_LOADING_MODULE,
			ENGINE_STATE_UNLOADING_MODULE,
			ENGINE_STATE_LOADING_SIMULATION,
			ENGINE_STATE_SIMULATION_LOADED,
			ENGINE_STATE_PREPROCESSING_SIMULATION,
			ENGINE_STATE_SIMULATION_READY_FOR_UPDATE,
			ENGINE_STATE_UPDATING_SIMULATION,
			ENGINE_STATE_SIMULATION_NO_MORE_UPDATES_ALLOWED,
			ENGINE_STATE_POSTPROCESSING_SIMULATION,
			ENGINE_STATE_SIMULATION_FINISHED,
			ENGINE_STATE_UNLOADING_SIMULATION,
		};


		/// Returns a list of built-in modules that the engine knows about; can be called without calling init().
		void getListOfKnownBuiltInModules(std::vector<std::string> & moduleNames);
		/// Returns a list of plug-in modules that the engine knows about, based on the module search path from user options; can be called without calling init().
		void getListOfKnownPlugInModules(std::vector<std::string> & moduleNames, const std::string & searchPath);
		/// Returns a EngineStateEnum type that represents the engine's current low-level state; if you want to query simpler "state" of the simulation, use isSimulationLoaded(), isSimulationRunning(), and isSimulationDone() instead.
		unsigned int getCurrentState() { return _engineState.getCurrentState(); }

		/// Initializes the engine, automatically loads any modules specified by config options.
		void init(SimulationOptions * options, SteerLib::EngineControllerInterface * engineController);
		/// Unloads all modules, and cleans up the engine.
		void finish();

		/// @name Simulation execution
		//@{
		/// Initializes a simulation by calling ModuleInterface::initializeSimulation() for all modules; called before preprocessSimulation().
		void initializeSimulation();
		/// Cleans up a simulation by calling ModuleInterface::cleanupSimulation() for all modules; called after postprocessSimulation().
		void cleanupSimulation();
		/// Starts a simulation by calling ModuleInterface::preprocessSimulation() for all modules; called after initializeSimulation()
		void preprocessSimulation();
		/// Ends a simulation by calling ModuleInterface::postprocessSimulation() for all modules; called before cleanupSimulation(), after updateSimulation() returns false.
		void postprocessSimulation();
		/// Updates the camera, clock, all modules, and all agents; returns false if the simulation is done; if isPaused is true, the simulation will update the camera and other real-time aspects, but will not simulate anything.
		bool update( bool advanceRealTimeOnly );
		/// stops execution
		void stop();
		//@}
	#ifdef ENABLE_GUI
		/// Handles keyboard input by forwarding the keyboard event to all modules.
		void processKeyboardInput(int key, int action);
		/// Initializes the openGL settings for the engine.
		void initGL();
		/// Handles resizing if the openGL window is re-sized.
		void resizeGL(int width, int height);
		/// Draws the scene using openGL; should only be called after initializing the engine, but a simulation does not have to be loaded.
		void draw();
	#endif

		/// @name EngineInterface functionality
		/// @brief Modules have access to these functions of the engine; these functions are documented in the SteerLib::EngineInterface documentation.
		//@{
		virtual SteerLib::GridDatabase2D * getSpatialDatabase() { return _spatialDatabase; }
		virtual const std::vector<SteerLib::AgentInterface*> & getAgents() { return _agents; }
		virtual const std::set<SteerLib::AgentInterface*> & getSelectedAgents() { return _selectedAgents; }
		virtual const std::set<SteerLib::ObstacleInterface*> & getObstacles() { return _obstacles; }
		virtual SteerLib::ModuleInterface * getModule(const std::string & moduleName);
		virtual SteerLib::ModuleMetaInformation * getModuleMetaInfo(const std::string & moduleName);
		virtual SteerLib::ModuleMetaInformation * getModuleMetaInfo(SteerLib::ModuleInterface * module);
		virtual const std::vector<SteerLib::ModuleInterface*> & getAllModules() { return _modulesInExecutionOrder; }
		virtual SteerLib::Clock & getClock() { return _clock; }
		virtual SteerLib::Camera & getCamera() { return _camera; }
		virtual SteerLib::EngineControllerInterface * getEngineController() { return _engineController; }
		virtual std::string getModuleSearchPath() { return _options->engineOptions.moduleSearchPath; }
		virtual std::string getTestCaseSearchPath() { return _options->engineOptions.testCaseSearchPath; }
		virtual const OptionDictionary & getModuleOptions(const std::string & moduleName) { return _options->getModuleOptions(moduleName); }
		virtual const SimulationOptions & getOptions() { return (*_options); }

		virtual bool isSimulationLoaded() { return _simulationLoaded; }
		virtual bool isSimulationRunning() { return _simulationRunning; }
		virtual bool isSimulationDone() { return _simulationDone; }
		virtual bool isModuleLoaded(const std::string & moduleName) { return (_moduleMetaInfoByName.find(moduleName) != _moduleMetaInfoByName.end()); }
		virtual bool isAgentSelected(SteerLib::AgentInterface * agent) { return (_selectedAgents.find(agent) != _selectedAgents.end()); }
		virtual bool hasCommand(const std::string & commandName);

		virtual void throwException(const std::string & errorMessage) { throw Util::GenericException(errorMessage); }

		virtual void loadModule(const std::string & moduleName, const std::string & searchPath, const std::string & options);
		virtual void unloadModule(SteerLib::ModuleInterface * moduleToDestroy, bool recursivelyUnloadDependencies );

		virtual SteerLib::AgentInterface * createAgent(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::ModuleInterface * owner);
		virtual void destroyAgent(SteerLib::AgentInterface * agentToDestroy);
		virtual void destroyAllAgentsFromModule(SteerLib::ModuleInterface * owner);
		virtual void addAgent(SteerLib::AgentInterface * newAgent, SteerLib::ModuleInterface * owner);
		virtual void removeAgent(SteerLib::AgentInterface * agentToRemove);
		virtual void selectAgent(SteerLib::AgentInterface * agent) { if (agent != NULL) _selectedAgents.insert(agent); }
		virtual void unselectAgent(SteerLib::AgentInterface * agent) { if (agent != NULL) _selectedAgents.erase(agent); }
		virtual void unselectAllAgents() { _selectedAgents.clear(); }

		//virtual SteerLib::ObstacleInterface * createObstacle( const SteerLib::ObstacleInitialConditions & initialConditions, SteerLib::ModuleInterface * module);
		//virtual void destroyObstacle(SteerLib::ObstacleInterface * obstacleToDestroy);
		//virtual void destroyAllObstaclesFromModule(SteerLib::ModuleInterface * owner);
		virtual void addObstacle(SteerLib::ObstacleInterface * newObstacle);
		virtual void removeObstacle(SteerLib::ObstacleInterface * obstacleToRemove );


		virtual void addCommand(const std::string & commandName, SteerLib::CommandFunctionPtr commandFunction);
		virtual void removeCommand(const std::string & commandName);
		virtual void runCommand(const std::string & commandName);

		//@}


	protected:

		/// Clears all data structures and initializes values to dummy values; however, does not de-allocate data, so this should only be used inside of init().
		void _reset();
		/// Runs one step of the simulation
		bool _simulateOneStep();
		/// Just for debugging, dumps out the contents of the engine's organizational data structures
		void _dumpModuleDataStructures();
		/// Returns an instance of a built-in module of name moduleName, or returns NULL if moduleName is not a built-in module.
		SteerLib::ModuleInterface * _createBuiltInModule(const std::string & moduleName);
		/// Loads a module and recursively loads its dependencies.
		SteerLib::ModuleMetaInformation * _loadModule(const std::string & moduleName, const std::string & searchPath);
		/// Unloads a module and, if desired, recursively unloads modules that no longer are used by other modules.
		bool _unloadModule(SteerLib::ModuleInterface * moduleToDestroy, bool recursivelyUnloadDependencies, bool errorIfCannotUnload );
		/// Helper function to initialize and start the engine state machine that makes sure the engine is always in a valid state.
		void _setupStateMachine();

	#ifdef ENABLE_GUI
		void _drawEnvironment();
		void _drawModules();
		void _drawObstacles();
		void _drawAgents();
	#endif

		class EngineStateMachineCallback : public Util::StateMachineCallbackInterface
		{
		public:
			/// Known transitions affect some accessor variables
			void transitioned(unsigned int previousState, unsigned int currentState);
			/// Unknown transitions will occur if the user of the engine tries to call functions at the wrong time.
			unsigned int handleUnknownTransition(unsigned int currentState, unsigned int requestedNewState);

			void setEngine(SimulationEngine * newEngine) { _engine = newEngine; }
			SimulationEngine * _engine;
		};


		/// @name Data structures that organize modules and meta data.
		//@{
		/// maps the name of a module to its meta information
		std::map<std::string, SteerLib::ModuleMetaInformation*> _moduleMetaInfoByName;
		/// maps the pointer to a module to its meta information
		std::map<SteerLib::ModuleInterface*, SteerLib::ModuleMetaInformation*> _moduleMetaInfoByReference;
		/// the modules sorted in order of execution (i.e. modules execute after their dependencies.)
		std::vector<SteerLib::ModuleInterface*> _modulesInExecutionOrder;
		/// maps the name of a conflicting module to the module that declared it a conflict.
		std::multimap<std::string, std::string> _moduleConflicts;
		//@}

		/// @name Data structures to keep track of agents
		//@{
		std::vector<SteerLib::AgentInterface*> _agents;
		std::set<SteerLib::AgentInterface*> _selectedAgents;
		std::map<SteerLib::AgentInterface*, SteerLib::ModuleInterface*> _agentOwners;
		//@}

		/// @name Other objects managed by the engine
		//@{
		std::map<std::string, SteerLib::CommandFunctionPtr> _commands;
		SteerLib::Clock _clock;
		SteerLib::Camera _camera;
		SteerLib::GridDatabase2D * _spatialDatabase;
		std::set<SteerLib::ObstacleInterface*> _obstacles;
		SteerLib::EngineControllerInterface * _engineController;
		//@}


		/// @name Engine state and parameters
		//@{
		unsigned int _numFramesSimulated;
		bool _simulationLoaded;
		bool _simulationRunning;
		bool _simulationDone;
		bool _stop;
		/// State machine to oversee the engine's state and transitions, so that we can robustly and cleanly make sure that the engine remains in a valid state at all times.
		Util::StateMachine _engineState;
		EngineStateMachineCallback _engineStateMachineCallback;
		SimulationOptions * _options;
		//@}

	private:
		// @name Un-implemented functions, for protection
		// @brief The SimulationEngine is not a singleton class, but we disallow it from being copied or assigned; code must either get a reference to an existing engine, or create a new engine.
		//@{
		SimulationEngine(const SimulationEngine & );  // not implemented, not copyable
		SimulationEngine& operator= (const SimulationEngine & );  // not implemented, not assignable
		//@}

	};

} // end namespace SteerLib


#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
