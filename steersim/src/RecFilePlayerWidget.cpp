//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file RecFilePlayerWidget.cpp
/// @brief Implements the RecFilePlayerWidget class

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "SteerLib.h"
#include "core/SteerSimOptions.h"
#include "qtgui/Tooltip.h"
#include "qtgui/RecFilePlayerWidget.h"
#include "modules/RecFilePlayerModule.h"

using namespace Util;
using namespace SteerLib;
using namespace SteerSimQt;


RecFilePlayerWidget::RecFilePlayerWidget(RecFilePlayerModule * recFilePlayerModule, SteerLib::EngineInterface * engine)
{
	_recFilePlayerModule = recFilePlayerModule;
	_engine = engine;
	_controller = _engine->getEngineController();
	_qtController = (QtEngineController*)(_controller->getQtEngineController());
	
	_recFileNameComboBox = new QComboBox();
	_recFileNameComboBox->addItem(QString::fromStdString(_recFilePlayerModule->_recFilename));
	_recFileNameComboBox->setEditable(true);
	_recFileNameComboBox->setToolTip(TOOL_TIP_REC_FILE_NAME);
	_recFileNameComboBox->setFixedWidth(155);

	_recFileNameBrowseButton = new QPushButton("...");
	_recFileNameBrowseButton->setMaximumWidth(30);

	_frameNumberLineEdit = new QLineEdit(QString("%1").arg( _engine->getClock().getCurrentFrameNumber() ));
	_frameNumberLineEdit->setMaxLength(10);
	_frameNumberLineEdit->setMaximumWidth(70);
	_frameNumberLineEdit->setAlignment(Qt::AlignRight);
	_frameNumberLineEdit->setReadOnly(true);
	_frameNumberLineEdit->setToolTip(TOOL_TIP_FRAME_NUMBER);

	_loadUnloadButton = new QPushButton("Load recording");
	//_loadUnloadButton->setToolTop(...)

	_playPauseUnpauseButton = new QPushButton();
	_playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	_pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	_playPauseUnpauseButton->setIcon(_playIcon);

	_stopButton = new QPushButton();
	_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	_stopButton->setToolTip(TOOL_TIP_STOP_BUTTON);

	_backwardOneFrameButton = new QPushButton();
	_backwardOneFrameButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	_backwardOneFrameButton->setToolTip(TOOL_TIP_STEP_BACKWARD_BUTTON);

	_forwardOneFrameButton = new QPushButton();
	_forwardOneFrameButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	_forwardOneFrameButton->setToolTip(TOOL_TIP_STEP_FORWARD_BUTTON);

	connect( _recFileNameComboBox, SIGNAL(editTextChanged(const QString)), this, SLOT(recFileNameChanged(const QString)) );
	connect( _recFileNameBrowseButton, SIGNAL(pressed()), this, SLOT(recFileNameBrowsePressed()) );
	connect( _stopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()) );
	connect( _backwardOneFrameButton, SIGNAL(pressed()), this, SLOT(backwardStepButtonPressed()) );
	connect( _forwardOneFrameButton, SIGNAL(pressed()), this, SLOT(forwardStepButtonPressed()) );

	connect( _qtController, SIGNAL(simulationLoadedSignal()),   this, SLOT(handleSimulationLoadedSignal()) );
	connect( _qtController, SIGNAL(simulationUnloadedSignal()),   this, SLOT(handleSimulationUnloadedSignal()) );
	connect( _qtController, SIGNAL(simulationStartedSignal()),  this, SLOT(handleSimulationStartedSignal()) );
	connect( _qtController, SIGNAL(simulationPausedSignal()),   this, SLOT(handleSimulationPausedSignal()) );
	connect( _qtController, SIGNAL(simulationUnpausedSignal()), this, SLOT(handleSimulationUnpausedSignal()) );
	connect( _qtController, SIGNAL(simulationStoppedSignal()),  this, SLOT(handleSimulationStoppedSignal()) );
	connect( _qtController, SIGNAL(simulationAdvancedOneFrameSignal()),  this, SLOT(handleSimulationAdvancedOneFrameSignal()) );

	_initWidgetLayout();

	_setWidgetsToUnloadedState();
}

