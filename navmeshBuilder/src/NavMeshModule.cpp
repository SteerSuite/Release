//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * NavMeshModule.cpp
 *
 *  Created on: 2014-11-29
 *      Author: glenpb
 *
 * ./bin/navmeshBuilder -ai modules/sfAI -testcase ../testcases/4-way-oncomming.xml -module navmesh -config config-navmesh.xml
 */

#include "SteerLib.h"
#include "NavMeshModule.h"
#include "MeshLoaderObj.h"
#include "Timer.h"
#include "opengl.h"
#include "stdio.h"
#include "InputGeom.h"
#include "Sample.h"
#include "Sample_SoloMesh.h"
#include "imguiRenderGL.h"
#include "imgui.h"


namespace NavMeshGlobals
{
	SteerLib::EngineInterface * gEngine;
	SteerLib::SpatialDataBaseInterface * gSpatialDatabase;

	PhaseProfilers * gPhaseProfilers;
}


using namespace SteerLib;
using namespace NavMeshGlobals;

void NavMeshModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{

	_engine = engineInfo;

	// iterate over all the options
	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		if ((*optionIter).first == "technique") {
			// _techniqueName = (*optionIter).second;
		}
		else if ((*optionIter).first == "saveGeometry") {
			_meshFileName = (*optionIter).second;
		}
		else {
			throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to navmesh module.");
		}
	}

	std::cout << "Number of obstacles in engine: " << _engine->getObstacles().size() << std::endl;
	// gEngine = _engine;
	this->_pathPlanner = new RecastNavMeshPlanner(engineInfo);

	gSpatialDatabase = engineInfo->getSpatialDatabase();
	// _sample = createSolo();
	// std::cout << "" << _engine->getStaticGeometry().first.at(0);
	// std::cout << "Got some geometry" << engineInfo << std::endl;

	/*mesh->add_vertex(0,0,0);
	mesh->add_vertex(12,0,0);
	mesh->add_vertex(0,12,0);
	mesh->add_face(0,1,2);
	*/
	// this->_spatialDatabase->init(options, _engine);
	// _benchmarkTechnique->setEngineInterface(_engine);
}


void NavMeshModule::initializeSimulation()
{
	std::cout << "initialize Number of obstacles in engine: " << _engine->getObstacles().size() << std::endl;
}

void NavMeshModule::preprocessSimulation()
{
	if (this->_meshFileName != "")
	{
		this->saveStaticGeometryToObj(this->_meshFileName);
	}
	// This needs to be here because obstacles are not put in _engine until after init();

	this->getPathPlanner()->refresh();

}

void NavMeshModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{

}

void NavMeshModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber) {
	// std::cout << "found the business? " << _engine->getAgents().size() << std::endl;
	// _benchmarkTechnique->update(_metricsCollectorModule->getSimulationMetrics(), timeStamp, dt);
	// _benchmarkTechnique->update( _engine, timeStamp, dt);
}


void NavMeshModule::postprocessSimulation() {
}


void NavMeshModule::draw()
{
#ifdef ENABLE_GUI
	_pathPlanner->draw();
	// std::cout << "drawing navmesh Module" << std::endl;

	// std::cout << "Path " << _navTool->getPath().at(0) << std::endl;
	/*
	std::vector<Util::Point> points;
	for (size_t p=0; p < _navTool->getPath().size(); p +=3)
	{
		points.push_back( Util::Point( _navTool->getPath().at(p), _navTool->getPath().at(p+1), _navTool->getPath().at(p+2) ) + Util::Point(0.0,0.6,0.0) );
	}

	DrawLib::drawStar(points.at(0), Util::Vector(1,0,0), 0.34f, gBlue);
	for (size_t p=1; p < points.size(); p++)
	{
		DrawLib::drawLine(points.at(p), points.at(p-1), gBlack, 5.0f);
		DrawLib::drawStar(points.at(p), Util::Vector(1,0,0), 0.34f, gOrange);
		// std::cout << "point " << p << ", " << points.at(p) << std::endl;
	}
*/
#endif

}

void NavMeshModule::finish()
{


}

void NavMeshModule::saveStaticGeometryToObj(std::string fileName)
{
	std::pair<std::vector<Util::Point>,std::vector<size_t> > mesh_stuff = _engine->getStaticGeometry();
	std::fstream fileStream;
	fileStream.open(fileName.c_str(),std::ios::out);

	fileStream << "# SteerSuite Geometry" << std::endl;
	fileStream << "o Scenario Geometry" << std::endl;

	fileStream << std::endl;


	for (size_t point=0; point < mesh_stuff.first.size(); point++)
	{
		Util::Point p = mesh_stuff.first.at(point);
		fileStream << "v " <<p.x << " " << p.y << " " << p.z <<std::endl;
	}

	fileStream << std::endl;

	for (size_t face=0; face < mesh_stuff.second.size(); face+=3)
	{ // wavefront vert indicies start at 1
		fileStream << "f " <<mesh_stuff.second.at(face)+1 << " " <<
				mesh_stuff.second.at(face+1)+1 << " " <<
				mesh_stuff.second.at(face+2)+1 << std::endl;
	}

	fileStream.close();
}

// External exported functions
PLUGIN_API SteerLib::ModuleInterface * createModule()
{
	return new NavMeshModule();
}

PLUGIN_API PlanningDomainInterface * getPathPlanner(SteerLib::PlanningDomainModuleInterface*  module)
{
	return module->getPathPlanner();
}

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module )
{
	if (module) delete module; module = NULL;
}




