//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
#ifndef __LOGGER__
#define __LOGGER__

#include <fstream>
#include "LogObject.h"
#include "UtilGlobals.h"
#include <string>

enum UTIL_API DataType 
{
	Integer = 0,
	Float = 1,
	LongLong = 2,
	String = 3
};

enum UTIL_API LogMode 
{
	Read,
	Write
};

class UTIL_API Logger 
{

public:
	Logger (const std::string & fileName, LogMode logMode);
	Logger () {}; // for testing
	~Logger();

	void addDataField(const std::string &fieldName, DataType dataType);
	
	DataType getFieldDataType(unsigned int index) const; 
	std::string getFieldName(unsigned int index) const; 
	size_t getNumberOfFields () const; 

	void writeMetaData ();
	std::string getMetaData ();
	void readMetaData ();

	void writeLogObject ( const LogObject & logObject );
	void writeLogObjectPretty ( const LogObject & logObject );
	void readNextLogObject ( LogObject & logObject);
	std::string logObjectToString ( const LogObject & logObject );

	std::string calcBufferSpace(std::string one, std::string two);

	template <typename T> 
	void writeData (T data )
	{
		_fileStream << data << "\n";
		// _fileStream.sync();
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
