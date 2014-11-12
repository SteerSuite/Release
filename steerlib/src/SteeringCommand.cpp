//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SteeringCommand.cpp
/// @brief Implements the SteerLib::SteeringCommand helper class.

#include "simulation/SteeringCommand.h"

using namespace SteerLib;

void SteeringCommand::clear()
{
	steeringMode = LOCOMOTION_MODE_COMMAND;
	aimForTargetDirection = false;
	targetDirection = Util::Vector(1.0f, 0.0f, 0.0f);
	turningAmount = 0.0f;
	aimForTargetSpeed = false;
	targetSpeed = 0.0f;
	acceleration = 0.0f;
	scoot = 0.0f;
	dynamicsSteeringForce = Util::Vector(0.0f, 0.0f, 0.0f);
}


