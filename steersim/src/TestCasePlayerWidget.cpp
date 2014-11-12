//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file TestCasePlayerWidget.cpp
/// @brief Implements the TestCasePlayerWidget class

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "SteerLib.h"
#include "core/SteerSimOptions.h"
#include "qtgui/Tooltip.h"
#include "qtgui/TestCasePlayerWidget.h"
#include "modules/TestCasePlayerModule.h"

using namespace Util;
using namespace SteerLib;
using namespace SteerSimQt;

TestCasePlayerWidget::TestCasePlayerWidget(TestCasePlayerModule * testCasePlayerModule, SteerLib::EngineInterface * engine)
{
	_testCasePlayerModule = testCasePlayerModule;
	_engine = engine;
	_controller = _engine->getEngineController();
	_qtController = (QtEngineController*)(_controller->getQtEngineController());
	
	_testCaseNameComboBox = new QComboBox();
	_testCaseNameComboBox->addItem(QString::fromStdString(_testCasePlayerModule->_testCaseFilename));
	_testCaseNameComboBox->setEditable(true);
	_testCaseNameComboBox->setToolTip(TOOL_TIP_TEST_CASE_NAME);
	_testCaseNameComboBox->setFixedWidth(155);

	_testCaseNameBrowseButton = new QPushButton("...");
	_testCaseNameBrowseButton->setMaximumWidth(30);

	_moduleNameComboBox = new QComboBox();
	_moduleNameComboBox->addItem(QString::fromStdString(_testCasePlayerModule->_aiModuleName));
	_moduleNameComboBox->setEditable(true);
	_moduleNameComboBox->setToolTip(TOOL_TIP_MODULE_NAME);
	_moduleNameComboBox->setFixedWidth(155);

	_moduleNameBrowseButton = new QPushButton("...");
	_moduleNameBrowseButton->setMaximumWidth(30);

	_frameNumberLineEdit = new QLineEdit(QString("%1").arg( _engine->getClock().getCurrentFrameNumber() ));
	_frameNumberLineEdit->setMaxLength(10);
	_frameNumberLineEdit->setMaximumWidth(70);
	_frameNumberLineEdit->setAlignment(Qt::AlignRight);
	_frameNumberLineEdit->setReadOnly(true);
	_frameNumberLineEdit->setToolTip(TOOL_TIP_FRAME_NUMBER);

	_loadUnloadButton = new QPushButton("Load test case");
	//_loadUnloadButton->setToolTop(...)

	_playPauseUnpauseButton = new QPushButton();
	_playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	_pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	_playPauseUnpauseButton->setIcon(_playIcon);

	_stopButton = new QPushButton();
	_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	_stopButton->setToolTip(TOOL_TIP_STOP_BUTTON);

	_forwardOneFrameButton = new QPushButton();
	_forwardOneFrameButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	_forwardOneFrameButton->setToolTip(TOOL_TIP_STEP_FORWARD_BUTTON);

	connect( _testCaseNameComboBox, SIGNAL(editTextChanged(const QString)), this, SLOT(testCaseNameChanged(const QString)) );
	connect( _moduleNameComboBox, SIGNAL(editTextChanged(const QString)), this, SLOT(aiModuleNameChanged(const QString)) );
	connect( _testCaseNameBrowseButton, SIGNAL(pressed()), this, SLOT(testCaseNameBrowsePressed()) );
	connect( _moduleNameBrowseButton, SIGNAL(pressed()), this, SLOT(moduleNameBrowsePressed()) );
	connect( _stopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()) );
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

TestCasePlayerWidget::~TestCasePlayerWidget()
{
}

void TestCasePlayerWidget::testCaseNameChanged(const QString & text)
{
	_testCasePlayerModule->_testCaseFilename = text.toStdString();
}

void TestCasePlayerWidget::aiModuleNameChanged(const QString & text)
{

}


