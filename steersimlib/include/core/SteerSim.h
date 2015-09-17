//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file steersim/src/Main.cpp
/// @brief Entry point of SteerSim.

#include <exception>
#include "SteerLib.h"
#include "core/CommandLineEngineDriver.h"
#include "core/GLFWEngineDriver.h"
#include "core/QtEngineDriver.h"
#include "SimulationPlugin.h"


void STEERLIB_API initializeOptionsFromCommandLine( int argc, char **argv, SteerLib::SimulationOptions & simulationOptions );
// const char * steersuite_init(int argc, char ** argv);
LogData * init_steersuite(int argc, char ** argv);


