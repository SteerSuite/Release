//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SimpleAIModule.cpp
/// @brief Implements the SimpleAIModule plugin.


#include "SteerLib.h"
#include "SimulationPlugin.h"
#include "SimpleAIModule.h"
#include "SimpleAgent.h"

#include "LogObject.h"
#include "LogManager.h"


// globally accessible to the simpleAI plugin
SteerLib::EngineInterface * gEngine;
SteerLib::GridDatabase2D * gSpatialDatabase;

namespace SimpleAIGlobals
{
	unsigned int gLongTermPlanningPhaseInterval;
	unsigned int gMidTermPlanningPhaseInterval;
	unsigned int gShortTermPlanningPhaseInterval;
	unsigned int gPredictivePhaseInterval;
	unsigned int gReactivePhaseInterval;
	unsigned int gPerceptivePhaseInterval;
	bool gUseDynamicPhaseScheduling;
	bool gShowStats;
	bool gShowAllStats;

	PhaseProfilers * gPhaseProfilers;
}

using namespace SimpleAIGlobals;

PLUGIN_API SteerLib::ModuleInterface * createModule()
{
	return new SimpleAIModule;
}

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module )
{
	delete module;
}


void SimpleAIModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	gEngine = engineInfo;
	gSpatialDatabase = engineInfo->getSpatialDatabase();

	gUseDynamicPhaseScheduling = false;
	gShowStats = false;
	logStats = false;
	gShowAllStats = false;
	logFilename = "simpleAI.log";

	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		std::stringstream value((*optionIter).second);
		if ((*optionIter).first == "")
		{
			value >> gLongTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "ailogFileName")
		{
			logFilename = value.str();
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
		else
		{
			// throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to PPR AI module.");
		}
	}

	if( logStats )
	{

		_logger = LogManager::getInstance()->createLogger(logFilename,LoggerType::BASIC_WRITE);

		_logger->addDataField("number_of_times_executed",DataType::LongLong );
		_logger->addDataField("total_ticks_accumulated",DataType::LongLong );
		_logger->addDataField("shortest_execution",DataType::LongLong );
		_logger->addDataField("longest_execution",DataType::LongLong );
		_logger->addDataField("fastest_execution", DataType::Float);
		_logger->addDataField("slowest_execution", DataType::Float);
		_logger->addDataField("average_time_per_call", DataType::Float);
		_logger->addDataField("total_time_of_all_calls", DataType::Float);
		_logger->addDataField("tick_frequency", DataType::Float);

		// LETS TRY TO WRITE THE LABELS OF EACH FIELD
		std::stringstream labelStream;
		unsigned int i;
		for (i=0; i < _logger->getNumberOfFields() - 1; i++)
			labelStream << _logger->getFieldName(i) << " ";
		labelStream << _logger->getFieldName(i);

		_logger->writeData(labelStream.str());

	}
}

void SimpleAIModule::initializeSimulation()
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

void SimpleAIModule::preprocessSimulation()
{
	//TODO does nothing for now
}

void SimpleAIModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	//TODO does nothing for now
}

void SimpleAIModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	//TODO does nothing for now
}

void SimpleAIModule::cleanupSimulation()
{

	if ( logStats )
	{
		LogObject logObject;

		logObject.addLogData(gPhaseProfilers->aiProfiler.getNumTimesExecuted());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTicksAccumulated());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMinTicks());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMaxTicks());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMinExecutionTimeMills());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMaxExecutionTimeMills());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getAverageExecutionTimeMills());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTime());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getTickFrequency());

		_logger->writeLogObject(logObject);

		// cleanup profileing metrics for next simulation/scenario
		gPhaseProfilers->aiProfiler.reset();
		gPhaseProfilers->longTermPhaseProfiler.reset();
		gPhaseProfilers->midTermPhaseProfiler.reset();
		gPhaseProfilers->shortTermPhaseProfiler.reset();
		gPhaseProfilers->perceptivePhaseProfiler.reset();
		gPhaseProfilers->predictivePhaseProfiler.reset();
		gPhaseProfilers->reactivePhaseProfiler.reset();
		gPhaseProfilers->steeringPhaseProfiler.reset();
	}

	// kdTree_->deleteObstacleTree(kdTree_->obstacleTree_);
}

void SimpleAIModule::finish()
{
	// nothing to do here
}

SteerLib::AgentInterface * SimpleAIModule::createAgent()
{
	return new SimpleAgent; 
}

void SimpleAIModule::destroyAgent( SteerLib::AgentInterface * agent )
{
	delete agent;
}
