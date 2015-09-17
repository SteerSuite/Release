//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * PlanningDomainModuleInterface.h
 *
 *  Created on: 2015-02-05
 *      Author: gberseth
 */

#ifndef PLANNINGDOMAINMODULEINTERFACE_H_
#define PLANNINGDOMAINMODULEINTERFACE_H_

#include "interfaces/ModuleInterface.h"
#include "interfaces/SpatialDataBaseInterface.h"

namespace SteerLib{

	class PlanningDomainModuleInterface : public ModuleInterface
	{
	public:
		virtual PlanningDomainInterface * getPathPlanner() =0;
	};
}

#endif /* PLANNINGDOMAINMODULEINTERFACE_H_ */
