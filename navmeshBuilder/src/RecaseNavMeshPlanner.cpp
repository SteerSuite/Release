//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * RecaseNavMeshPlanner.cpp
 *
 *  Created on: 2015-02-05
 *      Author: gberseth
 */

#include "RecastNavMeshPlanner.h"

// Sample* createSolo() { return new Sample_SoloMesh(); }

RecastNavMeshPlanner::RecastNavMeshPlanner( SteerLib::EngineInterface * engineInfo )
{
	// TODO Auto-generated constructor stub
	this->_navTool = new NavMeshTesterTool();
	_engine = engineInfo;
	_sample = new Sample_SoloMesh();
}

RecastNavMeshPlanner::~RecastNavMeshPlanner() {
	// TODO Auto-generated destructor stub
}


bool RecastNavMeshPlanner::findPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
		unsigned int _maxNodesToExpandForSearch)
{
	_navTool->setToolMode(_navTool->TOOLMODE_PATHFIND_FOLLOW);
	// Util::Point pos = _engine->getAgents().at(0)->position();
	float agentpos[] =  {startPosition.x, startPosition.y, startPosition.z};
	// Util::Point goal = _engine->getAgents().at(0)->currentGoal().targetLocation;
	float agentTargetPos[] = {endPosition.x, endPosition.y, endPosition.z};
	// std::cout << "Planning a path from " << startPosition << " to " << endPosition << std::endl;
	_navTool->addAgent(agentpos, agentTargetPos, 0.4, 7.0, 0.5, 7);

	for (size_t p=0; p < _navTool->getFollowPath().size(); p +=3)
	{
		// path.push_back( Util::Point( _navTool->getFollowPath().at(p), _navTool->getFollowPath().at(p+1), _navTool->getFollowPath().at(p+2) ));
		path.push_back( Util::Point( _navTool->getFollowPath().at(p), 0.0f, _navTool->getFollowPath().at(p+2) ));
	}
	// std::cout << "Planning path length is " << path.size() << std::endl;
	if ( path.size() > 0)
	{
	return true;
	}
	else
	{
		return false;
	}
}

bool RecastNavMeshPlanner::findSmoothPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
		unsigned int _maxNodesToExpandForSearch)
{
	// Util::Point pos = _engine->getAgents().at(0)->position();
	_navTool->setToolMode(_navTool->TOOLMODE_PATHFIND_STRAIGHT);
	float agentpos[] =  {startPosition.x, startPosition.y, startPosition.z};
	// Util::Point goal = _engine->getAgents().at(0)->currentGoal().targetLocation;
	float agentTargetPos[] = {endPosition.x, endPosition.y, endPosition.z};
	// std::cout << "Planning a path from " << startPosition << " to " << endPosition << std::endl;
	_navTool->addAgent(agentpos, agentTargetPos, 0.4, 7.0, 0.5, 7);

	for (size_t p=0; p < _navTool->getPath().size(); p +=3)
	{
		// path.push_back( Util::Point( _navTool->getPath().at(p), _navTool->getPath().at(p+1), _navTool->getPath().at(p+2) ));
		path.push_back( Util::Point( _navTool->getFollowPath().at(p), 0.0f, _navTool->getFollowPath().at(p+2) ));
	}
	// std::cout << "Planning smooth path length is " << path.size() << std::endl;

	if ( path.size() > 0)
	{
	return true;
	}
	else
	{
		return false;
	}
}

bool RecastNavMeshPlanner::refresh()
{
	/*
	if ( this->_navTool )
	{
		delete this->_navTool;
	}
	this->_navTool = new NavMeshTesterTool();
	if ( this->_sample )
	{
		delete this->_sample;
	}

	_sample = new Sample_SoloMesh();
	*/
	this->_navTool->reset();
	// this->_sample->reset();
	// std::cout << "this is the planner:" << this << std::endl;
	// This needs to be here because obstacles are not put in _engine until after init();
	// Mesh * mesh = new Mesh();
	// mesh->init(xyzPositions,triVerts);
	// std::pair<std::vector<Util::Point>,std::vector<size_t> > mesh_stuff = box.getGeometry();
	// std::cout << "About to get some more geometry" << _engine << std::endl;
	// this call does not want to work... It has given me problems before... engin doesn't like being passed between many functions'
	std::pair<std::vector<Util::Point>,std::vector<size_t> > mesh_stuff = _engine->getStaticGeometry();
	// mesh->init(mesh_stuff.first,mesh_stuff.second);

	InputGeom* geom = new InputGeom();
	geom->loadMesh(&ctx, mesh_stuff.first, mesh_stuff.second);
	// geom->loadMesh(&ctx, meshPath);
	_sample->setContext(&ctx);
	ctx.resetLog();
	_sample->handleMeshChanged(geom);
	_sample->handleSettings();
	_sample->handleBuild();
	ctx.dumpLog("Dumping Log\n");

	// _sample->getNavMesh();
	// std::pair<std::vector<Util::Point>,std::vector<size_t> > navmesh_stuff = _sample->getNavMeshGeometry();
	// mesh->init(navmesh_stuff.first,navmesh_stuff.second);
	// mesh->


	_navTool->init(_sample);
	std::cout << "NavMesh number of agents: " << _engine->getAgents().size() << std::endl;


	Mesh * mesh = new Mesh();
	std::pair<std::vector<Util::Point>,std::vector<size_t> > navmesh_stuff = this->getNavMeshGeometry();

	/*
	mesh->init(navmesh_stuff.first,navmesh_stuff.second);
	if ( this->_mesh )
	{
		delete this->_mesh;
	}
	this->_mesh = mesh;
	std::cout << "Done adding a drawmesh "<< std::endl;
	*/
	return true;
}

std::pair<std::vector<Util::Point> , std::vector<size_t>> RecastNavMeshPlanner::getNavMeshGeometry()
{
	return _sample->getNavMeshGeometry();
}
std::pair<std::vector<Util::Point> , std::vector<size_t>> RecastNavMeshPlanner::getEnvironemntGeometry()
{
	return _sample->getEnvironemntGeometry();
}

void RecastNavMeshPlanner::draw()
{
#ifdef ENABLE_GUI

	/*
	Mesh * mesh = new Mesh();
	std::pair<std::vector<Util::Point>,std::vector<size_t> > navmesh_stuff = this->getNavMeshGeometry();

	mesh->init(navmesh_stuff.first,navmesh_stuff.second);
	*/
	// this->_mesh->drawMesh();

#endif
}

