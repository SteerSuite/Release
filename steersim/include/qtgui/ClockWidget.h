//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __CLOCK_WIDGET_H__
#define __CLOCK_WIDGET_H__

/// @file ClockWidget.h
/// @brief Declares the ClockWidget class, which is the Qt GUI component for the simulation clock.


#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QApplication>

#include "SteerLib.h"
#include "qtgui/QtEngineController.h"
#include "qtgui/GLWidget.h"

namespace SteerSimQt {

	class ClockWidget : public QWidget
	{
		Q_OBJECT

	public:
		ClockWidget(SteerLib::EngineInterface * engine);
		void init();

	protected slots:
		void handleSimulationLoadedSignal();
		void handleSimulationUnloadedSignal();
		void handleSimulationStartedSignal();
		void handleSimulationStoppedSignal();
		void handleSimulationPausedSignal();
		void handleSimulationUnpausedSignal();
		void handleRealTimeUpdatedSignal();
		void handleSimulationAdvancedOneFrameSignal();

	protected:
		void _initWidgetLayout();

		SteerLib::EngineInterface * _engine;

		QLineEdit * _realTimeLineEdit;
		QLineEdit * _realFpsLineEdit;
		QLineEdit * _simulationTimeLineEdit;
		QLineEdit * _simulationFrameLineEdit;

		QLabel * _infoLabel;

	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif
