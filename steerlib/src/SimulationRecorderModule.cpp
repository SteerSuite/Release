//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SimulationRecorderModule.cpp
/// @brief Implements the SimulationRecorderModule built-in module.

#include "modules/SimulationRecorderModule.h"
#include "simulation/SimulationOptions.h"

using namespace SteerLib;

void SimulationRecorderModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) {

	_recFilename = "";
	_engine = engineInfo;
	_simulationWriter = NULL;

	// parse the options
	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		if ((*optionIter).first == "retime") {
		}
		else if ((*optionIter).first == "recfile") {
			_recFilename = (*optionIter).second;
		}
	}

	//if (_recFilename == "") {
	//	throw Util::GenericException("No rec file specified for recording.");
	//}

	_initialized = false;



}

void SimulationRecorderModule::preprocessSimulation() {

	if (_initialized) return; 

	_simulationWriter = new SteerLib::RecFileWriter();

	// note, these are aliases (using the &)
	const std::vector<SteerLib::AgentInterface*> & agents = _engine->getAgents();
	const std::set<SteerLib::ObstacleInterface*> & obstacles = _engine->getObstacles();

	// if the simulation is reading a usual test case, then this filename is stored in 
	// the recfile so that initial conditions can be validated.
	// otherwise, the recfile is not associated with any test case filename.
	if (_engine->isModuleLoaded("testCasePlayer"))
		_simulationWriter->startRecording(agents.size(), _recFilename,  (*_engine->getModuleOptions("testCasePlayer").find("testcase")).second );
	else
		_simulationWriter->startRecording(agents.size(), _recFilename);

	std::set<SteerLib::ObstacleInterface*>::const_iterator obstacleIter;
	for (obstacleIter = obstacles.begin(); obstacleIter != obstacles.end(); ++obstacleIter) {
		_simulationWriter->addObstacleBoundingBox((*obstacleIter)->getBounds());
	}

	// Technically, the number of frames is one more than the number of simulation steps taken.
	// here, write the zero-th frame, which represents initial conditions.

	if ((_engine->getClock().getCurrentFrameNumber() != 0) || (_engine->getClock().getCurrentSimulationTime() != 0.0f)) {
		throw Util::GenericException("The simulationRecorder module made a bad assumption that simulation time was 0.0 during initialization; apparently this needs to be fixed.");
	}
	_simulationWriter->startFrame(_engine->getClock().getCurrentSimulationTime(), 0.0f);
	for (unsigned int i=0; i<agents.size(); i++) {
		// These values must be strictly initialized, just in case the agent is not enabled.
		// This is necessary so that two rec files will be exactly the same if the simulations were exactly the same.
		Util::Point pos(0.0f, 0.0f, 0.0f);
		Util::Vector dir(0.0f, 0.0f, 0.0f);
		Util::Point goal(0.0f, 0.0f, 0.0f);
		float radius = 0;
		bool enabled = agents[i]->enabled();
		if (enabled) {
			pos =  agents[i]->position();
			dir =  agents[i]->forward();
			assert(dir.lengthSquared() != 0.0f);
			goal = agents[i]->currentGoal().targetLocation;
			radius = agents[i]->radius();
		}
		_simulationWriter->setAgentInfoForCurrentFrame(i,pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, goal.x, goal.y, goal.z, radius, enabled);
	}
	_simulationWriter->finishFrame();


	_initialized = true;

}


void SimulationRecorderModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber) {

	// note, this is an alias (using the &)
	const std::vector<SteerLib::AgentInterface *>  & agents = _engine->getAgents();

	//std::cout << " coming here \n";

	_simulationWriter->startFrame(_engine->getClock().getCurrentSimulationTime(), dt);
	for (unsigned int i=0; i<agents.size(); i++) {
		// These values must be strictly initialized, just in case the agent is not enabled.
		// This is necessary so that two rec files will be exactly the same if the simulations were exactly the same.
		Util::Point pos(0.0f, 0.0f, 0.0f);
		Util::Vector dir(0.0f, 0.0f, 0.0f);
		Util::Point goal(0.0f, 0.0f, 0.0f);
		float radius = 0;
		bool enabled = agents[i]->enabled();
		if (enabled) {
			pos =  agents[i]->position();
			dir =  agents[i]->forward();
			//assert(dir.lengthSquared() != 0.0f);
			goal = agents[i]->currentGoal().targetLocation;
			radius = agents[i]->radius();
		}
		_simulationWriter->setAgentInfoForCurrentFrame(i,pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, goal.x, goal.y, goal.z, radius, enabled);
	}
	_simulationWriter->finishFrame();

}

void SimulationRecorderModule::postprocessSimulation() {

	if (!_initialized ) return;

	assert(_simulationWriter->isOpen() && _simulationWriter->isRecording());

	if (_simulationWriter->isWritingFrame()) {
		_simulationWriter->finishFrame();
	}

	_simulationWriter->finishRecording();
#ifdef _DEBUG
	std::cout << "Wrote " << _engine->getClock().getCurrentFrameNumber()+1 << " frames. (one extra frame for initial conditions)" << std::endl;
#endif
	delete _simulationWriter;
}

