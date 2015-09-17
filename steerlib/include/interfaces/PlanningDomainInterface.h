//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * PathPlanningInterface.h
 *
 *  Created on: 2015-02-05
 *      Author: gberseth
 */

#ifndef PLANNINGDOMAININTERFACE_H_
#define PLANNINGDOMAININTERFACE_H_

#include "util/Geometry.h"
#include <vector>
#include <stack>

namespace SteerLib{

	class STEERLIB_API PlanningDomainInterface
	{
	public:
		virtual ~PlanningDomainInterface() {};

		/// @name Path planning queries
		//@{
		/// Returns "true" if a path was found from startLocation to goalLocation, or "false" if no complete path was found; in either case, the path (complete if returning true, or partial path if returning false) is stored in outputPlan as a sequence of grid cell indices.
		virtual bool findPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
				unsigned int _maxNodesToExpandForSearch) = 0;

		virtual bool findSmoothPath (Util::Point &startPosition, Util::Point &endPosition, std::vector<Util::Point> & path,
				unsigned int _maxNodesToExpandForSearch) = 0;
		/// Used to recompute items when the environment changes.
		virtual bool refresh() = 0;
		// If there is anything to draw
		virtual void draw() = 0;

		//@}

	};
}


#endif /* PLANNINGDOMAININTERFACE_H_ */
