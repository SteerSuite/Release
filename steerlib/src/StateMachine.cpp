//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "util/StateMachine.h"


using namespace std;
using namespace Util;

/// private helper macro to declare and initialize a state machine key
#define STATE_MACHINE_KEY(keyName, oldState, newState) \
	unsigned long long keyName = (oldState); \
	keyName = ((keyName) << 32); \
	keyName += (newState);


StateMachine::StateMachine()
{
	_started = false;
	_startState = 0xffffffff;
	_currentState = 0xffffffff;
	_callback = NULL;
}


void StateMachine::addTransition(unsigned int oldState, unsigned int newState)
{
	if (_started) {
		throw GenericException("Cannot call StateMachine::addTransition() after the state machine is started with start(); use stop() first.");
	}

	// declare and initialize the key
	STATE_MACHINE_KEY(key, oldState, newState);

	// add the key to the list of known transitions.
	_transitions.insert(key);
}

void StateMachine::setCallback( StateMachineCallbackInterface * callback )
{
	if (_started) {
		throw GenericException("Cannot call StateMachine::setCallback() after the state machine is started with start(); use stop() first.");
	}

	_callback = callback;
}

void StateMachine::setStartState( unsigned int newState )
{
	if (_started) {
		throw GenericException("Cannot call StateMachine::setStartState() after the state machine is started with start(); use stop() first.");
	}

	_startState = newState;
}


void StateMachine::start()
{
	if (_started) {
		throw GenericException("Cannot call StateMachine::start() because it was already called once; use stop() first.");
	}

	_currentState = _startState;
	_started = true;
}

void StateMachine::stop()
{
	if (!_started) {
		throw GenericException("Cannot call StateMachine::stop() because the state machine was not started; use start() first.");
	}

	_started = false;
}

bool StateMachine::canTransitionToState(unsigned int newState)
{
	if (!_started) {
		throw GenericException("Cannot call StateMachine::canTransitionToState() because the state machine was not started; use start() first.");
	}

	// declare and initialize the key
	STATE_MACHINE_KEY(key, _currentState, newState);

	if (_transitions.find(key) != _transitions.end()) {
		return true;
	}
	else {
		return false;
	}

}

void StateMachine::transitionToState(unsigned int newState)
{
	if (!_started) {
		throw GenericException("Cannot call StateMachine::transitionToState() because the state machine was not started; use start() first.");
	}

	// declare and initialize the key
	STATE_MACHINE_KEY(key, _currentState, newState);

	if (_transitions.find(key) != _transitions.end()) {
		// transition is allowed, so make the transition.
		if (_callback != NULL) _callback->transitioned(_currentState, newState);
		_currentState = newState;
	}
	else {
		// transition is unknown.
		if (_callback != NULL) {
			_currentState = _callback->handleUnknownTransition(_currentState, newState);
		}
		else {
			throw GenericException("StateMachine::transitionToState(): unknown transition requested from state " + toString(_currentState) + " to " + toString(newState) + ".\nIf you would like to handle this error in your own code,\nimplement the StateMachineCallbackInterface::handleUnknownTransition() function.");
		}
	}
}

