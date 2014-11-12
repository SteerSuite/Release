//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_MEMORY_MAPPER_H__
#define __UTIL_MEMORY_MAPPER_H__

/// @file MemoryMapper.h
/// @brief Declares a platform-independent memory mapper for reading files.

#ifdef _WIN32
// see steerlib/util/DrawLib.h for explanation
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include "Globals.h"

namespace Util {

	/**
	 * @brief A platform-independent class that provides an easy way to use read-only memory mapping.
	 *
	 * Memory mapping is the technique of mapping a process's memory address space to a different resource,
	 * in this case, a file.  In many cases it can help performance significantly compared to
	 * the traditional file seek mechanism.
	 *
	 * This class is a platform-independent interface to use read-only memory mapping on files.
	 *
	 * To use this class, simply call open() with the file you want to map to memory;
	 * Then, #getBasePointer() can be used to get a pointer to the beginning of the file,
	 * and #getPointerAtOffset() can be used to get a pointer to any arbitrary location in the file.
	 * These pointers can be treated just like any other pointers until the file is closed.  Note
	 * that these pointers do not need to be freed or de-allocated, simply call #close() when you
	 * are done.
	 *
	 */
	class UTIL_API MemoryMapper {
	public:
		MemoryMapper();
		~MemoryMapper();
		/// Opens a file for read-only memory mapping.
		void open( std::string filename );
		/// Closes the file.
		void close();
		/// Returns a pointer to the beginning of the file
		void * getBasePointer() { return _basePtr; }
		/// Returns a pointer to an arbitrary location in the file, where offset is measured in bytes.
		void * getPointerAtOffset(unsigned int offset);
		/// Returns the size of the file in bytes.
		unsigned int getFileSize() { return _fileSize; }
		/// Returns true if a file is open (which implies it is successfully memory mapped), false if otherwise.
		bool isOpen() { return _opened; }

	protected:
		std::string _filename;
		bool _opened;
		unsigned int _fileSize;
		void * _basePtr;
#ifdef _WIN32
		void * _fileHandle;
#else
		int _fileHandle;
#endif
	};

} // end namsepace Util

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
