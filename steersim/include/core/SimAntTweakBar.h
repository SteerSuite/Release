//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * SimAntTweakBar.h
 *
 *  Created on: 2013-08-23
 *      Author: glenpb
 */

#ifndef SIMANTTWEAKBAR_H_
#define SIMANTTWEAKBAR_H_

// #include "core/GLFWEngineDriver.h"
#include "SteerLib.h"
#include "AntTweakBar/include/AntTweakBar.h"
#include <vector>


class SimAntTweakBar
{
public:
	SimAntTweakBar(SteerLib::SimulationEngine * gEngine, SteerLib::SimulationOptions * options, int sizeX, int sizeY);
	virtual ~SimAntTweakBar();

	void draw();
	void setRealFPS();
	void updateWindowSize(int, int);
	void updateBarPosition();
	void updateBarSize();
	void updateWindowSize();
	void updateActiveAgents();
	void updateSteeringAlgorithmParams();
	void updateSteeringAlgorithmBlending();
	void updateFrameNumber();

private:
	double speed;
	double _real_fps;
	float _max_speed;
	float _bgColour[3];
	unsigned int _frameNumber;
	float bgColor[3];
	float g_Rotation[4];

	std::string * _barName;

	SteerLib::SimulationEngine * _gEngine;
	TwBar * _bar;
	int _wsizeX, _wsizeY;
	int _numActiveAgents;

	SteerLib::SimulationOptions * _options;
};

#endif /* SIMANTTWEAKBAR_H_ */
