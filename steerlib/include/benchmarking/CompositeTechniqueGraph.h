//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * CompositeTechniqueGraph.h
 *
 *  Created on: 2013-03-28
 *      Author: glenpb
 */

#include "CompositeTechnique02.h"

#ifndef COMPOSITETECHNIQUEGRAPH_H_
#define COMPOSITETECHNIQUEGRAPH_H_

namespace SteerLib
{

	class STEERLIB_API CompositeBenchmarkTechniqueGraph : public SteerLib::CompositeBenchmarkTechnique02
	{

	public:
		void getScores ( std::vector<float> & scores )
		{
			scores.push_back(_numCollisionsOfAllAgents);
			scores.push_back(_totalTimeOfAllAgents);
			scores.push_back(_totalEnergyOfAllAgents);
			scores.push_back(_totalInstantaneousAcceleration);
			// TODO scores.push_back(_to\);
		}

	};

}



#endif /* COMPOSITETECHNIQUEGRAPH_H_ */
