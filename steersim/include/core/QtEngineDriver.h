//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __QT_ENGINE_DRIVER_H__
#define __QT_ENGINE_DRIVER_H__

/// @file QtEngineDriver.h
/// @brief Declares the QtEngineDriver class

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "SteerLib.h"
#include "qtgui/QtEngineController.h"
#include "qtgui/GLWidget.h"
#include "qtgui/ConsoleWidget.h"
#include "qtgui/ModuleManagerWidget.h"
#include "qtgui/GlobalEventFilter.h"
#include "qtgui/ClockWidget.h"

#include <QtCore/QObject>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QtGui>

namespace SteerSimQt {

	class QtEngineController;
	class ClockWidget;

	class QtEngineDriver: public SteerLib::EngineControllerInterface
	{

	public:
		static QtEngineDriver * getInstance();
		void init(int argc, char ** argv);
		void finish();
		void run();

		/// @name The EngineControllerInterface
		/// @brief The QtEngineDriver supports pause-related controls, startup-related controls, and provides access to the QtEngineController class so that modules can connect signals/slots.
		//@{
		virtual bool isStartupControlSupported() { return true; }
		virtual bool isPausingControlSupported() { return true; }
		virtual bool isQtEngineDriver() { return true; }
		virtual bool isPaused() { return _controller->_paused; }
		virtual void loadSimulation();
		virtual void unloadSimulation();
		virtual void startSimulation();
		virtual void stopSimulation();
		virtual void pauseSimulation();
		virtual void unpauseSimulation();
		virtual void togglePausedState();
		virtual void pauseAndStepOneFrame();
		virtual void* getQtEngineController() { return _controller; }
		virtual void* getQtMainWindow() { return _mainWindow; }
		//@}

	protected:
		/// Use static function QtEngineDriver::getInstance() instead.
		QtEngineDriver(); 
		~QtEngineDriver();

		/// @name helper functions
		//@{
		void _initGUI();
		void _createActions();
		void _createMenus();
		void _createToolBars();
		//@}


		bool _alreadyInitialized;
		bool _dumpFrames;

		SteerLib::SimulationEngine * _engine;
		QApplication * _app;
		QtEngineController * _controller;

		/// @name Standard GUI components
		/// @brief These GUI classes are used by any instance of the Qt GUI regardless of what modules the engine has loaded. Modules may create additional GUI components that are not listed here.
		//@{
		QMainWindow * _mainWindow;
		GLWidget * _glWidget;
		ConsoleWidget * _consoleWidget;
		ModuleLoaderWidget * _moduleLoaderWidget;
		ClockWidget * _clockWidget;
		GlobalEventFilter * _eventFilter;
		//@}

	private:
		/// @brief These functions are private, and un-implemented, to protect against mangling the instance.
		//@{
		QtEngineDriver(const QtEngineDriver & );  // not implemented, not copyable
		QtEngineDriver& operator= (const QtEngineDriver & );  // not implemented, not assignable
		//@}

	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif
