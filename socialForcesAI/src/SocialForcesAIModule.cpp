//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file SocialForcesAIModule.cpp
/// @brief Implements the SocialForcesAIModule plugin.


#include "SimulationPlugin.h"
#include "SocialForcesAIModule.h"
#include "SocialForcesAgent.h"

#include "LogObject.h"
#include "LogManager.h"


// globally accessible to the simpleAI plugin
// SteerLib::EngineInterface * gEngine;
// SteerLib::SpatialDataBaseInterface * gSpatialDatabase;

namespace SocialForcesGlobals
{

	// SteerLib::EngineInterface * gEngineInfo;
	// SteerLib::SpatialDataBaseInterface * gSpatialDatabase;
	unsigned int gLongTermPlanningPhaseInterval;
	unsigned int gMidTermPlanningPhaseInterval;
	unsigned int gShortTermPlanningPhaseInterval;
	unsigned int gPredictivePhaseInterval;
	unsigned int gReactivePhaseInterval;
	unsigned int gPerceptivePhaseInterval;
	bool gUseDynamicPhaseScheduling;
	bool gShowStats;
	bool gShowAllStats;
	bool dont_plan;


	// Adding a bunch of parameters so they can be changed via input
	float sf_acceleration;
	float sf_personal_space_threshold;
	float sf_agent_repulsion_importance;
	float sf_query_radius;
	float sf_body_force;
	float sf_agent_body_force;
	float sf_sliding_friction_force;
	float sf_agent_b;
	float sf_agent_a;
	float sf_wall_b;
	float sf_wall_a;
	float sf_max_speed;


	PhaseProfilers * gPhaseProfilers;
}

using namespace SocialForcesGlobals;

PLUGIN_API SteerLib::ModuleInterface * createModule()
{
	return new SocialForcesAIModule;
}

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module )
{
	delete module;
}


void SocialForcesAIModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	// gEngine = engineInfo;
	// gSpatialDatabase = engineInfo->getSpatialDatabase();
	_gEngine = engineInfo;		
	_data = "";

	gUseDynamicPhaseScheduling = false;
	gShowStats = false;
	logStats = false;
	gShowAllStats = false;
	logFilename = "sfAI.log";
	dont_plan = false;

	sf_acceleration = ACCELERATION;
	sf_personal_space_threshold = PERSONAL_SPACE_THRESHOLD;
	sf_agent_repulsion_importance = AGENT_REPULSION_IMPORTANCE;
	sf_query_radius = QUERY_RADIUS;
	sf_body_force = BODY_FORCE;
	sf_agent_body_force = AGENT_BODY_FORCE;
	sf_sliding_friction_force = SLIDING_FRICTION_FORCE;
	sf_agent_b = AGENT_B;
	sf_agent_a = AGENT_A;
	sf_wall_b = WALL_B;
	sf_wall_a = WALL_A;
	sf_max_speed = MAX_SPEED;


	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		std::stringstream value((*optionIter).second);
		// std::cout << "option " << (*optionIter).first << " value " << value.str() << std::endl;
		if ((*optionIter).first == "")
		{
			value >> gLongTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "sf_acceleration")
		{
			value >> sf_acceleration;
			std::cout << "set sf acceleration to " << sf_acceleration << std::endl;
		}
		else if ((*optionIter).first == "sf_personal_space_threshold")
		{
			value >> sf_personal_space_threshold;
		}
		else if ((*optionIter).first == "sf_agent_repulsion_importance")
		{
			value >> sf_agent_repulsion_importance;
		}
		else if ((*optionIter).first == "sf_query_radius")
		{
			value >> sf_query_radius;
		}
		else if ((*optionIter).first == "sf_body_force")
		{
			value >> sf_body_force;
		}
		else if ((*optionIter).first == "sf_agent_body_force")
		{
			value >> sf_body_force;
		}
		else if ((*optionIter).first == "sf_sliding_friction_force")
		{
			value >> sf_sliding_friction_force;
			// std::cout << "*************** set sf_sliding_friction_force to " << sf_sliding_friction_force << std::endl;
		}
		else if ((*optionIter).first == "sf_agent_b")
		{
			value >> sf_agent_b;
		}
		else if ((*optionIter).first == "sf_agent_a")
		{
			value >> sf_agent_a;
		}
		else if ((*optionIter).first == "sf_wall_b")
		{
			value >> sf_wall_b;
		}
		else if ((*optionIter).first == "sf_wall_a")
		{
			value >> sf_wall_a;
		}
		else if ((*optionIter).first == "sf_max_speed")
		{
			value >> sf_max_speed;
		}
		else if ((*optionIter).first == "ailogFileName")
		{
			logFilename = value.str();
			logStats = true;
		}
		else if ((*optionIter).first == "logAIStats")
		{
			logStats = true;
		}
		else if ((*optionIter).first == "stats")
		{
			gShowStats = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "allstats")
		{
			gShowAllStats = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "dont_plan")
		{
			dont_plan = Util::getBoolFromString(value.str());
		}
		else
		{
			// throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to PPR AI module.");
		}
	}

		_rvoLogger = LogManager::getInstance()->createLogger(logFilename,LoggerType::BASIC_WRITE);

		_rvoLogger->addDataField("number_of_times_executed",DataType::LongLong );
		_rvoLogger->addDataField("total_ticks_accumulated",DataType::LongLong );
		_rvoLogger->addDataField("shortest_execution",DataType::LongLong );
		_rvoLogger->addDataField("longest_execution",DataType::LongLong );
		_rvoLogger->addDataField("fastest_execution", DataType::Float);
		_rvoLogger->addDataField("slowest_execution", DataType::Float);
		_rvoLogger->addDataField("average_time_per_call", DataType::Float);
		_rvoLogger->addDataField("total_time_of_all_calls", DataType::Float);
		_rvoLogger->addDataField("tick_frequency", DataType::Float);

	if( logStats )
		{
		// LETS TRY TO WRITE THE LABELS OF EACH FIELD
		std::stringstream labelStream;
		unsigned int i;
		for (i=0; i < _rvoLogger->getNumberOfFields() - 1; i++)
			labelStream << _rvoLogger->getFieldName(i) << " ";
		labelStream << _rvoLogger->getFieldName(i);
		_data = labelStream.str() + "\n";

		_rvoLogger->writeData(labelStream.str());

	}
}

void SocialForcesAIModule::initializeSimulation()
{
	//
	// initialize the performance profilers
	//
	gPhaseProfilers = new PhaseProfilers;
	gPhaseProfilers->aiProfiler.reset();
	gPhaseProfilers->longTermPhaseProfiler.reset();
	gPhaseProfilers->midTermPhaseProfiler.reset();
	gPhaseProfilers->shortTermPhaseProfiler.reset();
	gPhaseProfilers->perceptivePhaseProfiler.reset();
	gPhaseProfilers->predictivePhaseProfiler.reset();
	gPhaseProfilers->reactivePhaseProfiler.reset();
	gPhaseProfilers->steeringPhaseProfiler.reset();

}

void SocialForcesAIModule::finish()
{
	// nothing to do here
}

void SocialForcesAIModule::preprocessSimulation()
{


}

void SocialForcesAIModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	if ( frameNumber == 1)
	{
		// Adding in this extra one because it seemed sometimes agents would forget about obstacles.

	}
	if ( !agents_.empty() )
	{

	}

	/*
	for (int i = 0; i < static_cast<int>(agents_.size()); ++i)
	{
		dynamic_cast<SocialForcesAgent *>(agents_[i])->computeNeighbors();
		dynamic_cast<SocialForcesAgent *>(agents_[i])->computeNewVelocity(dt);
	}*/
}

void SocialForcesAIModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	// do nothing for now
	int i = 0;
	i = i + i;
}
SteerLib::AgentInterface * SocialForcesAIModule::createAgent()
{
	SocialForcesAgent * agent = new SocialForcesAgent;
	agent->rvoModule = this;
	agent->id_ = agents_.size();
	agents_.push_back(agent);
	agent->_gEngine = this->_gEngine;
	return agent;
}

void SocialForcesAIModule::destroyAgent( SteerLib::AgentInterface * agent )
{
	/*
	 * This is going to cause issues soon.
	 */
	// agents_.erase(agents_.begin()+(agent)->id());
	int i;

	// Not as fast but seems to work properly
	// std::cout << "number of ORCA agents " << agents_.size() << std::endl;
	// SocialForcesAgent * rvoagent = dynamic_cast<SocialForcesAgent *>(agent);
	/*
	std::cout << "ORCA agent id " << (agent)->id() << std::endl;
	std::vector<SteerLib::AgentInterface * > tmpAgents;
	for (i = 0; i< agents_.size(); i++)
	{
		std::cout << " agent " << i << " " << agents_.at(i) << std::endl;
		if ( (agents_.at(i) != NULL) && (agents_.at(i)->id() != (agent)->id()) )
		{
			tmpAgents.push_back(agents_.at(i));
		}
	}
	agents_.clear();
	for (i = 0; i< tmpAgents.size(); i++)
	{
		agents_.push_back(tmpAgents.at(i));
	}*/


	// TODO this is going to be a memory leak for now.
	delete agent;
	/*
	if (agent && &agents_ && (agents_.size() > 1))
	{
		// std::cout << "agents.size(): " << agents_.size() << std::endl;
		agents_.erase(agents_.begin()+dynamic_cast<SocialForcesAgent *>(agent)->id());
		delete agent;
	}
	else if ( agent && &agents_ && (agents_.size() == 1))
	{
		// agents_.clear();
		delete agent;
	}*/


}

void SocialForcesAIModule::cleanupSimulation()
{
	agents_.clear();

		LogObject rvoLogObject;

		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getNumTimesExecuted());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTicksAccumulated());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getMinTicks());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getMaxTicks());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getMinExecutionTimeMills());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getMaxExecutionTimeMills());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getAverageExecutionTimeMills());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTime());
		rvoLogObject.addLogData(gPhaseProfilers->aiProfiler.getTickFrequency());

		_rvoLogger->writeLogObject(rvoLogObject);
		_data = _data + _rvoLogger->logObjectToString(rvoLogObject);
		_logData.push_back(rvoLogObject.copy());

		// cleanup profileing metrics for next simulation/scenario
		gPhaseProfilers->aiProfiler.reset();
		gPhaseProfilers->longTermPhaseProfiler.reset();
		gPhaseProfilers->midTermPhaseProfiler.reset();
		gPhaseProfilers->shortTermPhaseProfiler.reset();
		gPhaseProfilers->perceptivePhaseProfiler.reset();
		gPhaseProfilers->predictivePhaseProfiler.reset();
		gPhaseProfilers->reactivePhaseProfiler.reset();
		gPhaseProfilers->steeringPhaseProfiler.reset();
	if ( logStats )
	{
		_rvoLogger->writeLogObject(rvoLogObject);
	}

}
