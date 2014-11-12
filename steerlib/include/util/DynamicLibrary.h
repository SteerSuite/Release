//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_DYNAMIC_LIBRARY_H__
#define __UTIL_DYNAMIC_LIBRARY_H__

/// @file DynamicLibrary.h
/// @brief Declares Util::DynamicLibrary, a platform-independent wrapper for loading dynamic libraries.

#ifdef _WIN32
// see steerlib/util/DrawLib.h for explanation
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "Globals.h"

namespace Util {

    /**
     * @brief A platform-independent wrapper for loading dynamic libraries.
     *
     * This class wraps Windows and Linux dynamic library functionality so that it is works the same on both platforms.
     *
     */
    class UTIL_API DynamicLibrary
	{
	public:
	    DynamicLibrary();
	    /// This constructor also loads the library, and #load() does not need to be called.
	    DynamicLibrary( const std::string & libraryName );

	    /// loads the library specified by libraryName. Make sure to use explicit relative paths.
	    void load( const std::string & libraryName );
	    /// unloads the library associated with this class
	    void unload( );
	    /// gets the address for a symbol (a function or variable) located in the library.
	    void * getSymbol( const std::string & symbolName, bool throwIfNotFound );


	    /// Returns the name of the library
	    std::string getName() { return _libraryName; }
	    /// Returns true if a dynamic library is currently loaded.
	    bool isLoaded() { return _isLoaded; }
	    
	protected:
	    void _init();
	    std::string _libraryName;
	    bool _isLoaded;
	    
#ifdef _WIN32
	    HMODULE _libraryHandle;
#else
	    void * _libraryHandle;
#endif
	    
	};
    
}  // end namespace Util

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif

