//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * KdTreeDataBaseModule.cpp
 *
 *  Created on: 2014-11-29
 *      Author: glenpb
 *
 * ./bin/navmeshBuilder -ai modules/sfAI -testcase ../testcases/4-way-oncomming.xml -module navmesh -config config-navmesh.xml
 */

#include "SteerLib.h"
#include "KdTreeDataBaseModule.h"


namespace KdTreeDataBaseGlobals
{
	SteerLib::EngineInterface * gEngine;

	PhaseProfilers * gPhaseProfilers;
}


using namespace SteerLib;
using namespace KdTreeDataBaseGlobals;


void KdTreeDataBaseModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{

	_engine = engineInfo;


	// iterate over all the options
	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		if ((*optionIter).first == "kdtree_size") {
			_neighbour_size = atoi((*optionIter).second.c_str());
		}
		else if ((*optionIter).first == "saveGeometry") {
			_meshFileName = (*optionIter).second;
		}
		else {
			throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to navmesh module.");
		}
	}

	// gEngine = _engine;
	float xmin = -(_engine->getOptions().gridDatabaseOptions.gridSizeX / 2.0f);
	float xmax = (_engine->getOptions().gridDatabaseOptions.gridSizeX / 2.0f);
	float zmin = -(_engine->getOptions().gridDatabaseOptions.gridSizeZ / 2.0f);
	float zmax = (_engine->getOptions().gridDatabaseOptions.gridSizeZ / 2.0f);
	this->_spatialDatabase = new KdTreeDataBase(_engine, xmin, xmax, zmin, zmax);

	// this->_spatialDatabase->buildAgentTree();
	std::cout << "KdTreeDataBaseModule inited: " << this->_spatialDatabase << std::endl;

}


void KdTreeDataBaseModule::preprocessSimulation()
{
	this->_spatialDatabase->buildObstacleTree();
}

void KdTreeDataBaseModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	if ( frameNumber == 1)
	{
		// Adding in this extra one because it seemed sometimes agents would forget about obstacles.
		this->_spatialDatabase->buildObstacleTree();
	}

	this->_spatialDatabase->buildAgentTree();
}

void KdTreeDataBaseModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{


}



void KdTreeDataBaseModule::postprocessSimulation()
{

}


void KdTreeDataBaseModule::draw()
{
	_spatialDatabase->draw();
	// std::cout << "drawing KdTreeDataBase Module" << std::endl;

}

void KdTreeDataBaseModule::finish()
{


}


// External exported functions
PLUGIN_API SteerLib::ModuleInterface * createModule()
{
	return new KdTreeDataBaseModule();
}

PLUGIN_API SpatialDataBaseInterface * getSpatialDataBase(SteerLib::SpatialDataBaseModuleInterface*  module)
{
	return module->getSpatialDataBase();
}

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module )
{
	if (module) delete module; module = NULL;
}