void TestCasePlayerWidget::loadButtonPressed()
{
	// NOTE CAREFULLY, these are aliases for code readability;
	// but they are actually directly modifying the _testCasePlayerModule's member variables.
	std::string & _aiModuleName = _testCasePlayerModule->_aiModuleName;
	std::string & _aiModuleSearchPath = _testCasePlayerModule->_aiModuleSearchPath;

#ifdef _WIN32
	std::string extension = ".dll";
#else
	std::string extension = ".o";
#endif
	std::string fullPath = _moduleNameComboBox->currentText().toStdString();;
	_aiModuleName = Util::basename(fullPath, extension);
	_aiModuleSearchPath = fullPath.substr(0, fullPath.find_last_of("/\\")+1);
	if (_aiModuleSearchPath == _aiModuleName) {
		_aiModuleSearchPath = "";
	}


	// before asking the engine driver to load, we should change the AI module in the test case player module.
	// load the module if it is not already
	if ( !_engine->isModuleLoaded(_aiModuleName) ) {
		_engine->loadModule(_aiModuleName, _aiModuleSearchPath, "");
		/// @todo add a boolean that flags this module was loaded here, so that we can unload the module after the simulation is done.

		// get a pointer to the loaded module
		_testCasePlayerModule->_aiModule = _engine->getModule(_aiModuleName);
	}


	_controller->loadSimulation();
}

void TestCasePlayerWidget::unloadButtonPressed()
{
	_controller->unloadSimulation();
}

void TestCasePlayerWidget::stopButtonPressed()
{
	_controller->stopSimulation();
}

void TestCasePlayerWidget::playButtonPressed()
{
	_controller->startSimulation();
}

void TestCasePlayerWidget::pauseButtonPressed()
{	
	_controller->pauseSimulation();
}

void TestCasePlayerWidget::unpauseButtonPressed()
{	
	_controller->unpauseSimulation();
}

void TestCasePlayerWidget::forwardStepButtonPressed()
{
	_controller->pauseAndStepOneFrame();
}


//====================================


void TestCasePlayerWidget::handleSimulationLoadedSignal()
{
	_setWidgetsToLoadedState();
}

void TestCasePlayerWidget::handleSimulationUnloadedSignal()
{
	_setWidgetsToUnloadedState();
}

void TestCasePlayerWidget::handleSimulationStartedSignal()
{
	_setWidgetsToPlayingState();
}

void TestCasePlayerWidget::handleSimulationStoppedSignal()
{
	_setWidgetsToDoneState();
}


void TestCasePlayerWidget::handleSimulationPausedSignal()
{
	_setWidgetsToPausedState();
}

void TestCasePlayerWidget::handleSimulationUnpausedSignal()
{
	_setWidgetsToPlayingState();
}

void TestCasePlayerWidget::handleSimulationAdvancedOneFrameSignal()
{
}


void TestCasePlayerWidget::testCaseNameBrowsePressed()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QString::fromStdString(_engine->getTestCaseSearchPath()),tr("Test Case Files (*.xml);;All files (*)"));

	if (!fileName.isEmpty()) {
		if (_testCaseNameComboBox->findText(fileName) == -1)
			_testCaseNameComboBox->addItem(fileName);
		_testCaseNameComboBox->setCurrentIndex(_testCaseNameComboBox->findText(fileName));
	}
}

void TestCasePlayerWidget::moduleNameBrowsePressed()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Find Module"),QString::fromStdString(_engine->getModuleSearchPath()),tr("AI Module (*.o *.dll);;All files (*)"));
	
	if (!fileName.isEmpty()) {
         if (_moduleNameComboBox->findText(fileName) == -1)
             _moduleNameComboBox->addItem(fileName);
         _moduleNameComboBox->setCurrentIndex(_moduleNameComboBox->findText(fileName));
	}
}

void TestCasePlayerWidget::_initWidgetLayout()
{
	QGridLayout * _layout = new QGridLayout;
	/*
	_layout->addWidget(new QLabel("Test case: "),0,0);
	_layout->addWidget(_testCaseNameComboBox,0,1,1,2);
	_layout->addWidget(_testCaseNameBrowseButton,0,3);
	_layout->addWidget(new QLabel("AI module: "),1,0);
	_layout->addWidget(_moduleNameComboBox,1,1,1,2);
	_layout->addWidget(_moduleNameBrowseButton,1,3);
	*/
	QGridLayout * _inputsLayout = new QGridLayout;
	_inputsLayout->addWidget(new QLabel("Test case: "),0,0);
	_inputsLayout->addWidget(_testCaseNameComboBox,0,1,1,2);
	_inputsLayout->addWidget(_testCaseNameBrowseButton,0,3);
	_inputsLayout->addWidget(new QLabel("AI module: "),1,0);
	_inputsLayout->addWidget(_moduleNameComboBox,1,1,1,2);
	_inputsLayout->addWidget(_moduleNameBrowseButton,1,3);

	QGroupBox * _inputsGroup = new QGroupBox("Inputs");
	_inputsGroup->setLayout(_inputsLayout);

	//_layout->addWidget(_loadUnloadButton,2,0,1,4);

	QHBoxLayout * _playBackLayout = new QHBoxLayout;
	_loadUnloadButton->setFixedWidth(140);
	_playBackLayout->addWidget(_loadUnloadButton);
	_playBackLayout->addWidget(_stopButton);
	_playBackLayout->addWidget(_playPauseUnpauseButton);
	_playBackLayout->addWidget(_forwardOneFrameButton);
	//_playBackLayout->addWidget(new QLabel("Frame Number: "));
	//_playBackLayout->addWidget(_frameNumberLineEdit);
	_playBackLayout->setAlignment(Qt::AlignCenter);
	//_playBackLayout->addStretch(1);

	QGroupBox * _playBackGroup = new QGroupBox("Playback Controls");
	_playBackGroup->setLayout(_playBackLayout);

	_layout->addWidget(_inputsGroup,0,0,3,4);
	_layout->addWidget(_playBackGroup,3,0,1,4);
	_layout->setAlignment(Qt::AlignTop | Qt::AlignJustify);

	setLayout(_layout);
} 

