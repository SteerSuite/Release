//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file QtEngineDriver.cpp
/// @brief Implements the QtEngineDriver class.
///

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <iostream>

#include "SteerLib.h"
#include "core/QtEngineDriver.h"

using namespace std;
using namespace SteerLib;
using namespace Util;
using namespace SteerSimQt;

QtEngineDriver * QtEngineDriver::getInstance()
{
	static QtEngineDriver * qtsingletonInstance = new QtEngineDriver();
	return qtsingletonInstance;
}

//
// constructor
//
QtEngineDriver::QtEngineDriver()
{
	_alreadyInitialized = false;
	_dumpFrames = false;
	_engine = new SimulationEngine();
}

//
// destructor
//
QtEngineDriver::~QtEngineDriver()
{
	delete _mainWindow;
	delete _app;
	delete _engine;
}

//
// init()
//
void QtEngineDriver::init(int argc, char ** argv)
{
	if (_alreadyInitialized) {
		throw GenericException("QtEngineDriver::init() was called after being initialized. It should only be called once.");
	}
	_alreadyInitialized = true;

	CommandLineParser opts;
	std::vector< char * > leftOverArgs;
	opts.addOption( "-dumpframes", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &_dumpFrames, true);
	opts.addOption( "-dumpFrames", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &_dumpFrames, true);
	opts.parse(argc, argv, false, leftOverArgs);
	argc = leftOverArgs.size();
	argv = (leftOverArgs.size() == 0) ? NULL : &(leftOverArgs[0]);

	_engine = new SimulationEngine();
	_app = new QApplication(argc, argv);
	_mainWindow = new QMainWindow();
	_controller = new QtEngineController();

	// unclean, but this one should be initialized before others so that it can re-direct cout,cerr,clog as soon as possible.
	_consoleWidget = ConsoleWidget::getInstance();


	// initialize the engine
	_engine->init(argc, argv, this);

	// initialize the GUI
	_initGUI();

	// initialize the controller
	_controller->init(_engine, _glWidget);

}

void QtEngineDriver::finish()
{
}

void QtEngineDriver::_initGUI()
{
	QGLFormat fmt;
	fmt.setSampleBuffers(true);
	fmt.setSwapInterval(0); // vsync off; for now... eventually make this an option.

	_glWidget = new GLWidget(_engine,fmt,_dumpFrames);
	_moduleLoaderWidget = new ModuleLoaderWidget(_engine);
	_clockWidget = new ClockWidget(_engine);
	_eventFilter = new GlobalEventFilter(_controller, _mainWindow, _glWidget);

	_moduleLoaderWidget->init();
	_clockWidget->init();
	//_glWidget->init();
	//_consoleWidget->init();

	_app->installEventFilter(_eventFilter);

	QDockWidget * consoleDockWidget = new QDockWidget("Console");
	consoleDockWidget->setWidget(_consoleWidget);

	QDockWidget * moduleLoaderDockWidget = new QDockWidget("Module Loader");
	moduleLoaderDockWidget->setWidget(_moduleLoaderWidget);

	QDockWidget * clockDockWidget = new QDockWidget("Clock");
	clockDockWidget->setWidget(_clockWidget);

	_mainWindow->setCentralWidget(_glWidget);
	_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
	_mainWindow->addDockWidget(Qt::RightDockWidgetArea, moduleLoaderDockWidget);
	_mainWindow->addDockWidget(Qt::RightDockWidgetArea, clockDockWidget);

	_mainWindow->setWindowTitle(QMainWindow::tr("SteerSim"));
	_mainWindow->resize(1000, 750);
	_mainWindow->setUnifiedTitleAndToolBarOnMac(true);
}

void QtEngineDriver::run()
{
	_mainWindow->show();
	_app->exec();
}

void QtEngineDriver::loadSimulation()
{
	_controller->_loadSimulation();
}

void QtEngineDriver::unloadSimulation()
{
	_controller->_unloadSimulation();
}

void QtEngineDriver::startSimulation()
{
	_controller->_startSimulation();
}

void QtEngineDriver::pauseSimulation()
{
	_controller->_pauseSimulation();
}

void QtEngineDriver::unpauseSimulation()
{
	_controller->_unpauseSimulation();
}

void QtEngineDriver::togglePausedState()
{
	_controller->_togglePausedState();
}

void QtEngineDriver::stopSimulation()
{
	_controller->_stopSimulation();
}

void QtEngineDriver::pauseAndStepOneFrame()
{
	_controller->_pauseAndStepOneFrame();
}

/*
void QtEngineDriver::_createMenus()
{
	// this popup menu belongs to either the glWidget, global event filter, or somewhere else....
	// the challenge will be how to provide context-sensitive functionality into the menu
	_popupMenu = new QMenu("Menu");
	_popupMenu->addAction( "Add Agent here" );
	_popupMenu->addAction( "Add Obstacle here" );
	_popupMenu->addAction( "Remove this agent" );
	_popupMenu->addAction( "Select this agent only" );
	_popupMenu->addAction( "Add this agent to list of selected agents" );
	_popupMenu->addAction( "Add this agent to list of selected agents" );
	_popupMenu->addSeparator();
	_popupMenu->addAction( "Load the testCasePlayer module" );
	_popupMenu->addAction( "Load the recFilePlayer module" );
	_popupMenu->addAction( "Module manager..." );
	_popupMenu->addSeparator();
	_popupMenu->addAction( "Configure options..." );
	_popupMenu->addAction( "Exit" );


	//_mainWindow->add
	//_fileMenu = _mainWindow->menuBar()->addMenu("&File");
	//_fileMenu->addSeparator();
	//_editMenu = _mainWindow->menuBar()->addMenu(QMenu::tr("&Edit"));
	//_helpMenu = _mainWindow->menuBar()->addMenu(QMenu::tr("&Help"));
}

void QtEngineDriver::_createActions()
{
//	_runAct = new QAction(("Test case player..."), _mainWindow);
//	connect(_runAct, SIGNAL(triggered()), this, SLOT(runSimulation()));
}

void QtEngineDriver::_createToolBars()
{
	//_mainWindow->addToolBar(_toolBar);
	//_toolBar->addAction(new QAction(QIcon("testicon.png"),("Open the test case player"), _mainWindow));
}
*/

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
