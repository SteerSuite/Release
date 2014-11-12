//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __TEST_CASE_PLAYER_WIDGET_H__
#define __TEST_CASE_PLAYER_WIDGET_H__

/// @file TestCasePlayerWidget.h
/// @brief Declares the TestCasePlayerWidget class, which is the Qt GUI widget for the TestCasePlayerModule.

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QApplication>
#include "SteerLib.h"
#include "qtgui/GLWidget.h"
#include "qtgui/QtEngineController.h"

class TestCasePlayerModule;

namespace SteerSimQt {

	class TestCasePlayerWidget : public QWidget
	{
		Q_OBJECT

	public:
		TestCasePlayerWidget(TestCasePlayerModule * testCasePlayerModule, SteerLib::EngineInterface * engine);
		~TestCasePlayerWidget();

		void setFrameNumber(int frameNumber) { _frameNumberLineEdit->setText(QString("%1").arg(frameNumber)); }

	protected slots:
		void testCaseNameChanged(const QString & text);
		void aiModuleNameChanged(const QString & text);
		void loadButtonPressed();
		void unloadButtonPressed();
		void playButtonPressed();
		void pauseButtonPressed();
		void unpauseButtonPressed();
		void stopButtonPressed();
		void forwardStepButtonPressed();
		void testCaseNameBrowsePressed();
		void moduleNameBrowsePressed();

		void handleSimulationLoadedSignal();
		void handleSimulationUnloadedSignal();
		void handleSimulationStartedSignal();
		void handleSimulationStoppedSignal();
		void handleSimulationPausedSignal();
		void handleSimulationUnpausedSignal();
		void handleSimulationAdvancedOneFrameSignal();

	protected:
		void _initWidgetLayout();
		void _setWidgetsToUnloadedState();
		void _setWidgetsToLoadedState();
		void _setWidgetsToDoneState();
		void _setWidgetsToPlayingState();
		void _setWidgetsToPausedState();

		TestCasePlayerModule * _testCasePlayerModule;

		SteerLib::EngineInterface * _engine;
		SteerLib::EngineControllerInterface * _controller;
		QtEngineController * _qtController;

		QComboBox * _testCaseNameComboBox;
		QComboBox * _moduleNameComboBox;

		QLineEdit * _frameNumberLineEdit;

		QPushButton * _loadUnloadButton;
		QPushButton * _playPauseUnpauseButton;
		QPushButton * _stopButton;
		QPushButton * _forwardOneFrameButton;
		QPushButton * _testCaseNameBrowseButton;
		QPushButton * _moduleNameBrowseButton;

		QGroupBox * _testCaseGroupBox;
		QGroupBox * _aiModuleGroupBox;
		QGroupBox * _frameNumberGroupBox;

		QIcon _playIcon;
		QIcon _pauseIcon;

	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif
