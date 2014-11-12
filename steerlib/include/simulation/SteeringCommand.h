//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_STEERING_COMMAND_H__
#define __STEERLIB_STEERING_COMMAND_H__

/// @file SteeringCommand.h
/// @brief Declares the SteeringCommand class

#include <vector>
#include "util/Geometry.h"
#include "Globals.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {


	/**
	 * @brief Helper class for representing steering decisions.
	 *
	 * This data structure is an abstraction of a steering decision.  It can be 
	 * used as output of a steering AI algorithm.  The intention is that the user
	 * will freely modify and access the data members of this class, so everything
	 * is declared public.
	 *
	 * <b> executeCommandForParticle() is not implemented yet.</b>  For now, you can
	 * assimilate the code from PPRAgent::doSteering().
	 * The executeCommandForParticle() function receives information about an agent
	 * represented by an oriented point-mass, and returns the new position and 
	 * orientation of the agent following that command.
	 *
	 * If the agent is more complicated than a point-mass particle, this steering decision 
	 * can still be used, for example as input into an animation framework, i.e., motion
	 * synthesis.  (%SteerSuite does not currently have any motion synthesis framework, but it 
	 * may be considered in the long term).
	 * 
	 * The data provided will be used in different ways depending on the value of steeringMode:
	 *  - If steeringMode is LOCOMOTION_MODE_COMMAND, then the user must define speed and direction
	 *    commands.  There are three parts to this command: speed, direction, and scoot.
	 *     - aimForTargetSpeed is a boolean flag.  If it is true, then targetSpeed will be the speed that the
	 *       agent should try to maintain.  Optionally, the acceleration value may hint how fast it should try to
	 *       accelerate to maintain that speed.  If aimForTargetSpeed is false, then targetSpeed is ignored, and
	 *       acceleration will be a value (in meters per second) that describes the acceleration of the agent.
	 *     - turning uses the same specification method as speed, using the aimForTargetDirection, targetDirection,
	 *       and turningAmount values.
	 *     - scoot describes an additional amount of side-to-side motion.  If used properly, it allows a character to
	 *       side-step when appropriate.
	 *  - If steeringMode is LOCOMOTION_MODE_DYNAMICS, then the user must provide a force vector in 
	 *    dynamicsSteeringForce.
	 *  - If steeringMode is LOCOMOTION_MODE_SPACETIMEPATH, then the user must provide the
	 *    exact path and timing information.
	 *
	 *
	 * @todo 
	 *  - LOCOMOTION_MODE_SPACETIMEPATH is not fully implemented yet, and should be ignored for now.
	 *
	 */
	class STEERLIB_API SteeringCommand {
	public:

		/// See documentation of SteeringCommand class to understand the different steering modes.
		enum LocomotionType { LOCOMOTION_MODE_COMMAND, LOCOMOTION_MODE_DYNAMICS, LOCOMOTION_MODE_SPACETIMEPATH };

		void clear();

		LocomotionType steeringMode;

		//
		// command steering mode:
		//
		bool aimForTargetDirection;
		Util::Vector targetDirection;
		float turningAmount;
		bool aimForTargetSpeed;
		float targetSpeed;
		float acceleration;
		float scoot;

		// dynamics steering mode:
		Util::Vector dynamicsSteeringForce;

		// space-time path following mode: 
		// not implemented yet!!
	};



} // namespace SteerLib


#ifdef _WIN32
#pragma warning( pop )
#endif

#endif

