//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_STATE_MACHINE_H__
#define __UTIL_STATE_MACHINE_H__

/// @file StateMachine.h
/// @brief Declares StateMachine, a basic finite state machine.

#include <set>

#include "Globals.h"
#include "util/GenericException.h"
#include "util/Misc.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace Util {

	/**
	 * @brief The virtual interface for optional callbacks used by the StateMachine.
	 *
	 * @see
	 *   - SteerLib::StateMachine
	 */
	class UTIL_API StateMachineCallbackInterface
	{
	public:
		virtual ~StateMachineCallbackInterface() { }
		/// Implement this function to handle a known transition after it takes place.
		virtual void transitioned(unsigned int previousState, unsigned int currentState) { }
		/// Implement this function to handle unknown transitions; return the state you want to place the state machine after this function returns
		virtual unsigned int handleUnknownTransition(unsigned int currentState, unsigned int requestedNewState) {
			throw GenericException("StateMachine::transitionToState(): unknown transition requested from state " + Util::toString(currentState) + " to " + Util::toString(requestedNewState) + ".\nIf you would like to handle this error in your own code,\nimplement the StateMachineCallbackInterface::handleUnknownTransition() function.");
		}
	};



	/**
	 * @brief A class that provides basic state machine functionality.
	 *
	 * This class helps a user track states and transitions.  Integers are used to represent states;
	 * it is recommended you define your own enum data type to use instead of plain integers.
	 *
	 * To use this class, simply add the transitions you want to allow using addTransition().
	 * If setStartState() is not used to specify the starting state, then 0xffffffff will be used.
	 *
	 * Optionally, you can inherit and implement a StateMachineCallbackInterface, which allows your code
	 * to be called when any transitions occur, whether they are valid or unknown.  For example, it may be useful
	 * to handle erroneous transitions in StateMachineCallbackInterface::handleUnknownTransition().  Specify this
	 * callback during setup using the setCallback() function.
	 *
	 * After setting up, start the state machine using start(), which initializes the state machine
	 * to the start state, and throws an exception if any initialization functions are called.  start() itself
	 * cannot be called again until stop() was called. Once started, use transitionToState() to transition to a new state.
	 * This class will automatically check if the transition from currentState to newState is valid,
	 * and throws an exception if it was not valid.
	 *
	 * <h3> Notes </h3>
	 *  - All "valid" transitions must be defined by addTransition().
	 *    Invalid or unknown transitions will cause errors even if they were not specified by addErrorTransition().
	 *
	 *  - Internally, valid transitions are stored using a sparse 1-D lookup table (e.g. a std::set).
	 *    The implementation is not optimized for speed, but its performance should be decent and should scale
	 *    gracefully to a moderately large number of states and transitions.
	 *
	 *  - Transitions from a state to itself are treated just like any other pair of states.  In particular, you
	 *    must still define the transition normally or else it will cause an error.
	 *
	 * @todo
	 *  - make transitions effectively atomic by making it thread-safe.
	 *
	 */
	class UTIL_API StateMachine {

	public:
		/// @name Setting up the state machine
		//@{
		StateMachine();
		void addTransition(unsigned int oldState, unsigned int newState);
		void setCallback( StateMachineCallbackInterface * callback );
		void setStartState( unsigned int newState );
		//@}

		/// @name Using the state machine
		//@{
		void start();
		void stop();
		inline unsigned int getCurrentState() { return _currentState; }
		bool canTransitionToState(unsigned int newState);
		void transitionToState(unsigned int newState);
		//@}

	protected:
		bool _started;
		unsigned int _currentState;
		StateMachineCallbackInterface * _callback;
		unsigned int _startState;

		std::set<unsigned long long> _transitions;
	};


} // namsepace Util

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif

