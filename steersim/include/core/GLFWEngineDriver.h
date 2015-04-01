//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __GLFW_ENGINE_DRIVER_H__
#define __GLFW_ENGINE_DRIVER_H__

/// @file GLFWEngineDriver.h
/// @brief Declares the GLFWEngineDriver class


//
// GLFWEngineDriver object:  receives an exisiting SteerLib::SimulationEngine, and runs it properly.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_GLFW

#include "SteerLib.h"
#include "glfw/include/GL/glfw.h"
#include "util/FrameSaver.h"

/**
 * @brief A GUI back-end that controls a SteerLib::SimulationEngine.
 *
 * This back-end is used to visualize the contents of a simulation interactively.
 *
 * This class uses GLFW, which is more recent than the traditional GLUT.  Like GLUT, 
 * GLFW is pure C, so it can only invoke callbacks trhough static non-member wrappers.
 * Because of this, this class uses a singleton design pattern.
 *
 * @see
 *   - CommandLineEngineDriver to control a SimulationEngine without a GUI.
 */
class GLFWEngineDriver : public SteerLib::EngineControllerInterface
{
public:
	static GLFWEngineDriver * getInstance();
	void init(SteerLib::SimulationOptions * options);
	void finish();
	void run();
	const char * getData();
	LogData * getLogData();

	SteerLib::SimulationEngine * getEngine() { return _engine; };
	void writeScreenCapture();
	void dumpTestCase();

	void processWindowResizedEvent(int width, int height);
	void processKeyPressEvent(int key, int action);
	void processMouseMovementEvent(int x, int y);
	void processMouseButtonEvent(int button, int action);
	void processMouseWheelEvent(int pos);
	int processWindowCloseEvent();

	/// @name The EngineControllerInterface
	/// @brief The GLFWEngineDriver supports pause-related controls, does not support startup controls.
	//@{
	virtual bool isStartupControlSupported() { return false; }
	virtual bool isPausingControlSupported() { return true; }
	virtual bool isPaused() { return _paused; }
	virtual void loadSimulation() { throw Util::GenericException("GLFWEngineDriver does not support loadSimulation()."); }
	virtual void unloadSimulation() { throw Util::GenericException("GLFWEngineDriver does not support unloadSimulation()."); }
	virtual void startSimulation() { throw Util::GenericException("GLFWEngineDriver does not support startSimulation()."); }
	virtual void stopSimulation() { throw Util::GenericException("GLFWEngineDriver does not support stopSimulation()."); }
	virtual void pauseSimulation() { _paused = true; }
	virtual void unpauseSimulation() { _paused = false; }
	virtual void togglePausedState() { _paused = !_paused; }
	virtual void pauseAndStepOneFrame() { _paused = true;  _engine->update(false); }
	//@}

protected:
	/// Use static function GLFWEngineDriver::getInstance() instead.
	GLFWEngineDriver();
	~GLFWEngineDriver() {}

	void _initGLFW();
	void _checkGLCapabilities();
	void _initGL();
	void _findClosestAgentToMouse();
	void _drawScene();
	void _drawGUI();

	bool _alreadyInitialized;
	bool _paused;
	SteerLib::SimulationEngine * _engine;
	int _mouseX;
	int _mouseY;
	int _wheelPos;

	bool _moveCameraOnMouseMotion;
	bool _rotateCameraOnMouseMotion;
	bool _zoomCameraOnMouseMotion;

	bool _multisampleAntialiasingSupported;
	bool _useAntialiasing;
	bool _canUseMouseToSelectAgents;
	bool _canUseMouseWheelZoom;
	SteerLib::AgentInterface* _agentNearestToMouse;
	unsigned int _nextScreenshotNumber;
	bool _dumpFrames;
	bool _done;

	SteerLib::SimulationOptions * _options;

	Util::FrameSaver * _frameSaver;


private:
	/// @brief These functions are private, and un-implemented, to protect against mangling the instance.
	//@{
	GLFWEngineDriver(const GLFWEngineDriver & );  // not implemented, not copyable
	GLFWEngineDriver& operator= (const GLFWEngineDriver & );  // not implemented, not assignable
	//@}
};


#endif // ifdef ENABLE_GLFW
#endif // ifdef ENABLE_GUI

#endif
