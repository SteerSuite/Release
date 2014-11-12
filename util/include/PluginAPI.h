//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * PluginAPI.h
 *
 *  Created on: 2013-04-30
 *      Author: glenpb
 */

#ifndef PLUGINAPI_H_
#define PLUGINAPI_H_


#ifdef _WIN32
#define PLUGIN_ extern "C"  __declspec(dllexport)
#else
#define PLUGIN_ extern "C"
#endif

#endif /* PLUGINAPI_H_ */
