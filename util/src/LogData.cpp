//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * LogData.cpp
 *
 *  Created on: 2014-05-13
 *      Author: glenpb
 */

#include "LogData.h"
#include "PluginAPI.h"
#include <assert.h>

LogData::LogData()
{
	// TODO Auto-generated constructor stub

}

LogData::~LogData()
{
	// TODO Auto-generated destructor stub
	delete log;
}

void LogData::setLogger(Logger * log)
{
	this->log = log;
}
void LogData::addLogData(LogObject * logObj)
{
	this->logData.push_back(logObj);
}
void LogData::setLogData(std::vector<LogObject *> logData)
{
	this->logData = logData;
}

LogObject * LogData::getLogDataAt(size_t i) const
{
	return this->logData.at(i);
}

Logger * LogData::getLogger() const
{
	return this->log;
}

size_t LogData::size()
{
	return this->logData.size();
}

/*
 * the size() of logD must be the same as this
 */
void LogData::appendLogData(LogData * logD)
{
	std::cout << "this->size() == logD->size(), " << this->size() << " != " << logD->size() << std::endl;
	assert(this->size() == logD->size()); //, std::string("this->size() == logD->size(), ") + this->size() + " != " + logD->size());
	// Add fields to Logger
	for (size_t i=0; i < logD->getLogger()->getNumberOfFields(); i++)
	{
		this->getLogger()->addDataField(logD->getLogger()->getFieldName(i), logD->getLogger()->getFieldDataType(i));
	}

	// append data to end of each log object already if one exists
	for (size_t i=0; i < this->size(); i++)
	{
		for (size_t j=0; j < logD->getLogDataAt(i)->getRecordSize(); j++)
		{
			this->getLogDataAt(i)->addLogDataItem(logD->getLogDataAt(i)->getLogData(j));
		}
	}

	// this->size() == old->size()
	// for all this->LogData.size() == (old->logData.size() + logD->logData.size())
}



PLUGIN_ LogObject * getLogObjectAt(LogData * log, size_t i)
{
	// std::cout << *(log->getLogDataAt(i)) << std::endl;
	return log->getLogDataAt(i);
}
PLUGIN_ Logger * getLogger(LogData * log)
{
	return log->getLogger();
}
PLUGIN_ unsigned long long logDataLength(LogData * log)
{
	return log->size();
}

