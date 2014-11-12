//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_GLOBALS_H__
#define __STEERLIB_GLOBALS_H__

/// @file Globals.h
/// @brief Contains global macros and variables used by %SteerLib.

/// @def STEERLIB_API
/// @brief Defines that the corresponding function/variable/symbol should be made available from the SteerLib library.
/// @def UTIL_API
/// @brief Defines that the corresponding function/variable/symbol should be made available from the SteerLib library (used for the Util namespace in case it becomes a separate library).
#ifdef _WIN32
#ifdef _COMPILING_WIN32_STEERLIB
#define STEERLIB_API __declspec(dllexport)
#define UTIL_API __declspec(dllexport)
#else
#define STEERLIB_API
#define UTIL_API
#endif
#else
#define STEERLIB_API
#define UTIL_API
#endif

/// @todo the ENABLE_MULTITHREADING macro is new and not used everywhere that it should be, yet.
///  eventually make it a compile-time option for users.
#define ENABLE_MULTITHREADING

#define ENABLE_GLFW



#endif