RecFilePlayerWidget::~RecFilePlayerWidget()
{
}

void RecFilePlayerWidget::recFileNameChanged(const QString & text)
{
	_recFilePlayerModule->_recFilename = text.toStdString();
}


void RecFilePlayerWidget::loadButtonPressed()
{
	_controller->loadSimulation();
}

void RecFilePlayerWidget::unloadButtonPressed()
{
	_controller->unloadSimulation();
}

void RecFilePlayerWidget::stopButtonPressed()
{
	_controller->stopSimulation();
}

void RecFilePlayerWidget::playButtonPressed()
{
	_controller->startSimulation();
}

void RecFilePlayerWidget::pauseButtonPressed()
{	
	_controller->pauseSimulation();
}

void RecFilePlayerWidget::unpauseButtonPressed()
{	
	_controller->unpauseSimulation();
}

void RecFilePlayerWidget::backwardStepButtonPressed()
{
	std::cerr << "WAIT!  backward-stepping not finished implementing yet!\n";
}

void RecFilePlayerWidget::forwardStepButtonPressed()
{
	_controller->pauseAndStepOneFrame();
}


//====================================


void RecFilePlayerWidget::handleSimulationLoadedSignal()
{
	_setWidgetsToLoadedState();
}

void RecFilePlayerWidget::handleSimulationUnloadedSignal()
{
	_setWidgetsToUnloadedState();
}

void RecFilePlayerWidget::handleSimulationStartedSignal()
{
	_setWidgetsToPlayingState();
}

void RecFilePlayerWidget::handleSimulationStoppedSignal()
{
	_setWidgetsToDoneState();
}


void RecFilePlayerWidget::handleSimulationPausedSignal()
{
	_setWidgetsToPausedState();
}

void RecFilePlayerWidget::handleSimulationUnpausedSignal()
{
	_setWidgetsToPlayingState();
}

void RecFilePlayerWidget::handleSimulationAdvancedOneFrameSignal()
{
}


void RecFilePlayerWidget::recFileNameBrowsePressed()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Rec Files (*.rec);;All files (*)"));

	if (!fileName.isEmpty()) {
		if (_recFileNameComboBox->findText(fileName) == -1)
			_recFileNameComboBox->addItem(fileName);
		_recFileNameComboBox->setCurrentIndex(_recFileNameComboBox->findText(fileName));
	}
}


void RecFilePlayerWidget::_initWidgetLayout()
{
	QGridLayout * _layout = new QGridLayout;

	QGridLayout * _inputsLayout = new QGridLayout;
	_inputsLayout->addWidget(new QLabel("Rec file: "),0,0);
	_inputsLayout->addWidget(_recFileNameComboBox,0,1,1,2);
	_inputsLayout->addWidget(_recFileNameBrowseButton,0,3);

	QGroupBox * _inputsGroup = new QGroupBox("Inputs");
	_inputsGroup->setLayout(_inputsLayout);

	//_layout->addWidget(_loadUnloadButton,2,0,1,4);

/*
	QGridLayout * _playBackLayout = new QGridLayout;
	_loadUnloadButton->setFixedWidth(140);
	_playBackLayout->addWidget(_loadUnloadButton,0,0,1,4);
	_playBackLayout->addWidget(_backwardOneFrameButton,0,4,1,1);
	_playBackLayout->addWidget(_stopButton,0,5,1,1);
	_playBackLayout->addWidget(_playPauseUnpauseButton,0,6,1,1);
	_playBackLayout->addWidget(_forwardOneFrameButton,0,7,1,1);
	_playBackLayout->addWidget( new QSlider(Qt::Horizontal), 2,0,1,7);
	_playBackLayout->addWidget( new QSpinBox(), 2,7,1,1);
	_playBackLayout->setAlignment(Qt::AlignCenter);
*/

	QGridLayout * _playBackLayout = new QGridLayout;
	_playBackLayout->addWidget(_loadUnloadButton,0,0,1,7);
	_playBackLayout->addWidget(_backwardOneFrameButton,1,0,1,1);
	_playBackLayout->addWidget(_stopButton,1,1,1,1);
	_playBackLayout->addWidget(_playPauseUnpauseButton,1,2,1,1);
	_playBackLayout->addWidget(_forwardOneFrameButton,1,3,1,1);
	QSpinBox * foo = new QSpinBox(); foo->setFixedWidth(100);
	_playBackLayout->addWidget( foo, 1,4,1,3);
	_playBackLayout->addWidget( new QSlider(Qt::Horizontal), 2,0,1,7);
	_playBackLayout->setAlignment(Qt::AlignCenter);

	QGroupBox * _playBackGroup = new QGroupBox("Playback Controls");
	_playBackGroup->setLayout(_playBackLayout);

	_layout->addWidget(_inputsGroup,0,0,3,4);
	_layout->addWidget(_playBackGroup,3,0,1,4);
	_layout->setAlignment(Qt::AlignTop | Qt::AlignJustify);

	setLayout(_layout);
} 

void RecFilePlayerWidget::_setWidgetsToUnloadedState()
{
	_recFileNameComboBox->setEnabled(true);
	_recFileNameBrowseButton->setEnabled(true);

	_stopButton->setEnabled(false);
	_playPauseUnpauseButton->setEnabled(false);
	_backwardOneFrameButton->setEnabled(false);
	_forwardOneFrameButton->setEnabled(false);

	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_START_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()) );

	_loadUnloadButton->setText("Load recording");
	_loadUnloadButton->disconnect(SIGNAL(pressed()));
	connect( _loadUnloadButton, SIGNAL(pressed()), this, SLOT(loadButtonPressed()) );
}

void RecFilePlayerWidget::_setWidgetsToLoadedState()
{
	_recFileNameComboBox->setEnabled(false);
	_recFileNameBrowseButton->setEnabled(false);

	_loadUnloadButton->setText("Unload recording");
	_loadUnloadButton->disconnect(SIGNAL(pressed()));
	connect( _loadUnloadButton, SIGNAL(pressed()), this, SLOT(unloadButtonPressed()) );

	_stopButton->setEnabled(false);
	_backwardOneFrameButton->setEnabled(false);
	_forwardOneFrameButton->setEnabled(false);

	_playPauseUnpauseButton->setEnabled(true);
	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_START_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()) );
}

void RecFilePlayerWidget::_setWidgetsToPlayingState()
{
	_recFileNameComboBox->setEnabled(false);
	_recFileNameBrowseButton->setEnabled(false);

	_loadUnloadButton->setText("Replay is in progress...");
	_loadUnloadButton->setEnabled(false);

	_stopButton->setEnabled(true);
	_backwardOneFrameButton->setEnabled(true);
	_forwardOneFrameButton->setEnabled(true);

	_playPauseUnpauseButton->setIcon(_pauseIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_PAUSE_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(pauseButtonPressed()) );
}

void RecFilePlayerWidget::_setWidgetsToPausedState()
{
	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_UNPAUSE_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(unpauseButtonPressed()) );
}

void RecFilePlayerWidget::_setWidgetsToDoneState()
{
	_recFileNameComboBox->setEnabled(false);
	_recFileNameBrowseButton->setEnabled(false);

	_stopButton->setEnabled(false);
	_backwardOneFrameButton->setEnabled(false);
	_forwardOneFrameButton->setEnabled(false);

	_playPauseUnpauseButton->setEnabled(false);
	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));

	_loadUnloadButton->setText("Play another recording...");
	_loadUnloadButton->setEnabled(true);
	_loadUnloadButton->disconnect(SIGNAL(pressed()));
	connect( _loadUnloadButton, SIGNAL(pressed()), this, SLOT(unloadButtonPressed()) );
}

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
