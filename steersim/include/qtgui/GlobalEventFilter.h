//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __GLOBAL_EVENT_FILTER_H__
#define __GLOBAL_EVENT_FILTER_H__

/// @file GlobalEventFilter.h
/// @brief Declares the GlobalEventFilter class, which handles events that should affect the entire GUI.

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtGui/QMainWindow>

namespace SteerSimQt {

	// forward declarations
	class GLWidget;
	class ConsoleWidget;
	class QtEngineController;

	class GlobalEventFilter : public QObject
	{
		Q_OBJECT

	public:
		GlobalEventFilter(QtEngineController * controller, QMainWindow * mainWindow, GLWidget * gl);

	protected:
		bool eventFilter( QObject * obj, QEvent * event);

		QMainWindow * _mainWindow;
		QtEngineController * _controller;
		GLWidget * _glWidget;
	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif
