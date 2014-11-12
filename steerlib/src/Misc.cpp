//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file Misc.cpp
/// @brief Imeplements any miscellaneous utility functions and classes.


#include <algorithm>
#include "util/Misc.h"
#include "util/GenericException.h"

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

using namespace Util;


bool Util::isForwardSlash(char ch)
{
	return (char)ch == '/';
}

bool Util::isBackSlash(char ch)
{
	return (char)ch == '/';
}

bool Util::fileCanBeOpened(const std::string & filename)
{
	std::ifstream dummyFile;  
	dummyFile.open(filename.c_str());
	if (dummyFile.is_open()) {
		dummyFile.close();
		return true;
	}
	else {
		return false;
	}
}



bool Util::endsWith( const std::string & str, const std::string & extension )
{
	if (extension.length() > str.length()) {
		return false;
	}

	if (extension.length() == 0) {
		return false;
	}

	std::string::const_iterator strIter;
	std::string::const_iterator extIter;

	strIter = str.end();
	extIter = extension.end();

	// test both strings backwards from their ends
	while ( extIter != extension.begin() ) {
		--strIter;
		--extIter;
		if ((*strIter) != (*extIter)) {
			return false;
		}
	}

	return ((*strIter) == (*extIter));

}


std::string Util::basename( const std::string & path, const std::string & extension )
{
	std::string base;

	// returns a substring from the character after the last '/' or '\' to the end.
	base = path.substr(path.find_last_of("/\\") + 1);

	if (endsWith(base,extension)) {
		base = base.substr(0, base.length() - extension.length());
	}

	return base;
}


bool Util::pathExists( const std::string & pathname )
{
#ifdef _WIN32
	DWORD ret;
	ret = GetFileAttributes(pathname.c_str());
	if (ret == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	else {
		return true;
	}

#else
	struct stat pathInfo;
	if (stat(pathname.c_str(), &pathInfo) == 0 ) {
		// stat succeeded
		return true;
	}
	else {
		// stat had error
		return false;
	}
#endif
}



bool Util::isExistingFile( const std::string & pathname )
{
#ifdef _WIN32
	DWORD ret;
	ret = GetFileAttributes(pathname.c_str());
	if (ret == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	else {
		if (!(ret & FILE_ATTRIBUTE_DIRECTORY)) {
			return true;
		}
		else {
			return false;
		}
	}

#else
	struct stat pathInfo;
	if (stat(pathname.c_str(), &pathInfo) == 0 ) {
		// stat succeeded

		/// @todo
		///   - is this perhaps too restricting, if a user wants to
		///     read a pipe, socket, etc. as if it were a file??
		if ( S_ISREG(pathInfo.st_mode) ) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		// stat had error
		return false;
	}
#endif
}

bool Util::isExistingDirectory( const std::string & pathname )
{
#ifdef _WIN32
	DWORD ret;
	ret = GetFileAttributes(pathname.c_str());
	if (ret == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	else {
		if (ret & FILE_ATTRIBUTE_DIRECTORY) {
			return true;
		}
		else {
			return false;
		}
	}
#else
	struct stat pathInfo;
	if (stat(pathname.c_str(), &pathInfo) == 0 ) {
		// stat succeeded
		if ( S_ISDIR(pathInfo.st_mode) ) {
			return true;
		}
		else {
			return false;
		}
	}
	else
	{
		// stat had error
		return false;
	}
#endif
}


void Util::getFilesInDirectory( const std::string & directoryName, const std::string & extension, std::vector<std::string> & fileNames)
{
#ifdef _WIN32

	std::string dirName = directoryName;
	std::replace_if(dirName.begin(), dirName.end(), isForwardSlash, '\\');
	if (!endsWith(dirName,"\\")) {
		dirName += "\\";
	}

	// for the win32 directory search API, we must add the wild-card, even if we are not
	// filtering by extension (i.e. even if the extension string is blank).
	dirName += "*" + extension;

	HANDLE ret;
	WIN32_FIND_DATA data;
	ret = FindFirstFile(dirName.c_str(), &data);
	if (ret != INVALID_HANDLE_VALUE) {
		if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string fileName(data.cFileName);
			// at this point we know the path is a file and has the desired extension.
			fileNames.push_back(fileName);
		}

		while (FindNextFile(ret, &data)) {
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string fileName(data.cFileName);
				// at this point we know the path is a file and has the desired extension.
				fileNames.push_back(fileName);
			}
		}
	}
#else
	std::string dirName = directoryName;
	std::replace_if(dirName.begin(), dirName.end(), isBackSlash, '/');
	if (!endsWith(dirName,"/")) {
		dirName += "/";
	}

	DIR * dir;
	dir = opendir( dirName.c_str() );
	if (dir != NULL) {
		// Solaris requires that we stat each dirname, rather than
		// using the linux-specific dirInfo->d_type.
		struct dirent * dirEntry;
		struct stat entryInfo;
		dirEntry = readdir(dir);
		while (dirEntry != NULL) {
			std::string filePath = dirName + std::string(dirEntry->d_name);
			if (stat(filePath.c_str(), &entryInfo) == 0 ) {
				if ( S_ISREG(entryInfo.st_mode) ) {
					std::string fileName(dirEntry->d_name);
					if ( endsWith(fileName, extension)) {
						// at this point we know the path is a file and has the desired extension.
						fileNames.push_back(fileName);
					}
				}
			}
			dirEntry = readdir(dir);
		}
	}
#endif
}



bool Util::getBoolFromString( const std::string & str )
{
	std::string boolStr = toLower(str);

	if ((boolStr == "true") || (boolStr == "1") || (boolStr == "yes")) {
		return true;
	}
	else if ((boolStr == "false") || (boolStr == "0") || (boolStr == "no")) {
		return false;
	}
	else {
		throw GenericException("Util::getBoolFromString() - unable to determine the boolean value for string \"" + str + "\".");
	}

	assert(false); // should not get here.
}



std::string Util::toLower (const std::string &s)
{
	std::string result;
	result.resize(s.size());

	std::transform(s.begin(), s.end(), result.begin(), tolower);

	return result;
}
