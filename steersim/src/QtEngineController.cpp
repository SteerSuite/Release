//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "SteerLib.h"
#include "qtgui/QtEngineController.h"
#include "qtgui/GLWidget.h"

using namespace SteerLib;
using namespace Util;
using namespace SteerSimQt;

void QtEngineController::init(SimulationEngine * engine, GLWidget * glWidget)
{
	_paused = false;
	_engine = engine;
	_glWidget = glWidget;
	_timer = new QTimer(this);
	connect( _timer, SIGNAL(timeout()), this, SLOT(updateGUIAndEngine()) );

	// According to Qt documentation, setInterval(0) means it will update after all events in the queue have been processed.
	// it seemed like that was actually causing unpleasant, irregular update rates, and it was much more regular to make it 1 msec.
	_timer->setInterval(1);
	_timer->start();
}

QtEngineController::~QtEngineController()
{
	delete _timer;
}


void QtEngineController::updateGUIAndEngine()
{
	bool stillRunning = true;

	if (_engine->getCurrentState() == SimulationEngine::ENGINE_STATE_SIMULATION_READY_FOR_UPDATE) {
		stillRunning = _engine->update(_paused);
		_glWidget->updateGL();  // make sure things get rendered one-to-one with engine updates, which is important for dumping frames to file.
		if (stillRunning == false) {
			_stopSimulation();
		}
		emit realTimeUpdatedSignal();
		if (!_paused) emit simulationAdvancedOneFrameSignal();
	}
	else {
		_glWidget->updateGL();
	}

}


void QtEngineController::_loadSimulation()
{
	_engine->initializeSimulation();
	std::cout << "Simulation loaded.\n";
	emit simulationLoadedSignal();
}

void QtEngineController::_startSimulation()
{
	std::cout << "Simulation started.\n";
	emit simulationStartedSignal();
	_engine->preprocessSimulation();
}

void QtEngineController::_stopSimulation()
{
	_engine->postprocessSimulation();
	std::cout << "Simulation stopped.\n";
	emit simulationStoppedSignal();
}

void QtEngineController::_pauseSimulation()
{
	// eventually delete this line after new clock/pausing mechanism is stablized
	//_engine->getClock().setPausedState(true);
	_paused = true;
	std::cout << "Simulation paused.\n";
	emit simulationPausedSignal();
}

void QtEngineController::_unpauseSimulation()
{
	// eventually delete this line after new clock/pausing mechanism is stablized
	//_engine->getClock().setPausedState(false);
	_paused = false;
	std::cout << "Simulation unpaused.\n";
	emit simulationUnpausedSignal();
}

void QtEngineController::_togglePausedState()
{
	if (_paused) {
		_unpauseSimulation();
	}
	else {
		_pauseSimulation();
	}
}

void QtEngineController::_pauseAndStepOneFrame()
{
	std::cout << "Simulation stepping forward one frame.\n";
	bool wasAlreadyPaused = _paused;
	_engine->update(false);
	if (!wasAlreadyPaused) emit simulationPausedSignal();
	emit realTimeUpdatedSignal();
	emit simulationAdvancedOneFrameSignal();
}

void QtEngineController::_unloadSimulation()
{
	_engine->cleanupSimulation();
	std::cout << "Simulation unloaded.\n";
	emit simulationUnloadedSignal();
}


#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
