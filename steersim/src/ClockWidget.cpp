//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "qtgui/ClockWidget.h"

using namespace SteerSimQt;

ClockWidget::ClockWidget(SteerLib::EngineInterface * engine)
{
	_engine = engine;
}

void ClockWidget::init()
{
	_initWidgetLayout();

	QtEngineController * qtController = (QtEngineController*)_engine->getEngineController()->getQtEngineController();
	connect( qtController  , SIGNAL(simulationLoadedSignal()),   this, SLOT(handleSimulationLoadedSignal()) );
	connect( qtController  , SIGNAL(simulationUnloadedSignal()),   this, SLOT(handleSimulationUnloadedSignal()) );
	connect( qtController  , SIGNAL(simulationStartedSignal()),  this, SLOT(handleSimulationStartedSignal()) );
	connect( qtController  , SIGNAL(simulationPausedSignal()),   this, SLOT(handleSimulationPausedSignal()) );
	connect( qtController  , SIGNAL(simulationUnpausedSignal()), this, SLOT(handleSimulationUnpausedSignal()) );
	connect( qtController  , SIGNAL(simulationStoppedSignal()),  this, SLOT(handleSimulationStoppedSignal()) );
	connect( qtController  , SIGNAL(realTimeUpdatedSignal()),  this, SLOT(handleRealTimeUpdatedSignal()) );
	connect( qtController  , SIGNAL(simulationAdvancedOneFrameSignal()),  this, SLOT(handleSimulationAdvancedOneFrameSignal()) );

}


void ClockWidget::_initWidgetLayout()
{
	QVBoxLayout * layout = new QVBoxLayout;
	QGridLayout * clockModeLayout = new QGridLayout;
	QGridLayout * clockInfoLayout = new QGridLayout;

	_realTimeLineEdit = new QLineEdit();
	_realTimeLineEdit->setReadOnly(true);
	_realTimeLineEdit->setText(QTime(0,0,0,0).toString("hh:mm:ss.zzz"));
	_realTimeLineEdit->setFixedWidth(100);
	_realTimeLineEdit->setAlignment(Qt::AlignRight);

	_realFpsLineEdit = new QLineEdit();
	_realFpsLineEdit->setReadOnly(true);
	_realFpsLineEdit->setText(QTime(0,0,0,0).toString("60.3"));
	_realFpsLineEdit->setFixedWidth(60);
	_realFpsLineEdit->setAlignment(Qt::AlignRight);

	_simulationTimeLineEdit = new QLineEdit();
	_simulationTimeLineEdit->setReadOnly(true);
	_simulationTimeLineEdit->setText(QTime(0,0,0,0).toString("hh:mm:ss.zzz"));
	_simulationTimeLineEdit->setFixedWidth(100);
	_simulationTimeLineEdit->setAlignment(Qt::AlignRight);

	_simulationFrameLineEdit = new QLineEdit();
	_simulationFrameLineEdit->setReadOnly(true);
	_simulationFrameLineEdit->setText(QTime(0,0,0,0).toString("123"));
	_simulationFrameLineEdit->setFixedWidth(60);
	_simulationFrameLineEdit->setAlignment(Qt::AlignRight);


	clockInfoLayout->addWidget(new QLabel("Real-time fps: "),0,1);
	clockInfoLayout->addWidget(_realFpsLineEdit,1,1);
	clockInfoLayout->addWidget(new QLabel("Real-time clock: "),0,0);
	clockInfoLayout->addWidget(_realTimeLineEdit,1,0);
	clockInfoLayout->addWidget(new QLabel("Current frame: "),2,1);
	clockInfoLayout->addWidget(_simulationFrameLineEdit,3,1);
	clockInfoLayout->addWidget(new QLabel("Simulation clock: "),2,0);
	clockInfoLayout->addWidget(_simulationTimeLineEdit,3,0);

//	_infoLabel = new QLabel("Real-time: \nfps: \nSimulation time: \nCurrent frame: \n");
//	clockInfoLayout->addWidget(_infoLabel);


	QGroupBox * clockInfoGroup = new QGroupBox("Clock status");
	clockInfoGroup->setLayout(clockInfoLayout);
	//clockInfoGroup->setAlignment(Qt::AlignCenter);


	QGroupBox * clockModeGroup = new QGroupBox("Clock mode");
	clockModeGroup->setLayout(clockModeLayout);
	//clockModeGroup->setAlignment(Qt::AlignCenter);

	//layout->addWidget(clockModeGroup,0,0,1,1);
	//layout->addWidget(clockInfoGroup,1,0,1,1);
	//layout->addWidget(clockModeGroup);
	layout->addWidget(clockInfoGroup);
	layout->setAlignment(Qt::AlignTop | Qt::AlignJustify);
	setLayout(layout);
} 

void ClockWidget::handleSimulationLoadedSignal()
{
}

void ClockWidget::handleSimulationUnloadedSignal()
{
}

void ClockWidget::handleSimulationStartedSignal()
{
}

void ClockWidget::handleSimulationStoppedSignal()
{
}

void ClockWidget::handleSimulationPausedSignal()
{
}

void ClockWidget::handleSimulationUnpausedSignal()
{
}

void ClockWidget::handleRealTimeUpdatedSignal()
{
	SteerLib::Clock & clockReference = _engine->getClock();
	float realTime = clockReference.getCurrentRealTime();
	float fps = clockReference.getRealFps();

	int hour = ((int)(realTime))/3600;
	int min = ((int)(realTime))/60 - hour*60;
	int sec = ((int)(realTime)) - hour*3600 - min*60;
	int ms = ((int)(realTime*1000.0f)) - hour*3600000 - min*60000 - sec*1000;
	_realTimeLineEdit->setText( QTime(hour,min,sec,ms).toString("hh:mm:ss.zzz") );
	_realFpsLineEdit->setText(QString::number(fps,'f',2));


}

void ClockWidget::handleSimulationAdvancedOneFrameSignal()
{
	SteerLib::Clock & clockReference = _engine->getClock();
	float simTime = clockReference.getCurrentSimulationTime();
	unsigned int currentFrame = clockReference.getCurrentFrameNumber();

	int hour = ((int)(simTime))/3600;
	int min = ((int)(simTime))/60 - hour*60;
	int sec = ((int)(simTime)) - hour*3600 - min*60;
	int ms = ((int)(simTime*1000.0f)) - hour*3600000 - min*60000 - sec*1000;
	_simulationTimeLineEdit->setText( 	QTime(hour,min,sec,ms).toString("hh:mm:ss.zzz") );
	_simulationFrameLineEdit->setText( QString::number(currentFrame) );
}


#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
