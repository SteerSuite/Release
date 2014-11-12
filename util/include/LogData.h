//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * LogData.h
 *
 *  Created on: 2014-05-13
 *      Author: glenpb
 */

#ifndef LOGDATA_H_
#define LOGDATA_H_

#include "Logger.h"

class UTIL_API LogData
{
public:
	LogData();
	~LogData();

	void setLogger(Logger * log);
	void addLogData(LogObject * logObj);
	void setLogData(std::vector<LogObject *> logData);
	LogObject * getLogDataAt(size_t i) const;
	Logger * getLogger() const;
	size_t size();
	void appendLogData(LogData * logD);

private:
	Logger * log;
	std::vector<LogObject *> logData;
};

#endif /* LOGDATA_H_ */
