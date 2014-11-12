//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include <math.h>
#include "SteerLib.h"

#include "core/SteerSimOptions.h"
#include "glfw/include/GL/glfw.h"

#include "qtgui/GLWidget.h"
#include "qtgui/QtEngineController.h"

using namespace std;
using namespace SteerLib;
using namespace Util;
using namespace SteerSimQt;

extern SteerSimOptions gSteerSimConfig;

#define MULTISAMPLE_ARB 0x809D


GLWidget::GLWidget(SimulationEngine * newEngine, const QGLFormat & format, bool dumpFrames) : QGLWidget(format)
{
	_engine = newEngine;
	_controller = _engine->getEngineController();
	_mouseX = 0;
	_mouseY = 0;
	_moveCameraOnMouseMotion = false;
	_rotateCameraOnMouseMotion = false;
	_zoomCameraOnMouseMotion = false;
	_multisampleAntialiasingSupported = false; // until we find out later
	_useAntialiasing = gSteerSimConfig.defaultUseAntialiasing();
	_agentNearestToMouse = NULL;
	_canUseMouseToSelectAgents = gSteerSimConfig.defaultCanUseMouseSelection();
	_nextScreenshotNumber = 0;
	_dumpFrames = dumpFrames;
	_screenshotData = NULL;

	_controlKeyPressed = false;

	setMouseTracking ( true );
	setFocusPolicy ( Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    makeCurrent();
}


void GLWidget::setControlKey(bool value)
{
	_controlKeyPressed = value;
}


void GLWidget::initializeGL()
{
	_checkGLCapabilities();

	if (_multisampleAntialiasingSupported && _useAntialiasing) {
		glEnable( MULTISAMPLE_ARB );
	} else {
		glDisable( MULTISAMPLE_ARB );
	}

	_engine->initGL();

	// GLWidget::resizeGL is called on initialization, but I forget how??
	// but anyway that means its not needed here.
	//_engine->resizeGL(_windowWidth, _windowHeight); 

	DrawLib::init();
}

void GLWidget::paintGL()
{
	// get the camera from the engine
	Camera & cam = _engine->getCamera();

	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam.apply();
	DrawLib::positionLights();
	_engine->draw();


	// check for errors
	int error = glGetError();
	if (error != GL_NO_ERROR) {
		cerr << "OpenGL error occurred: " << gluErrorString(error) << "\n";
		throw GenericException("OpenGL error occurred in SteerSimGUI::_drawScene().");
	}

	// TODO: is this in the right place, before swapping buffers?!?  maybe its different for Qt because of timing?
	if (_dumpFrames && _engine->isSimulationRunning()) _saveOpenGLBufferToPng();

	// double buffering, swap back and front buffers
	glFlush();
	swapBuffers();
}

void GLWidget::resizeGL(int width, int height)
{
    _engine->resizeGL(width, height);

	if (_screenshotData != NULL) {
		delete _screenshotData;
		// Warning: assuming an int is 32 bits...
		_screenshotData = new int[width*height];
	}
}

void GLWidget::_findClosestAgentToMouse()
{
	// gotta find mouse/screen position in world coords

	// Since screen is 2d and world coords is 3d, we convert screen
	// position into a ray originating from the camera position and
	// going in the direction indicated by the mouse pos

	// next bit is apparently standard openGl procedure to getting what
	// we want:

	double modelView[16];
	double projection[16];
	int viewport[4];

	double x1, y1, z1, x2, y2, z2;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	int x = _mouseX;
	int y = viewport[3] - _mouseY;
	gluUnProject(x, y, 0, modelView, projection, viewport, &x1, &y1, &z1);
	gluUnProject(x, y, 1, modelView, projection, viewport, &x2, &y2, &z2);

	// Our ray: originates at the camara and goes somewhere
	Vector cameraRay = Vector((float)(x2-x1), (float)(y2-y1), (float)(z2-z1));
	Point cameraPos = _engine->getCamera().position();

	// to determine which agents are closest to the mouse click, figure out which
	// agents are closest to the ray
	// NOTE: assuming that agents are all at the y=0 plane.
	float t = -cameraPos.y / cameraRay.y;  // this gives us the t value for where the point along the ray would be on the y=0 plane.
	Point locationOfMouseOnYPlane = cameraPos + t*cameraRay;

	// nearest: nearest agent to mouse pos
	// nearestAndContained: nearest agent to mouse pos and the point mouse is within radius

	AgentInterface* nearest = NULL;

	float distanceToNearest = FLT_MAX;

	const std::vector<AgentInterface*> & agents = _engine->getAgents();
	for(std::vector<AgentInterface*>::const_iterator i = agents.begin(); i != agents.end(); i++)
	{
		float dist = distanceBetween((*i)->position(), locationOfMouseOnYPlane);
		// if its the closest one, but also within some distance threshold
		if ((dist < (*i)->radius()+0.5f) && (dist < distanceToNearest)) {
			nearest = (*i);
			distanceToNearest = dist;
		}
	}
	_agentNearestToMouse = nearest;
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	// NOTE: The GlobalEventFilter is a friend class that may intercept events and affect the state of this widget without ever
	//       entering this event handler.

	if (event->key() == Qt::Key_A) {
		if (!_multisampleAntialiasingSupported) {
			cerr << "WARNING: toggling antialiasing may have no effect; antialiasing does not seem to be supported." << std::endl;
		}
		_useAntialiasing = !_useAntialiasing;
		if (_useAntialiasing) {
			glEnable( MULTISAMPLE_ARB );
		} else {
			glDisable( MULTISAMPLE_ARB );
		}
		std::cout << "Antialiasing turned " << (_useAntialiasing ? "on" : "off") << std::endl;
	}
	else if (event->key() == Qt::Key_Control) {
		_controlKeyPressed = true;
	}
	else if (event->key() == Qt::Key_P) {
		cout << "CAMERA INFO:" << endl;
		cout << "  Position:     " << _engine->getCamera().position() << endl;
		cout << "  LookAt:       " << _engine->getCamera().lookat() << endl;
		cout << "  Up:           " << _engine->getCamera().up() << endl;
	} 
	else if (event->key() == Qt::Key_F12) {
		_saveOpenGLBufferToPng();
	}
	else if (event->key() == Qt::Key_Home) {
		_dumpFrames = true;
	}
	else if (event->key() == Qt::Key_End) {
		_dumpFrames = false;
	}
	else if (event->key() == Qt::Key_Right) {
		_controller->pauseAndStepOneFrame();
	}
	else if (event->key() == Qt::Key_Space) {
		if(_engine->isSimulationRunning()) {
			/// @todo create a "togglePausedState" controller function
			if(_controller->isPaused())
				_controller->unpauseSimulation();
			else
				_controller->pauseSimulation();
		}
		else {
			if (_engine->isSimulationLoaded()) {
				if (!_engine->isSimulationDone()) {
					_controller->startSimulation();
				}
			}
			else {
				// eventually, this is probably not a useful message, but just putting it here temporarily for testing.
				std::cout << "No simulation loaded yet.\n";
			}
		}
	}
	else if (event->key() == Qt::Key_F) {
		cout << "Frame Number " << _engine->getClock().getCurrentFrameNumber() << "\n";
	}
	else {
		int glfwButtonCode;
		int glfwActionCode = GLFW_PRESS;

		switch(event->key()) {
			case Qt::Key_Left:
				glfwButtonCode = gSteerSimConfig.stepBackwardKey();
				break;
			case Qt::Key_Minus:
				glfwButtonCode = '-';
				break;
			case Qt::Key_Equal:
				glfwButtonCode = '=';
				break;
			case Qt::Key_Backspace:
				glfwButtonCode = gSteerSimConfig.resetPlaybackSpeedKey();
				break;
			case Qt::Key_R:
				glfwButtonCode = 'R';
				break;
			default:
				return;
		}
		if (_engine) _engine->processKeyboardInput( glfwButtonCode , glfwActionCode );
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Control) {
		_controlKeyPressed = false;
	}
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() ==  Qt::LeftButton) {
		if(!_controlKeyPressed) {
			_findClosestAgentToMouse();
			if (_agentNearestToMouse != NULL) {
				if (!_engine->isAgentSelected(_agentNearestToMouse)) {
					_engine->selectAgent(_agentNearestToMouse);
					unsigned int i;
					for (i=0; i< _engine->getAgents().size(); i++) {
						if ( _engine->getAgents()[i] == _agentNearestToMouse ) {
							break;
						}
					}
					if (_agentNearestToMouse != NULL) cerr << "selected agent #" << i << " (total " << _engine->getSelectedAgents().size() << " agents are currently selected)." << endl;
				}
				else {
					_engine->unselectAgent(_agentNearestToMouse);
					unsigned int i;
					for (i=0; i< _engine->getAgents().size(); i++) {
						if ( _engine->getAgents()[i] == _agentNearestToMouse ) {
							break;
						}
					}
					if (_agentNearestToMouse != NULL) cerr << "un-selected agent #" << i << " (total " << _engine->getSelectedAgents().size() << " agents are currently selected)." << endl;
				}
			}
			else {
				_engine->unselectAllAgents();
			}

			//if (_agentNearestToMouse != NULL) {
			//	if (_engine->isAgentSelected(_agentNearestToMouse)) {
			//		_engine->unselectAgent(_agentNearestToMouse);
			//	}
			//	else {
			//		_engine->selectAgent(_agentNearestToMouse);
			//	}
			//}
			//else {
			//	_engine->unselectAllAgents();
			//}
		}
	}

	if (event->button() == Qt::MidButton) {
		if(_controlKeyPressed) _moveCameraOnMouseMotion = true;
	}

	if (event->button() == Qt::LeftButton) {
		if(_controlKeyPressed) _rotateCameraOnMouseMotion = true;
	}

	if (event->button() == Qt::RightButton) {
		if(_controlKeyPressed) _zoomCameraOnMouseMotion = true;
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MidButton /*gSteerSimConfig.moveCameraClick()*/) {
		_moveCameraOnMouseMotion = false;
	}

	if (event->button() == Qt::LeftButton /*gSteerSimConfig.rotateCameraClick()*/) {
		_rotateCameraOnMouseMotion = false;
	}

	if (event->button() == Qt::RightButton /*gSteerSimConfig.zoomCameraClick()*/) {
		_zoomCameraOnMouseMotion = false;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	// get mouse changes
	int deltaX = event->x() - _mouseX;
	int deltaY = event->y() - _mouseY;

	// update mouse position
	_mouseX = event->x();
	_mouseY = event->y();

	// camera rotate
	if(_rotateCameraOnMouseMotion)
	{
		float xAdjust = -deltaX * gSteerSimConfig.defaultMouseRotationFactor();
		float yAdjust = deltaY * gSteerSimConfig.defaultMouseRotationFactor();

		_engine->getCamera().nudgeRotate(yAdjust, xAdjust);
	}

	// camera zoom
	if(_zoomCameraOnMouseMotion)
	{
		float yAdjust = deltaY * gSteerSimConfig.defaultMouseZoomFactor();
		_engine->getCamera().nudgeZoom(yAdjust);
	}

	// camera move
	if(_moveCameraOnMouseMotion)
	{
		float xAdjust = deltaX * gSteerSimConfig.defaultMouseMovementFactor();
		float yAdjust = deltaY * gSteerSimConfig.defaultMouseMovementFactor();

		_engine->getCamera().nudgePosition(xAdjust, yAdjust);
	}
}

void GLWidget::wheelEvent(QWheelEvent * event)
{
	int deltaWheel = event->delta()/120;
	if (deltaWheel != 0) {
		float wheelAdjust = -20.0f * deltaWheel * gSteerSimConfig.defaultMouseZoomFactor();
		_engine->getCamera().nudgeZoom(wheelAdjust);
	}
}


void GLWidget::_checkGLCapabilities()
{

	std::string extensionsString = std::string((const char*)glGetString( GL_EXTENSIONS ));
	std::istringstream extensionsStream(extensionsString);

	// assume extensions are not supported until we find that they are supported
	_multisampleAntialiasingSupported = false;

	std::string extension;
	while (extensionsStream >> extension) {

		if (extension == "GL_ARB_multisample") {
			_multisampleAntialiasingSupported = true;
		}

	}

}

void GLWidget::_saveOpenGLBufferToPng()
{
	glReadBuffer(GL_BACK);

	if (_screenshotData == NULL) {
		_screenshotData = new int[width()*height()];
	}

	glReadPixels(0, 0, width(), height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, _screenshotData);

	QImage screenshot((uchar*)_screenshotData, width(), height(), QImage::Format_RGB32);

	QString filename = "image" + 
		((_nextScreenshotNumber < 10 ? "0000" : (_nextScreenshotNumber < 100 ? "000" : (_nextScreenshotNumber < 1000 ? "00" : (_nextScreenshotNumber < 10000 ? "0" : ""))))
		+ QString::number(_nextScreenshotNumber++)) + ".png";
	screenshot.mirrored(false,true).save(filename, "png");
	std::cerr << "Screenshot written to " + filename.toStdString() + "\n";
}

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
