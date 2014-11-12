//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file CommandLineEngineDriver.cpp
/// @brief Implements the CommandLineEngineDriver functionality.
///
/// @todo
///   - update documentation in this file
///

#include <iostream>
#include "core/CommandLineEngineDriver.h"

using namespace std;
using namespace SteerLib;
using namespace Util;

//
// constructor
//
CommandLineEngineDriver::CommandLineEngineDriver()
{
	_alreadyInitialized = false;
	_engine = NULL;
}


//
// init()
//
void CommandLineEngineDriver::init(SteerLib::SimulationOptions * options)
{
	if (_alreadyInitialized) {
		throw GenericException("CommandLineEngineDriver::init() - should not call this function twice.\n");
	}

	_alreadyInitialized = true;

	_engine = new SimulationEngine();
	_engine->init(options, this);
}


//
// run() - never returns, will end the program properly when appropriate
//
void CommandLineEngineDriver::run()
{
	#ifdef _DEBUG1
	bool verbose = true;  // TODO: make this a user option...
	#else
	bool verbose = false;
	#endif
	bool done = false;

	if (verbose) std::cout << "\rInitializing...\n";
	_engine->initializeSimulation();

	if (verbose) std::cout << "\rPreprocessing...\n";
	_engine->preprocessSimulation();

	// loop until the engine tells us its done
	while (!done) {
		if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber();
		done = !_engine->update(false);
	}

	if (verbose) std::cout << "\rFrame Number:   " << _engine->getClock().getCurrentFrameNumber() << std::endl;

	if (verbose) std::cout << "\rPostprocessing...\n";
	_engine->postprocessSimulation();

	if (verbose) std::cout << "\rCleaning up...\n";
	_engine->cleanupSimulation();

	if (verbose) std::cout << "\rDone.\n";
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

const char * CommandLineEngineDriver::getData()
{
	ModuleInterface * moduleInterface = (_engine->getModule("scenario"));
	ModuleInterface * aimoduleInterface = (_engine->getModule("sfAI"));
	std::string out;
	char * outptr;
	if ( moduleInterface != NULL && (aimoduleInterface != NULL) )
	{
		std::string out1 = moduleInterface->getData();
		std::string out2 = aimoduleInterface->getData();

		std::vector<std::string> vout1 = split(out1, '\n');
		std::vector<std::string> vout2 = split(out2, '\n');

		for (unsigned int i =0; i < vout1.size(); i++)
		{
			// I think the space is neccessary to sperate item
			// consider changing to another delimiter
			out = out + vout1.at(i) + " " + vout2.at(i) + "\n";
		}

		outptr = (char *) malloc(sizeof(char) * out.length());
		strncpy(outptr, out.c_str(), out.length());
		return outptr;
	}
	else
	{
		char * out = (char *) malloc(sizeof(char)*20);
		strncpy(out, "EXIT_SUCCESS_", 20);
		return out;
	}
}

LogData * CommandLineEngineDriver::getLogData()
{
	ModuleInterface * moduleInterface = (_engine->getModule("scenario"));
	LogData * lD = moduleInterface->getLogData();
	ModuleInterface * aimoduleInterface = (_engine->getModule("sfAI"));

	// TODO use this properly instead.
	std::vector<SteerLib::ModuleInterface*> modules = _engine-> getAllModules();

	if ( (aimoduleInterface != NULL) )
	{
		lD->appendLogData(aimoduleInterface->getLogData());
	}
	return lD;
}

//
// finish() - cleans up.
//
void CommandLineEngineDriver::finish()
{
	_engine->finish();

	delete _engine;
}

