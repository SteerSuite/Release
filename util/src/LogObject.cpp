//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "LogObject.h"
#include <cassert>
#include "PluginAPI.h"



std::ostream& operator<<(std::ostream& output, const LogObject& logObject) {

	output << logObject.getRecordSize() << " ";
	for (unsigned int i=0; i < logObject.getRecordSize(); i++)
	{
		output << logObject.getLogData(i).integerData << " ";
	}
	//output << "\n";
    return output;  // for multiple << operators.
}

std::istream& operator>>(std::istream& input, LogObject& logObject)
{
	unsigned int numRecords = 0;
	input >> numRecords;

	// std::cout << "num records in istream " << numRecords << "\n";

	for (unsigned int i=0; i < numRecords; i++)
	{
		int dataItem;
		input >> dataItem;

		logObject.addLogData ( dataItem);
	}

	return input;
}

const DataItem& LogObject::getLogData(unsigned int index) const
{
	assert ( index < _record.size ());
	return _record[index];
}

const size_t LogObject::getRecordSize () const
{
	return _record.size();
}

extern "C"
{
	PLUGIN_ LogObject * LogObject_new()
	{
		return new LogObject();
	}

	PLUGIN_ void  _addLogObjectFloat(LogObject * logObj, float f)
	{
		logObj->addLogData(f);
	}

	PLUGIN_ void  _addLogObjectInt(LogObject * logObj, int f)
	{
		logObj->addLogData(f);
	}

	PLUGIN_ int getIntegerLogData(LogObject * logObj, int index)
	{
		return logObj->getLogData(index).integerData;
	}
	PLUGIN_ float getFloatLogData(LogObject * logObj, int index)
	{
		return logObj->getLogData(index).floatData;
	}
	PLUGIN_ long long getLongLongLogData(LogObject * logObj, int index)
	{
		return logObj->getLogData(index).longlongData;
	}
	PLUGIN_ const char * getStringLogData(LogObject * logObj, int index)
	{
		return logObj->getLogData(index).string.c_str();
	}

	const size_t getLogObjectSize(LogObject * logobj)
	{
		return logobj->getRecordSize();
	}
}
