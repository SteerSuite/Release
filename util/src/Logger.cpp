//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
#include "Logger.h"
#include <cassert>
#include <sstream>
#include "PluginAPI.h"


Logger::Logger (const std::string & fileName, LogMode logMode)
{
	_fileName = fileName;

	// open file pointer 
	if ( logMode == LogMode::Write)
		_fileStream.open(fileName.c_str(),std::ios::out);
	else if ( logMode == LogMode::Read)
		_fileStream.open(fileName.c_str(),std::ios::in);
}

Logger::~Logger()
{
	_fieldNames.clear ();
	_dataTypes.clear ();

	// close file pointer 
	_fileStream.close ();

}

void 
Logger::addDataField(const std::string &fieldName, DataType dataType)
{
	_fieldNames.push_back(fieldName);
	_dataTypes.push_back(dataType);
}
	
DataType Logger::getFieldDataType(unsigned int index) const
{
	// std::cout << "index is: " << index << " and dataTypes has: " << _dataTypes.size() << std::endl;
	assert (index < _dataTypes.size());
	return _dataTypes[index];
}

std::string Logger::getFieldName(unsigned int index) const
{
	assert (index < _fieldNames.size());
	return _fieldNames[index];
}

size_t Logger::getNumberOfFields () const
{
	assert (_fieldNames.size() == _dataTypes.size ());
	return _fieldNames.size ();
}

void Logger::writeLogObject ( const LogObject & logObject )
{
	std::cout << "Records in LogObject " << logObject.getRecordSize() << std::endl;
	for (unsigned int i=0; i < logObject.getRecordSize(); i++)
	{
		switch ( getFieldDataType(i) )
		{
		case DataType::Float:
			_fileStream << logObject.getLogData(i).floatData << " ";
			break;
		case DataType::Integer:
			_fileStream << logObject.getLogData(i).integerData << " ";
			break;
		case DataType::LongLong:
			// std::cout << "Writing LongLongData " << logObject.getLogData(i).longlongData << std::endl;
			_fileStream << logObject.getLogData(i).longlongData << " ";
			break;
		case DataType::String:
			// std::cout << "writing out some char data: " << logObject.getLogData(i).charstring << std::endl;

			_fileStream << logObject.getLogData(i).string << " ";
			break;
		default:
			std::cerr << "Unspecified data type for log object \n";
			break;
		}
	}
	_fileStream << "\n";
	_fileStream.sync(); // added, encase of crash, testcases that past will be logged.

}

void Logger::writeLogObjectPretty ( const LogObject & logObject )
{
	//_fileStream << logObject.getRecordSize() << " ";
	std::stringstream valueHolder;
	for (unsigned int i=0; i < logObject.getRecordSize(); i++)
	{
		switch ( getFieldDataType(i) )
		{
		case DataType::Float:
			valueHolder << logObject.getLogData(i).floatData;
			_fileStream << valueHolder.str() << calcBufferSpace(_fieldNames.at(i), valueHolder.str()) <<  " ";
			break;
		case DataType::Integer:
			valueHolder << logObject.getLogData(i).integerData;
			_fileStream << logObject.getLogData(i).integerData << calcBufferSpace(_fieldNames.at(i), valueHolder.str()) << " ";
			break;
		case DataType::LongLong:
			valueHolder << logObject.getLogData(i).longlongData;
			_fileStream << logObject.getLogData(i).longlongData << calcBufferSpace(_fieldNames.at(i), valueHolder.str()) << " ";
			break;
		case DataType::String:
			// std::cout << "writing out some char data: " << logObject.getLogData(i).charstring << std::endl;
			valueHolder << logObject.getLogData(i).string;
			_fileStream << logObject.getLogData(i).string << calcBufferSpace(_fieldNames.at(i), valueHolder.str()) << " ";
			break;
		default:
			std::cerr << "Unspecified data type for log object \n";
			break;
		}
		valueHolder.str("");
	}
	_fileStream << "\n";
	_fileStream.sync(); // added, incase of crash cases that past will be logged.

}


std::string Logger::logObjectToString ( const LogObject & logObject )
{
	//_fileStream << logObject.getRecordSize() << " ";
	std::stringstream ss;
	for (unsigned int i=0; i < logObject.getRecordSize(); i++)
	{
		switch ( getFieldDataType(i) )
		{
		case DataType::Float:
			ss << logObject.getLogData(i).floatData << " ";
			break;
		case DataType::Integer:
			ss << logObject.getLogData(i).integerData << " ";
			break;
		case DataType::LongLong:
			ss << logObject.getLogData(i).longlongData << " ";
			break;
		case DataType::String:
			// std::cout << "writing out some char data: " << logObject.getLogData(i).charstring << std::endl;

			ss << logObject.getLogData(i).string << " ";
			break;
		default:
			std::cerr << "Unspecified data type for log object \n";
			break;
		}
	}
	ss << "\n";
	return ss.str();

}

