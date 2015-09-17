//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef BEHAVIOUR_PARAMETER_H_
#define BEHAVIOUR_PARAMETER_H_

#include "Globals.h"

#ifdef _WIN32
// see steerlib/util/DrawLib.h for explanation
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <string>
#include <vector>

namespace SteerLib 
{
	class STEERLIB_API BehaviourParameter
	{
	public:
		BehaviourParameter();
		BehaviourParameter(std::string key, std::string value);
		virtual ~BehaviourParameter();

		std::string key;
		//TODO for now this is float but sometime these are only INTEGERS
		std::string value;

	};
}

#endif 