//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __LOG_MANAGER__
#define __LOG_MANAGER__

#include <iostream>
#include <map>
#include "Logger.h"
#include "UtilGlobals.h"


enum UTIL_API LoggerType 
{
	BASIC_READ,
	BASIC_WRITE
	// add other loggers 
};

class UTIL_API LogManager
{
public:

	static LogManager * getInstance (); // returns single static instance of LogManager 
	Logger * createLogger ( const std::string &logName, LoggerType loggerType = LoggerType::BASIC_WRITE);

private:

	LogManager () {} // constructor is declared private to prevent instantiation 
	
	static LogManager * _instance;

	std::map<std::string , Logger *> _loggers;

};

#endif 
