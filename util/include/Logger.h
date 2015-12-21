//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __LOGGER__
#define __LOGGER__

#include <fstream>
#include "LogObject.h"
#include "UtilGlobals.h"
#include <string>

enum DataType
{
	Integer = 0,
	Float = 1,
	LongLong = 2,
	String = 3
};

enum LogMode
{
	Read,
	Write
};

class UTIL_API Logger
{

public:
	Logger (const std::string & fileName, LogMode logMode);
	Logger () {}; // for testing
	virtual ~Logger();

	virtual void addDataField(const std::string &fieldName, DataType dataType);
	
	DataType getFieldDataType(unsigned int index) const; 
	virtual std::string getFieldName(unsigned int index) const; 
	virtual size_t getNumberOfFields () const; 

	void writeMetaData ();
	std::string getMetaData ();
	void readMetaData ();

	virtual void writeLogObject ( const LogObject & logObject );
	void writeLogObjectPretty ( const LogObject & logObject );
	void readNextLogObject ( LogObject & logObject);
	std::string logObjectToString ( const LogObject & logObject );

	std::string calcBufferSpace(std::string one, std::string two);

	template <typename T> 
	void writeData (T data )
	{
		_fileStream << data << "\n";
		_fileStream.sync();
	}


	void closeLog ();

private:

	std::fstream _fileStream;
	std::string _fileName;

	// meta-information 
	std::vector<std::string> _fieldNames;
	std::vector<DataType> _dataTypes;
};

#endif 
