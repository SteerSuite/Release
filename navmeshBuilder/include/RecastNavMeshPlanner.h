//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * RecastNavMeshPlanner.h
 *
 *  Created on: 2015-02-05
 *      Author: gberseth
 */

#ifndef RECASTNAVMESHPLANNER_H_
#define RECASTNAVMESHPLANNER_H_

#include "interfaces/PlanningDomainInterface.h"
#include "NavMeshTesterTool.h"
#include "SteerLib.h"
#include "NavMeshTesterTool.h"
#include "InputGeom.h"
#include "Sample.h"
#include "Sample_SoloMesh.h"

#include "Mesh.h"

using namespace SteerLib;

class RecastNavMeshPlanner : public PlanningDomainInterface
{
public:
	RecastNavMeshPlanner( SteerLib::EngineInterface * engineInfo );
	virtual ~RecastNavMeshPlanner();

	/// @name Path planning queries
	//@{
	/// Returns "true" if a path was found from startLocation to goalLocation, or "false" if no complete path was found; in either case, the path (complete if returning true, or partial path if returning false) is stored in outputPlan as a sequence of grid cell indices.

	virtual bool findPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
			unsigned int _maxNodesToExpandForSearch);

	virtual bool findSmoothPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
			unsigned int _maxNodesToExpandForSearch);

	/// update navmesh
	virtual bool refresh();
	//@}

	// If there is anything to draw
	virtual void draw();

	virtual std::pair<std::vector<Util::Point> , std::vector<size_t>> getNavMeshGeometry();
	virtual std::pair<std::vector<Util::Point> , std::vector<size_t>> getEnvironemntGeometry();

	SteerLib::EngineInterface * _engine;

private:
	BuildContext ctx;
	Sample* _sample;
	NavMeshTesterTool * _navTool;
//	Mesh * _mesh; // Only used for drawing the navmesh
};

#endif /* RECASTNAVMESHPLANNER_H_ */
