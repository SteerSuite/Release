//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file MemoryMapper.cpp
/// @brief Implements the Util::MemoryMapper class.


#include <string>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
//#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif // #ifdef _WIN32 else

#include "util/MemoryMapper.h"
#include "util/GenericException.h"
#include "util/Misc.h"

using namespace std;
using namespace Util;


//
// constructor
//
MemoryMapper::MemoryMapper()
{
	_filename = "";
	_fileSize = 0;
	_basePtr = NULL;
	_fileHandle = 0;
	_opened = false;
}

//
// destructor
//
MemoryMapper::~MemoryMapper()
{
	if (_opened) this->close();
}


//
// open()
//
void MemoryMapper::open( std::string filename )
{
	if (_opened) {
		throw GenericException("MemoryMapper::open(): this memory mapped file is already open.");
	}

#ifdef _WIN32
	// open the file
	// GENERIC_READ indicates read-only, FILE_SHARE_READ indicates that other open requests can open it read-only
	// TODO: consider using FILE_ATTRIBUTE_READONLY instead of FILE_ATTRIBUTE_NORMAL ?
	HANDLE fHandle = CreateFileA( filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( fHandle == INVALID_HANDLE_VALUE ) {
		throw GenericException("Could not open file \"" + filename + "\".");
	}

	// get the file size
	DWORD fSize, hibits=0;
	fSize = GetFileSize(fHandle,&hibits);
	if (hibits != 0) {
		throw GenericException("MemoryMapper::open(): currently do not support very large files.");
	}

	// create a temporary mapping
	HANDLE mapping = CreateFileMapping(fHandle, NULL, PAGE_READONLY, 0, (DWORD)fSize, NULL);
	if( mapping == NULL ) {
		throw GenericException("MemoryMapper::open(): could not create file mapping;  CreateFileMapping returned error code " + toString(GetLastError()) );
	}

	// get the base pointer for the mapping
	_basePtr = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, fSize );
	if( _basePtr == NULL ) 
	{
		throw GenericException("MemoryMapper::open(): could not memory map the file; MapViewOfFile returned error code " + toString(GetLastError()) );
	}

	// clean up the temporary handle
	CloseHandle(mapping);

	// initialize rest of the variables once we know everything was successful.
	_fileSize = fSize;
	_fileHandle = fHandle;
	_filename = filename;
	_opened = true;

#else
	// open the file
	// the "::" tells C++ to resolve open() at global scope
	int fd = ::open( filename.c_str(), O_RDONLY);
	if (fd == -1) {
		throw GenericException("Could not open file \"" + filename + "\"." );
	}

	// get the file size
	struct stat fileInfo;
	if (fstat( fd, &fileInfo ) == -1) {
		throw GenericException("MemoryMapper::open(): could not fstat the file \"" + filename + "\".");
	}

	// get the base pointer for the mapping
	_basePtr = mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (_basePtr == MAP_FAILED) {
		throw GenericException("MemoryMapper::open(): could not memory map the file \"" + filename + "\".");
	}

	// initialize vars once we know everything was successful.
	_fileSize = fileInfo.st_size;
	_fileHandle = fd;
	_filename = filename;
	_opened = true;

#endif
}


//
// close()
//
void MemoryMapper::close()
{
	if (!_opened) {
		throw GenericException("MemoryMapper::close(): no file was opened in the first place.");
	}

#ifdef _WIN32
	// un-map the memory-mapped file
	if (UnmapViewOfFile( _basePtr ) == false) {
		throw GenericException("MemoryMapper::close(): an error occurred while trying to un-map the file.");
	}

	// close the file
	CloseHandle( _fileHandle );
#else
	// un-map the memory-mapped file
	if (munmap(_basePtr, _fileSize) == -1) {
		throw GenericException("MemoryMapper::close(): an error occurred while trying to un-map the file.");
	}

	// close the file
	// the "::" tells C++ to resolve close() from global scope, to invoke the open syscall
	::close(_fileHandle);
#endif

	_filename = "";
	_fileSize = 0;
	_basePtr = NULL;
	_fileHandle = 0;
	_opened = false;

}


//
// getPointerAtOffset(): note that offset is measured in bytes.
//
void * MemoryMapper::getPointerAtOffset(unsigned int offset)
{
	if (offset >= _fileSize) {
		throw GenericException("MemoryMapper::getPointerAtOffset(): requested offset is out of bounds.  requested: " + toString(offset) + ", bounds: 0-" + toString((_fileSize-1)) );
	}
	return &(((char*)_basePtr)[offset]);
}

