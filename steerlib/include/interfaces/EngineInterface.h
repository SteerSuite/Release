//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_ENGINE_INTERFACE_H__
#define __STEERLIB_ENGINE_INTERFACE_H__

/// @file EngineInterface.h
/// @brief Declares the SteerLib::EngineInterface virtual interface.

#include "Globals.h"
#include "interfaces/SpatialDataBaseInterface.h"
#include "interfaces/PlanningDomainInterface.h"
#include "recfileio/RecFileIO.h"
#include "interfaces/AgentInterface.h"
#include "interfaces/ObstacleInterface.h"
#include "interfaces/EngineControllerInterface.h"
#include "util/DrawLib.h"
#include "util/DynamicLibrary.h"
#include "simulation/Clock.h"
#include "simulation/Camera.h"
#include "simulation/SimulationOptions.h"

namespace SteerLib {

	/// A pointer type used by the EngineInterface, points to a function that executes a custom command.
	typedef void (*CommandFunctionPtr)(const std::string & commandString);

	/**
	 * @brief A data structure that contains meta-information about a module.
	 */
	struct ModuleMetaInformation
	{
		/// The name of the corresponding module.
		std::string moduleName;
		/// Pointer to the actual module.
		SteerLib::ModuleInterface * module;
		/// Pointer to the dynamic library that created the module; NULL for built-in modules.
		Util::DynamicLibrary * dll;
		/// A container that lists all module names that should not be loaded simultaneously with this module.
		std::set<std::string> conflicts;
		/// A container that points to all modules that this module depends on.
		std::set<ModuleMetaInformation*> dependencies;
		/// A container that points to all modules that depend on this module.
		std::set<ModuleMetaInformation*> modulesDependentOnThis;
		/// True if the module is loaded and incorporated into the engine's data structures, false otherwise.
		bool isLoaded;
		/// True if the module was initialized with a call to SteerLib::ModuleInterface::init(), false otherwise.
		bool isInitialized;
	};



	/**
	 * @brief The virtual interface implemented by the engine to expose limited functionality to modules.
	 *
	 * The engine follows specific conventions about how to use modules; these conventions are described in the 
	 * SteerLib::ModuleInterface documentation.
	 *
	 * @see
	 *   - SteerLib::ModuleInterface class, the engine manages these modules which do the real useful work of a simulation.
	 *   - SteerLib::AgentInterface class, the engine updates and draws agents in the simulation.
	 *   - SteerLib::ObstacleInterface class, the engine also keeps track of obstacles in the simulation.
	 *   - SteerLib::EngineControllerInterface class, which exposes some more functionality to modules.
	 */
	class STEERLIB_API EngineInterface {
	public:
		virtual ~EngineInterface() { }

		/// @name Accessor functions
		/// @brief The engine provides access to most of the internals through these accessors so that modules can use them.  Beware that some
		/// of the accessors may not perform well, since the engine must search through data structures to find what you are asking for.
		//@{
		/// Returns a pointer to the SpatialDataBaseInterface contained in the engine.
		virtual SteerLib::SpatialDataBaseInterface * getSpatialDatabase() = 0;
		/// Returns a pointer to the PathPlanningInterface contained in the engine.
		virtual SteerLib::PlanningDomainInterface * getPathPlanner() = 0;
		/// Returns a reference to an STL vector containing a list of agents.
		virtual const std::vector<SteerLib::AgentInterface*> & getAgents() = 0;
		/// Returns a reference to an STL set of selected agents.
		virtual const std::set<SteerLib::AgentInterface*> & getSelectedAgents() = 0;
		/// Returns a reference to an STL set containing a list of all obstacles.
		virtual const std::set<SteerLib::ObstacleInterface*> & getObstacles() = 0;
		/// Returns a pointer to the ModuleInterface of the module with the name moduleName.
		virtual SteerLib::ModuleInterface * getModule(const std::string & moduleName) = 0;
		/// Returns a pointer to the ModuleMetaInformation of the module with the name moduleName.
		virtual SteerLib::ModuleMetaInformation * getModuleMetaInfo(const std::string & moduleName) = 0;
		/// Returns a pointer to the ModuleMetaInformation of the corresponding module.
		virtual SteerLib::ModuleMetaInformation * getModuleMetaInfo(SteerLib::ModuleInterface * module) = 0;
		/// Returns a pointer to an STL vector of all modules.
		virtual const std::vector<SteerLib::ModuleInterface*> & getAllModules() = 0;
		/// Returns a pointer to the SteerLib::Clock instance used by the engine.
		virtual SteerLib::Clock & getClock() = 0;
		/// Returns a pointer to the SteerLib::Camera instance used by the engine.
		virtual SteerLib::Camera & getCamera() = 0;
		/// Returns a pointer to the Engine Driver interface that modules can use.
		virtual SteerLib::EngineControllerInterface * getEngineController() = 0;
		/// Returns a string containing the directory that should be searched when looking for modules.
		virtual std::string getModuleSearchPath() = 0;
		/// Returns a string containing the directory that should be searched when looking for test cases.
		virtual std::string getTestCaseSearchPath() = 0;
		/// Returns an STL map of the options that were specified for all modules, regardless of whether the modules are actually loaded or not.
		virtual const OptionDictionary & getModuleOptions(const std::string & moduleName) = 0;
		/// Returns the options that were used to initialize the engine
		virtual const SimulationOptions & getOptions() = 0;
		// Get the current static triangle geometry of the Engine
		virtual std::pair<std::vector<Util::Point>,std::vector<size_t> > getStaticGeometry() = 0;
		//@}

		/// @name Boolean state queries
		//@{
		/// Returns true if a simulation is loaded, regardless of whether the simulation is running or not.
		virtual bool isSimulationLoaded() = 0;
		/// Returns true if a simulation is running; if a simulation is running, then it is implied that the simulation is loaded.
		virtual bool isSimulationRunning() = 0;
		/// Returns true if a simulation already finished; if this is true, then it is implied that the simulation is no longer running.
		virtual bool isSimulationDone() = 0;
		/// Stops the simulation
		virtual void stop() = 0;
		/// Returns true if the module name refers to a module that is currently loaded.
		virtual bool isModuleLoaded(const std::string & moduleName) = 0;
		/// Returns true if the agent is one of the currently selected agents.
		virtual bool isAgentSelected(SteerLib::AgentInterface * agent) = 0;
		/// Returns true if the command exists in the engine.
		virtual bool hasCommand(const std::string & commandName) = 0;
		//@}

		/// @name Module management
		//{@
		/// Loads a module and initializes it with the given options; or does nothing if the module is already loaded; and in either case, returns a reference to the module's meta information.
		virtual void loadModule(const std::string & moduleName, const std::string & searchPath, const std::string & options) = 0;
		/// Unloads a module, optionally unloading its dependencies if they are no longer in use by other modules.
		virtual void unloadModule(SteerLib::ModuleInterface * moduleToDestroy, bool recursivelyUnloadDependencies ) = 0;
		//}@

		/// @name Agent management
		//@{
		/// Asks a module to create an agent; returns a reference to the agent or NULL if the module does not support creating agents.
		virtual SteerLib::AgentInterface * createAgent(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::ModuleInterface * module) = 0;
		/// De-allocates an agent by asking the module (that created it) to destroy it.
		virtual void destroyAgent(SteerLib::AgentInterface * agentToDestroy) = 0;
		/// De-allocates all agents that are owned by the given module, by asking the module to destroy each one of them.
		virtual void destroyAllAgentsFromModule(SteerLib::ModuleInterface * owner) = 0;
		/// Adds an agent that is already allocated
		virtual void addAgent(SteerLib::AgentInterface * newAgent, SteerLib::ModuleInterface * owner) = 0;
		/// Removes an agent from the engine's data structures, without de-allocating it;  Whoever removed it is responsible for de-allocating it.
		virtual void removeAgent(SteerLib::AgentInterface * agentToRemove) = 0;
		/// Indicates that the given agent should be added to the set of "selected" agents.
		virtual void selectAgent(SteerLib::AgentInterface * agent) = 0;
		/// Indicates that the given agent should be removed from the set of selected agents; nothing will happen if the agent was not already selected.
		virtual void unselectAgent(SteerLib::AgentInterface * agent) = 0;
		/// Clears the list of selected agents.
		virtual void unselectAllAgents() = 0;
		/// Asks a module to create an agent; returns a reference to the agent or NULL if the module does not support creating agents.
		virtual void createAgentEmitter(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::ModuleInterface * module) = 0;
		//@}

		/// @name Obstacle management
		//@{
		/// Adds an obstacle that is already allocated; <b>Warning:</b> this does not update the spatial database.
		virtual void addObstacle(SteerLib::ObstacleInterface * newObstacle) = 0;
		/// Removes an obstacle without de-allocating it; whoever removed it is responsible for de-allocating it; <b>Warning:</b> this does not update the spatial database.
		virtual void removeObstacle(SteerLib::ObstacleInterface * obstacleToRemvoe) = 0;
		/// Removes all obstacles without de-allocating it; whoever removed it is responsible for de-allocating it; <b>Warning:</b> this does not update the spatial database.
		virtual void removeAllObstacles() = 0;
		//@}

		/// @name Command management
		//@{
		/// Adds a command to the engine so that other modules can invoke its functionality.
		virtual void addCommand(const std::string & commandName, SteerLib::CommandFunctionPtr commandFunction) = 0;
		/// Removes an existing command from the engine.
		virtual void removeCommand(const std::string & commandName) = 0;
		/// Invokes an existing command.
		virtual void runCommand(const std::string & commandName) = 0;
		//@}

		//True if CameraView is given in the testcase file
		virtual void isTestcaseCameraView(bool) = 0;

		/// @name Error management
		//@{
		/// Throws an exception on behalf of a module; this may be necessary if C++ exceptions are not properly thrown across library boundaries.
		virtual void throwException(const std::string & errorMessage) = 0;
		//@}
	};

} // end namespace SteerLib

#endif
