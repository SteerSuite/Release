//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_MODULE_INTERFACE_H__
#define __STEERLIB_MODULE_INTERFACE_H__

/// @file ModuleInterface.h
/// @brief Declares the SteerLib::ModuleInterface virtual interface.

#include <map>

#include "Globals.h"
#include "interfaces/AgentInterface.h"
#include "LogData.h"

namespace SteerLib {


	/// An STL map that maps an option (string) to its vaule (string); used for passing options to modules.
	typedef std::map<std::string, std::string> OptionDictionary;

	/// An STL map that maps a module name (string) to the options of that module (OptionDictionary type).
	typedef std::map< std::string, OptionDictionary >  ModuleOptionsDatabase;

	// forward declaration
	class STEERLIB_API EngineInterface;


	/**
	 * @brief The primary interface for a module used by the SimulationEngine.
	 *
	 * %SteerLib provides several built-in modules, and can dynamically load additional plug-in modules. Modules can serve a variety of purposes,
	 * such as controlling initialization, providing steering AI or a component of AI, specialized animation, specialized rendering, and more.
	 *
	 *  If you want to create a plug-in module for the SimulationEngine:
	 *  -# Inherit and implement this interface.  All the functions below are optional except for initialization/cleanup functions.
	 *  -# Include SimulationPlugin.h and implement those two functions
	 *  -# Compile your code as a dynamic library, and place it in the appropriate modules directory where the SimulationEngine will search for modules.
	 *
	 * Additionally, the engine provides the SteerLib::EngineInterface to all modules, which can be used to access other
	 * modules, execute commands, control initialization, control the simulation, provide GUI controls, and more.
	 *
	 * The engine uses modules in the following way:
	 *  - At any time, even before calling init(), the engine may call getDependencies() and getConflicts().
	 *  - init() will be called once, before any other functions (except for getDependencies() and getConflicts()).
	 *  - When the module is being unloaded, finish() will be called before the (optional) destructor.
	 *  - During the lifetime of the module, the engine may run any number of simulations.  Each simulation has the following basic sequence:
	 *     -# For all modules, initializeSimulation() is called.  Modules that want to ask the engine to create agents should do so in this function.
	 *     -# For all modules, preprocessSimulation() is called.  Modules can assume that the initial conditions of all 
	 *        agents and obstacles are properly set up before this function is called.
	 *     -# For every update step:
	 *         -# For all modules, preprocessFrame() is called.
	 *         -# The engine then updates all agents and obstacles
	 *         -# For all modules, postprocessFrame() is called.
	 *     -# For all modules, postprocessSimulation() is called.  At this time, all agents and obstacles should remain allocated, so that modules can use them for post-processing.
	 *     -# For all modules, cleanupSimulation() is called.  Here, agents and obstacles should be deleted by the same module that created them.
	 *
	 * <h3>Notes</h3>
	 *  - The role of EngineInterface::createAgent() and ModuleInterface::createAgent() are slightly different:  Modules
	 *    that provide steering AI should implement ModuleInterface::createAgent(), allocating and returning the pointer of one agent,
	 *    and in turn, the engine would call ModuleInterface::createAgent() when appropriate.   On the other hand, a module that
	 *    wants to control the initialization of a simulation, for example, the TestCasePlayerModule or RecFilePlayerModule, would call
	 *    EngineInterface::createAgent().   EngineInterface::createAgent() then indirectly calls the appropriate module's
	 *    ModuleInterface::createAgent() function.
	 *
	 *  - No guarantees are made about the ordering of modules being loaded/initialized.
	 *
	 *
	 * @see
	 *   - SimulationPlugin.h
	 *   - SteerLib::EngineInterface provides a lot of functionality to modules.
	 *   - SteerLib::AgentInterface
	 *
	 */
	class STEERLIB_API ModuleInterface {
	public:
		virtual ~ModuleInterface() { }

		/// @name Initialization and Cleanup
		//@{
		/// @brief If the module is dependent on other modules, it must be listed here; each dependency is a module name, separated by whitespace.
		/// <b>Note:</b>This function must be valid at all times, even before init() is called or after finish() is called.
		virtual std::string getDependencies() = 0;
		/// @brief If the module conflicts with other modules, it must be listed here; each conflict is a module name, separated by whitespace.
		/// <b>Note:</b>This function must be valid at all times, even before init() is called or after finish() is called.
		virtual std::string getConflicts() = 0;
		/// gets data from module
		virtual std::string getData() = 0;
		/// get log data from module
		virtual LogData * getLogData() = 0;
		/// Initializes the module.  Note that other modules may not yet have been initialized when this is called.
		virtual void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) = 0;
		/// Cleans up the module.
		virtual void finish() = 0;
		//@}

		/// @name Creating/destroying agents and obstacles
		/// @brief These (optional) functions will be called by the engine to create/destroy AI agents.  Implement these if you created steering AI agents.  If you want to create/destroy agents from the module itself, use SteerLib::EngineInterface::createAgent() instead.
		//@{
		/// Allocates and returns a reference to an implementation of the AgentInterface class.
		virtual SteerLib::AgentInterface * createAgent() { return NULL; }
		/// De-allocates the AgentInterface;  note that anything allocated within a dynamic library should be de-allocated by the dynamic library as well.
		virtual void destroyAgent( SteerLib::AgentInterface * agent ) { }
		//@}

		/// @name Run-time functionality
		/// @brief These functions are invoked by the engine, for every module.  If your module has explcitly declared a dependency, the engine
		/// will guarantee to call the dependency's update functions before your module.
		//@{
		/// This function is called before preprocessSimulation() to allow a module to create/initialize agents, obstacles, and any other data structures that should be allocated before the simulation begins.
		virtual void initializeSimulation() { }
		/// This function is called after postprocessSimulation() to allow a module to cleanup/destroy agents and obstacles, and any other data structures that were allocated for the simulation.
		virtual void cleanupSimulation() { }
		/// This update function is called once before the simulation begins.
		virtual void preprocessSimulation() { }
		/// This update function is called once after the simulation ends; agents are still allocated when this is called, and ideally should be cleaned up by the modules that created them.
		virtual void postprocessSimulation() { }
		/// This update function is called once per frame before all agents are updated.
		virtual void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber) { }
		/// This update function is called once per frame after all agents are updated.
		virtual void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber) { }
		/// This function called when user interacts with the program using the keyboard, called if the engine did not already recognize the keypress.
		virtual void processKeyboardInput(int key, int action ) { }
		/// Uses OpenGL to draw any module-specific information to the screen; <b>WARNING:</b> this may be called multiple times per simulation step.
		virtual void draw() { }
		//@}
	};

} // end namespace SteerLib

#endif