void TestCasePlayerWidget::_setWidgetsToUnloadedState()
{
	_testCaseNameComboBox->setEnabled(true);
	_moduleNameComboBox->setEnabled(true);
	_testCaseNameBrowseButton->setEnabled(true);
	_moduleNameBrowseButton->setEnabled(true);

	_stopButton->setEnabled(false);
	_playPauseUnpauseButton->setEnabled(false);
	_forwardOneFrameButton->setEnabled(false);

	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_START_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()) );

	_loadUnloadButton->setText("Load test case");
	_loadUnloadButton->disconnect(SIGNAL(pressed()));
	connect( _loadUnloadButton, SIGNAL(pressed()), this, SLOT(loadButtonPressed()) );
}

void TestCasePlayerWidget::_setWidgetsToLoadedState()
{
	_testCaseNameComboBox->setEnabled(false);
	_moduleNameComboBox->setEnabled(false);
	_testCaseNameBrowseButton->setEnabled(false);
	_moduleNameBrowseButton->setEnabled(false);

	_loadUnloadButton->setText("Unload test case");
	_loadUnloadButton->disconnect(SIGNAL(pressed()));
	connect( _loadUnloadButton, SIGNAL(pressed()), this, SLOT(unloadButtonPressed()) );

	_stopButton->setEnabled(false);
	_forwardOneFrameButton->setEnabled(false);

	_playPauseUnpauseButton->setEnabled(true);
	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_START_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()) );
}

void TestCasePlayerWidget::_setWidgetsToPlayingState()
{
	_testCaseNameComboBox->setEnabled(false);
	_moduleNameComboBox->setEnabled(false);
	_testCaseNameBrowseButton->setEnabled(false);
	_moduleNameBrowseButton->setEnabled(false);

	_loadUnloadButton->setText("Simulation is in progress...");
	_loadUnloadButton->setEnabled(false);

	_stopButton->setEnabled(true);
	_forwardOneFrameButton->setEnabled(true);

	_playPauseUnpauseButton->setIcon(_pauseIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_PAUSE_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(pauseButtonPressed()) );
}

void TestCasePlayerWidget::_setWidgetsToPausedState()
{
	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->setToolTip(TOOL_TIP_UNPAUSE_BUTTON);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));
	connect( _playPauseUnpauseButton, SIGNAL(pressed()), this, SLOT(unpauseButtonPressed()) );
}

void TestCasePlayerWidget::_setWidgetsToDoneState()
{
	_testCaseNameComboBox->setEnabled(false);
	_moduleNameComboBox->setEnabled(false);
	_testCaseNameBrowseButton->setEnabled(false);
	_moduleNameBrowseButton->setEnabled(false);

	_stopButton->setEnabled(false);
	_forwardOneFrameButton->setEnabled(false);

	_playPauseUnpauseButton->setEnabled(false);
	_playPauseUnpauseButton->setIcon(_playIcon);
	_playPauseUnpauseButton->disconnect(SIGNAL(pressed()));

	_loadUnloadButton->setText("New simulation...");
	_loadUnloadButton->setEnabled(true);
	_loadUnloadButton->disconnect(SIGNAL(pressed()));
	connect( _loadUnloadButton, SIGNAL(pressed()), this, SLOT(unloadButtonPressed()) );
}

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
