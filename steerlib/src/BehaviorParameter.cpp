//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * BehaviorPrivate.cpp
 *
 *  Created on: 2013-12-05
 *      Author: glenpb
 */

#include "testcaseio/BehaviourParameter.h"

using namespace SteerLib;

BehaviourParameter::BehaviourParameter() 
{
	// TODO Auto-generated constructor stub

}

BehaviourParameter::BehaviourParameter(std::string key, std::string value) : key(key), value(value)
{
	// ??
}

BehaviourParameter::~BehaviourParameter()
{
	// TODO
	this->key.clear();
}
