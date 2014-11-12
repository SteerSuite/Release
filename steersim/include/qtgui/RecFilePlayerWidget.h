//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __REC_FILE_PLAYER_WIDGET_H__
#define __REC_FILE_PLAYER_WIDGET_H__

/// @file RecFilePlayerWidget.h
/// @brief Declares the RecFilePlayerWidget class, which is the Qt GUI widget for the RecFilePlayerModule.

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QApplication>
#include "SteerLib.h"
#include "qtgui/GLWidget.h"
#include "qtgui/QtEngineController.h"

class RecFilePlayerModule;

namespace SteerSimQt {

	class RecFilePlayerWidget : public QWidget
	{
		Q_OBJECT

	public:
		RecFilePlayerWidget(RecFilePlayerModule * recFilePlayerModule, SteerLib::EngineInterface * engine);
		~RecFilePlayerWidget();

		void setFrameNumber(int frameNumber) { _frameNumberLineEdit->setText(QString("%1").arg(frameNumber)); }

	protected slots:
		void recFileNameChanged(const QString & text);
		void loadButtonPressed();
		void unloadButtonPressed();
		void playButtonPressed();
		void pauseButtonPressed();
		void unpauseButtonPressed();
		void stopButtonPressed();
		void backwardStepButtonPressed();
		void forwardStepButtonPressed();
		void recFileNameBrowsePressed();

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

		RecFilePlayerModule * _recFilePlayerModule;

		SteerLib::EngineInterface * _engine;
		SteerLib::EngineControllerInterface * _controller;
		QtEngineController * _qtController;

		QComboBox * _recFileNameComboBox;

		QLineEdit * _frameNumberLineEdit;

		QPushButton * _loadUnloadButton;
		QPushButton * _playPauseUnpauseButton;
		QPushButton * _stopButton;
		QPushButton * _backwardOneFrameButton;
		QPushButton * _forwardOneFrameButton;
		QPushButton * _recFileNameBrowseButton;

		QGroupBox * _recFileGroupBox;
		QGroupBox * _frameNumberGroupBox;

		QIcon _playIcon;
		QIcon _pauseIcon;

	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif
