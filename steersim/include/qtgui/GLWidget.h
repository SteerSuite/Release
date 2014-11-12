//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __GL_WIDGET_H__
#define __GL_WIDGET_H__

/// @file GLWidget.h
/// @brief Declares the GLWidget class, which is the Qt GUI widget for the openGL simulation visualization.

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtOpenGL/QGLWidget>
#include "SteerLib.h"
#include "core/QtEngineDriver.h"
#include "qtgui/QtEngineController.h"

namespace SteerSimQt {

	class GLWidget : public QGLWidget
	{
		Q_OBJECT

	public:
		GLWidget(SteerLib::SimulationEngine * newEngine, const QGLFormat & format, bool dumpFrames);
		~GLWidget();
		void setControlKey(bool value);


	protected:

		/// @name Qt OpenGL functions
		//@{
		void initializeGL();
		void paintGL();
		void resizeGL(int width, int height);
		//@}

		/// @name Qt keyboard/mouse event handling functions
		//@{
		void keyPressEvent(QKeyEvent * event);
		void keyReleaseEvent(QKeyEvent * event);
		void mousePressEvent(QMouseEvent * event);
		void mouseReleaseEvent(QMouseEvent * event);
		void mouseMoveEvent(QMouseEvent * event);
		void wheelEvent(QWheelEvent * event);
		//@}

		void _checkGLCapabilities();
		void _saveOpenGLBufferToPng();
		void _findClosestAgentToMouse();


		SteerLib::SimulationEngine * _engine;
		SteerLib::EngineControllerInterface * _controller;

		int _mouseX;
		int _mouseY;

		bool _moveCameraOnMouseMotion;
		bool _rotateCameraOnMouseMotion;
		bool _zoomCameraOnMouseMotion;

		bool _multisampleAntialiasingSupported;
		bool _useAntialiasing;

		bool _dumpFrames;
		unsigned int _nextScreenshotNumber;
		GLvoid * _screenshotData;

		bool _canUseMouseToSelectAgents;
		SteerLib::AgentInterface* _agentNearestToMouse;

		bool _controlKeyPressed;

		int _windowWidth;
		int _windowHeight;

	};

} // namespace SteerSimQt;


#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif