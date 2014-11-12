//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_ENGINE_CONTROLLER_INTERFACE_H__
#define __STEERLIB_ENGINE_CONTROLLER_INTERFACE_H__

/// @file EngineControllerInterface.h
/// @brief Declares the SteerLib::EngineControllerInterface virtual interface.

#include "Globals.h"

namespace SteerLib {

	/**
	 * @brief The virtual interface implemented by engine controllers to expose limited functionality to modules.
	 *
	 * Engine controllers are the back-end of a simulation, that provide an interface between the engine and
	 * the back-end.  This can be as simple as a command line, or as complex as a rich GUI or even a plugin for
	 * another system.
	 *
	 * This interface is used by engine controllers to provide modules with some control over how the simulation runs.
	 *
	 * <b>Warning:</b> Modules must first check what capabilities are supported by the engine controller.  Modules should not
	 * assume that the control functions will actually do anything if the capability is not supported;  some engine controllers
	 * may choose to ignore some or all of these functions.  For example, the CommandLineEngineDriver does not support any control,
	 * and the GLFWEngineDriver only allows pausing control.
	 *
	 * If you choose to implement your own EngineControllerInterface:
	 *   - If you return true for isStartupControlSupported(), you should make sure to properly implement loadSimulation(), unloadSimulation(),
	 *     startSimulation(), and stopSimulation().  Otherwise, those functions should be empty or cause an error.
	 *   - If you return true for isPausingcontrolSupported(), you should make sure to properly implement pauseSimulation(), unpauseSimulation(),
	 *     and pauseAndStepOneFrame().  Otherwise, those functions should be empty or cause an error.
	 *   - Most likely you should leave the Qt-related functions with their default implementations, which imply that Qt is not supported
	 *
	 * @see
	 *   - SteerLib::EngineInterface class, the functionality of the engine that is provided to modules.
	 */
	class STEERLIB_API EngineControllerInterface {
	public:
		virtual ~EngineControllerInterface() { }

		/// @name Querying the controller capabilities
		/// @brief These functions return true if a controller feature is implemented (i.e. supported), false if not.
		//@{
		/// Returns true if the controller allows modules to load, unload, start, and stop simulations.
		virtual bool isStartupControlSupported() = 0;
		/// Returns true if the controller allows modules to pause, unpause, and step simulations.
		virtual bool isPausingControlSupported() = 0;
		/// Returns true if the engine controller is the QtEngineDriver class.
		virtual bool isQtEngineDriver() { return false; }
		//@}

		/// @name Querying the controller state
		//@{
		/// Returns true if the controller is paused, false otherwise.
		virtual bool isPaused() = 0;
		//@}

		/// @name Simulation startup and cleanup controls
		/// @brief These controls should be implemented if isStartupControlSupported() returns true, otherwise the behavior depends on the implementation.
		//@{
		/// Tells the engine controller to initialize and setup initial conditions, but does not simulate anything yet.
		virtual void loadSimulation() = 0;
		/// Tells the engine controller to unload a simulation.
		virtual void unloadSimulation() = 0;
		/// Tells the engine controller to start running the simulation update loop.
		virtual void startSimulation() = 0;
		/// Tells the engine controller to permanently stop and cleanup a simulation; the only way to "restart" a simulation is to load a new simulation.
		virtual void stopSimulation() = 0;
		//@}

		/// @name Simulation pause and unpause controls
		/// @brief These controls should be implemented if isPausingControlSupported() returns true, otherwise the behavior depends on the implementation.
		//@{
		/// Tells the engine controller to temporarily pause the update loop.
		virtual void pauseSimulation() = 0;
		/// Tells the engine controller to unpause the update loop, if it is paused.
		virtual void unpauseSimulation() = 0;
		/// Tells the engine controller to flip the paused state; i.e. pause if it was previously unpaused, and unpause if it was previously paused.
		virtual void togglePausedState() = 0;
		/// Tells the engine controller to simulate one update step, but then pause the update loop.
		virtual void pauseAndStepOneFrame() = 0;
		//@}
		
		/// @name Qt-specific widget functions
		/// @brief If isQtEngineDriver() returns true, these accessors return pointers to Qt objects that can be used after static casting, otherwise the behavior depends on the implementation.
		//@{
		/// returns NULL or a pointer to the QtEngineController
		virtual void* getQtEngineController() { return NULL; }
		/// returns NULL or a pointer to the QMainWindow class (QMainWindow is a Qt class).
		virtual void* getQtMainWindow() { return NULL; }
		//@}
	};

} // end namespace SteerLib

#endif
