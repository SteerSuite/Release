//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file DynamicLibrary.cpp
/// @brief Implements the Util::DynamicLibrary class.


#include "util/DynamicLibrary.h"
#include "util/GenericException.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif


using namespace std;
using namespace Util;

void DynamicLibrary::_init()
{
    _libraryName = "";
    _isLoaded = false;
    _libraryHandle = NULL;
}


DynamicLibrary::DynamicLibrary()
{
    _init();
}


DynamicLibrary::DynamicLibrary( const std::string & libraryName )
{
    _init();
    load(libraryName);
}


void DynamicLibrary::load( const std::string & libraryName )
{
    if (_isLoaded) {
		throw GenericException( "Library is already loaded in this instance, either unload the library or use a new instance." );
    }
#ifdef _WIN32
    _libraryHandle = LoadLibrary( libraryName.c_str() );
    if (_libraryHandle == NULL) {
		throw GenericException( "Could not load dynamic library \"" + libraryName + "\"." );
    }
#else
    _libraryHandle = dlopen( libraryName.c_str(), RTLD_LAZY | RTLD_LOCAL );
    if (_libraryHandle == NULL) {
	throw GenericException( std::string(dlerror()) );
    }
#endif
    _libraryName = libraryName;
    _isLoaded = true;
}


void DynamicLibrary::unload( )
{
    if (!_isLoaded)
		return;

#ifdef _WIN32
    if (FreeLibrary(_libraryHandle) == false) {
		throw GenericException( "Unloading the library failed." );
    }
#else
    if (dlclose( _libraryHandle ) != 0) {
		throw GenericException( std::string( dlerror()) );
    }
#endif
    _init();
}


void* DynamicLibrary::getSymbol( const std::string & symbolName, bool throwIfNotFound )
{
    if (!_isLoaded) {
		throw GenericException( "Cannot call DynamicLibrary::getSymbol(), no library is loaded yet." );
    }

    void * symbol = NULL;

#ifdef _WIN32
    // symbol is NULL on error
    symbol = GetProcAddress( _libraryHandle,  symbolName.c_str() );
    if (symbol==NULL && throwIfNotFound) {
		throw GenericException( "In dynamic library " + _libraryName + ", could not load symbol named " + symbolName + "." );
    }
#else
    // dlsym() may be correct even if it returns NULL.
    // So, the correct way to look for errors is to clear dlerror() by calling it once,
    // then call dlsym(), and check if dlerror() had any error message afterwards.
    dlerror();
    symbol = dlsym( _libraryHandle,  symbolName.c_str() );
    char * errorMsg = dlerror();
    if (errorMsg != NULL) {
	if (throwIfNotFound) {
	    throw GenericException( std::string(errorMsg) );
	}
	else {
	    symbol = NULL;
	}
    }
#endif
    return symbol;
}
