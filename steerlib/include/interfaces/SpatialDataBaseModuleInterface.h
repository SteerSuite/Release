//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * SpatialDataBaseModuleInterface.h
 *
 *  Created on: 2014-11-29
 *      Author: glenpb
 */

#ifndef SPATIALDATABASEMODULEINTERFACE_H_
#define SPATIALDATABASEMODULEINTERFACE_H_

#include "interfaces/ModuleInterface.h"
#include "interfaces/SpatialDataBaseInterface.h"


namespace SteerLib{

	class SpatialDataBaseModuleInterface : public ModuleInterface
	{
	public:
		virtual SpatialDataBaseInterface * getSpatialDataBase() =0;
	};
}


#endif /* SPATIALDATABASEMODULEINTERFACE_H_ */
