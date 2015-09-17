//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * NavmeshModule.h
 *
 *  Created on: 2014-11-27
 *      Author: gberseth
 */

#ifndef KDTREEDATABASEMODULE_H_
#define KDTREEDATABASEMODULE_H_


#include "KdTreeDataBase.h"
#include "interfaces/SpatialDataBaseModuleInterface.h"
#include "SteerLib.h"
#include "SimulationPlugin.h"

// #include "Logger.h"

namespace KdTreeDataBaseGlobals
{
	struct PhaseProfilers {
		Util::PerformanceProfiler aiProfiler;
		Util::PerformanceProfiler drawProfiler;
		Util::PerformanceProfiler longTermPhaseProfiler;
		Util::PerformanceProfiler midTermPhaseProfiler;
		Util::PerformanceProfiler shortTermPhaseProfiler;
		Util::PerformanceProfiler perceptivePhaseProfiler;
		Util::PerformanceProfiler predictivePhaseProfiler;
		Util::PerformanceProfiler reactivePhaseProfiler;
		Util::PerformanceProfiler steeringPhaseProfiler;
	};


	extern SteerLib::EngineInterface * gEngine;

	extern PhaseProfilers * gPhaseProfilers;
}


class KdTreeDataBaseModule : public SteerLib::SpatialDataBaseModuleInterface
{
public:
	std::string getDependencies() { return ""; }

	std::string getConflicts() { return ""; }
	std::string getData() { return ""; }
	LogData * getLogData() { return new LogData(); }

	void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );


	void finish();

	void preprocessSimulation() ;

	void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber);

	void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber);

	void postprocessSimulation() ;


	void draw();

	SteerLib::KdTreeDataBase * getSpatialDataBase() { return _spatialDatabase; }
protected:
	SteerLib::EngineInterface * _engine;

	size_t _neighbour_size;

	std::string _meshFileName;
	SteerLib::KdTreeDataBase * _spatialDatabase;
};


#endif /* KDTREEDATABASEMODULE_H_ */