/**
 * Used to help make the output file a little more readable
 * First string should be the field name
 * Second should be the data converted to string
 *
 * Having the data be a longer string than the field breaks this
 */
std::string Logger::calcBufferSpace(std::string one, std::string two)
{

	int lendiff = one.length() - two.length();
	std::stringstream bufferSpace;
	while (lendiff-- > 0)
	{
		bufferSpace << " ";
	}
	return bufferSpace.str();
}

void Logger::readNextLogObject ( LogObject & logObject)
{
	unsigned int numRecords = 0;
	_fileStream >> numRecords;

	std::cout << "num records in istream " << numRecords << "\n";

	for (unsigned int i=0; i < numRecords; i++)
	{
		DataItem dataItem; 
		switch ( getFieldDataType(i) )
		{
		case DataType::Float:
			_fileStream >> dataItem.floatData;
			break;
		case DataType::Integer:
			_fileStream >> dataItem.integerData;
			break;
		case DataType::LongLong:
			_fileStream >> dataItem.longlongData;
			break;
		case DataType::String:
			_fileStream >> dataItem.string;
			break;
		default:
			std::cerr << "Unspecified data type for log object \n";
			break;
		}

		logObject.addLogDataItem ( dataItem);
	}

}

void 
Logger::closeLog()
{
	_fileStream.close ();
}


void Logger::writeMetaData ()
{
	//_fileStream << getNumberOfFields() << " ";
	for (unsigned int i=0; i < getNumberOfFields(); i++)
	{
		_fileStream << getFieldName(i) << " ";
		_fileStream << getFieldDataType(i) << " ";
	}
	_fileStream << "\n";

}

std::string Logger::getMetaData ()
{
	//_fileStream << getNumberOfFields() << " ";
	std::stringstream ss;
	for (unsigned int i=0; i < getNumberOfFields(); i++)
	{
		ss << getFieldName(i) << " ";
		ss << getFieldDataType(i) << " ";
	}
	ss << "\n";
	return ss.str();

}

void Logger::readMetaData ()
{
	std::cout << "logger read meta data  \n";

	unsigned int numberOfFields; 
	_fileStream >> numberOfFields; 

	for (unsigned int i=0; i < numberOfFields; i++)
	{
		std::string fieldName; 
		DataType dataType;
		int dataTypeInt; 

		_fileStream >> fieldName;
		_fileStream >> dataTypeInt;

		switch(dataTypeInt)
		{
			case 0: dataType = DataType::Integer; break;
			case 1: dataType = DataType::Float; break;
			case 2: dataType = DataType::LongLong; break;
			case 4: dataType = DataType::String; break;
		}

		addDataField(fieldName,dataType);
	}

}


PLUGIN_ Logger * new_Logger()
{
	return new Logger();
}
PLUGIN_ const char * printLogData(Logger * log, LogObject * logObj)
{
	// std::cout << "C print " << log->logObjectToString(*logObj) << std::endl;
	std::stringstream labelStream;
	size_t i;
	for (i =0; i < log->getNumberOfFields() - 1; i++)
	{
		labelStream << log->getFieldName(i) << ",";
	}
	labelStream << log->getFieldName(i) << std::endl;

	for (i =0; i < logObj->getRecordSize() - 1; i++)
	{
		labelStream << logObj->getLogData(i).integerData << ",";
	}
	labelStream << logObj->getLogData(i).integerData;


	return labelStream.str().c_str();
}

PLUGIN_ void addIntegerDataField(Logger * log, const char * s)
{
	log->addDataField(std::string(s), DataType::Integer);
}
PLUGIN_ void addFloatDataField(Logger * log, const char * s)
{
	log->addDataField(std::string(s), DataType::Float);
}

PLUGIN_ const char * getLogMetaData(Logger * log)
{
	return log->getMetaData().c_str();
}

PLUGIN_ const char * getFieldName(Logger * log, unsigned int index)
{
	return log->getFieldName(index).c_str();
}
PLUGIN_ int getLoggerFieldDataType(Logger * log, unsigned int index)
{
	return log->getFieldDataType(index);
}

