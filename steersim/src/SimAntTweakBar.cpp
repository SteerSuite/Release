//
// Copyright (c) 2009-2014 Glen Berseth, Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * SimAntTweakBar.cpp
 *
 *  Created on: 2013-08-23
 *      Author: glenpb
 */

#include "core/SimAntTweakBar.h"
#include <sstream>


using namespace SteerLib;


SimAntTweakBar::SimAntTweakBar(SteerLib::SimulationEngine * gEngine, SteerLib::SimulationOptions * options, int sizeX, int sizeY)
{
	_gEngine = gEngine;
	_real_fps = 0;
	int blending = 0;
	_options = options;
	_barName = new std::string("SimStats");
	_numActiveAgents = 0;
	_frameNumber = _gEngine->getClock().getCurrentFrameNumber();

	TwInit(TW_OPENGL, NULL);

	// Create a tweak bar
	_bar = TwNewBar(_barName->c_str());

	updateWindowSize(sizeX, sizeY);

	int alpha = 190;
	TwSetParam(_bar, NULL, "alpha", TW_PARAM_INT32, 1, &alpha);
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.

	// Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].

	TwAddVarRO(_bar, "fps", TW_TYPE_DOUBLE, &_real_fps,
				   " label='FPS' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");

	TwAddVarRO(_bar, "agents", TW_TYPE_INT32, &_numActiveAgents,
					   " label='agents' min=0 max=2000000 step=1 keyIncr=s keyDecr=S help='The number of agents in the simulation' ");

	TwAddVarRW(_bar, "Frame", TW_TYPE_UINT32, &_frameNumber,
			               " label='Frame' min=0 max=1000 step=1 keyIncr=l keyDecr=k help='Controls the current frame of the Simulation' ");


	// TwAddVarRW(_bar, "bgColor", TW_TYPE_COLOR3F, &_bgColour, " label='Background color' ");

	this->updateBarSize();
	this->updateBarPosition();
}

void SimAntTweakBar::draw()
{
	this->setRealFPS();
	this->updateActiveAgents();
	this->updateFrameNumber();

	TwDraw();

}

void SimAntTweakBar::updateFrameNumber()
{
	// Maybe this will work
	// int diff = abs(((int)_frameNumber) - _gEngine->getClock().getCurrentFrameNumber())-1;
	// diff = diff << 1;
	// diff = 1 >> diff;
	// std::cout << "diff is " << diff << std::endl;
	// std::cout << "frameNumber " << _frameNumber << " clock frame " <<
		//	_gEngine->getClock().getCurrentFrameNumber() << std::endl;
	/*
	for (int i=0; i < diff; i++)
	{
		_gEngine->getClock().backupSimulationAndUpdateRealTime();
	}*/
	_frameNumber = _gEngine->getClock().getCurrentFrameNumber();
}

void SimAntTweakBar::setRealFPS()
{
	this->_real_fps = _gEngine->getClock().getRealFps();

}

void SimAntTweakBar::updateWindowSize(int sizeX, int sizeY)
{
	_wsizeX=sizeX;
	_wsizeY=sizeY;
	TwWindowSize(sizeX, sizeY);
	updateBarPosition();
}

void SimAntTweakBar::updateBarPosition()
{
	int size[2];
	TwGetParam(_bar, NULL, "size", TW_PARAM_INT32, 2, size);

	int x= _wsizeX-size[0], y=0;


	std::stringstream command;
	command << " " << *_barName << " position='" ;
	command << x << " ";
	command << y << "'";

	TwDefine( command.str().c_str());
	// TwDefine( " << _barName << " position='200 40' ");

}

void SimAntTweakBar::updateBarSize()
{
	// TwDefine(" TweakBar size='180 80' ");

	int x=320, y=200;
	std::stringstream command;
	command << " " << *_barName << " size='" ;
	command << x << " ";
	command << y << "'";

	TwDefine( command.str().c_str());

}

void SimAntTweakBar::updateActiveAgents()
{
	int count=0;
	for (int i=0; i < _gEngine->getAgents().size(); i++)
	{
		if ( _gEngine->getAgents().at(i)->enabled() )
		{
			count++;
		}
	}
	_numActiveAgents = count;
}

SimAntTweakBar::~SimAntTweakBar()
{
	// TODO Auto-generated destructor stub

	delete _barName;
	delete _bar;

}

