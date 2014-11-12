//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __QT_ENGINE_CONTROLLER_H__
#define __QT_ENGINE_CONTROLLER_H__

/// @file QtEngineController.h
/// @brief Declares the QtEngineController class, which is main Qt class that runs simulations.

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtCore/QStringList>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QApplication>

#include "SteerLib.h"

namespace SteerSimQt {

	// forward declaration
	class GLWidget;

	class QtEngineController : public QObject
	{
		Q_OBJECT

		friend class QtEngineDriver;
		friend class GlobalEventFilter;

	public:
		void init(SteerLib::SimulationEngine * engine, GLWidget * glWidget);
		~QtEngineController();

	signals:
		/// @name custom Qt signals
		/// @brief These Qt signals indicate what's happening to the simulation, so that GUI elements of other modules can update themselves.
		//@{
		/// Indicates that a simulation is loaded and initialized (but not preprocessed), i.e. SteerLib::ModuleInterface::initializeSimulation() for all modules has been called.
		void simulationLoadedSignal();
		/// Indicates that the simulation will start immediately next, i.e. just before SteerLib::ModuleInterface::preprocessSimulation() is called for all modules.
		void simulationStartedSignal();
		/// Indicates that the simulation transitioned from unpaused to paused.
		void simulationPausedSignal();
		/// Indicates that the simulation transitioned from paused to unpaused.
		void simulationUnpausedSignal();
		/// Indicates that the engine real-time was updated; this signal is emitted along with simulationAdvancedOneFrameSignal() when the simulation advances, but also happens when the simulation is paused.
		void realTimeUpdatedSignal();
		/// Indiates that the simulation was stepped forward one frame.
		void simulationAdvancedOneFrameSignal();
		/// Indicates that the simulation is done and post-processed, but not de-allocated; i.e. after SteerLib::ModuleInterface::postprocessSimulation() was called for all modules, but before SteerLib::ModuleInterface::cleanupSimulation() is called for all modules.
		void simulationStoppedSignal();
		/// Indicates that a simulation is officially unloaded, i.e. SteerLib::ModuleInterface::cleanupSimulation() for all modules has been called, and the engine and modules are ready for a new simulation to be loaded.
		void simulationUnloadedSignal();
		//@}

	protected slots:
		/// @name custom Qt slots
		//@{
		/// This event handler is invoked every time the GUI should be updated.
		void updateGUIAndEngine();
		//@}


	protected:
		/// @name engine control functions
		/// @brief These functions are only allowed to be used by specific friend classes; modules should use the SteerLib::EngineControllerInterface instead.
		//@{
		void _loadSimulation();
		void _startSimulation();
		void _stopSimulation();
		void _pauseSimulation();
		void _unpauseSimulation();
		void _togglePausedState();
		void _pauseAndStepOneFrame();
		void _unloadSimulation();
		//@}


		SteerLib::SimulationEngine * _engine;
		GLWidget * _glWidget;
		QTimer * _timer;

		bool _paused;
	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif