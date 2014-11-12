//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
//
// Copyright (c) 2009 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERSIM_PLUGIN_H__
#define __STEERSIM_PLUGIN_H__

/// @file SteerSimPlugin.h
/// @brief Defines the functionality needed to develop an external module plugin.
///
/// To create a module:
///  -# Create a separate project that is intended to compile as a dynamic library.
///  -# Inherit the SteerLib::ModuleInterface abstract class and implement its functionality.  Most of the functionality is 
///     optional, so the default empty implementations will be used if you do not implement them.
///  -# Implement createModule() and destroyModule() declared in SteerSimPlugin.h.  These are used by the engine to
///     create/destroy an instance of your ModuleInterface.
///  -# Compile your dynamic library, and use the "-module <moduleName>" option when running steersim.
///  -# Optionally, you can also make a separate class that inherits SteerLib::AgentInterface to implement your own agents, and 
///     implement SteerLib::ModuleInterface::createAgent() and SteerLib::ModuleInterface::destroyAgent() to allocate and destory
///     these agents.
///  -# Enjoy!
///

#include "interfaces/ModuleInterface.h"


/// @def PLUGIN_API
/// @brief Describes a symbol that should be made available for run-time dynamic linking (i.e., for plugins).
#ifdef _WIN32
#define PLUGIN_API  extern "C" __declspec(dllexport)
#else
#define PLUGIN_API  extern "C"
#endif


/**
 * @brief Allocates a module, and returns a pointer to it.
 *
 * When loading a plugin, the core engine will call #createModule() to get an
 * instance of your class that implements the ModuleInterface. The engine will use that 
 * ModuleInterface object as needed, and then to unload the plugin, the engine will 
 * call #destroyModule().
 *
 * #createModule() should allocate and return a reference to your own class that inherits and
 * implements the Module interface, and #destroyModule() should receive that same reference
 * and de-allocate it.
 *
 * @see
 *  - destroyModule()
 *  - Refer to the User Guide for how to create a SteerSuite plugin.
 */
PLUGIN_API SteerLib::ModuleInterface * createModule();

/**
 * @brief De-allocates a module that was allocated by the same dynamic library.
 *
 * When loading a plugin, the core engine will call #createModule() to get an
 * instance of your class that implements the ModuleInterface. The engine will use that 
 * ModuleInterface object as needed, and then to unload the plugin, the engine will 
 * call #destroyModule().
 *
 * #createModule() should allocate and return a reference to your own class that inherits and
 * implements the Module interface, and #destroyModule() should receive that same reference
 * and de-allocate it.
 *
 * @see
 *  - createModule()
 *  - Refer to the User Guide for how to create a SteerSuite plugin.
 */
PLUGIN_API void destroyModule( SteerLib::ModuleInterface* module );


#endif
