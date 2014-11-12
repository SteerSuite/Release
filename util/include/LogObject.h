//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
#ifndef __LOGOBJECT__
#define __LOGOBJECT__


#include <string> // c++ stl string class 
#include <cstring> // for strcmp (c string.h) 
#include <iostream>
#include <vector>
#include <typeinfo>
#include "UtilGlobals.h"


struct UTIL_API DataItem // using a union was causing memory corruption issues with char *
{
	int integerData;
	float floatData;
	long long longlongData;
	// TODO ADD OTHER DATA TYPES HERE 
	std::string string; // unions can not handle this (no complex types).
};


class UTIL_API LogObject 
{
	friend std::ostream& operator<<(std::ostream& output, const LogObject& logObject);
	friend std::istream& operator>>(std::istream& input, LogObject& logObject);

public:
	LogObject() {}
	~LogObject () {};

	template<class T>
	void addLogData (T dataItem)
	{
		struct DataItem data;
		std::string type = typeid(dataItem).name() ;
		if ( type[0] == 'i')
		{
			data.integerData = dataItem;
	#ifdef _DEBUG
			std::cout << "Logged int " << data.integerData << "\n" ;
	#endif
		}
		else if ( type[0] == 'f')
		{
				//std::cout << "Logging float " << dataItem << "\n" ;
			data.floatData = dataItem;
		}
		else if ( type[0] == 'x' || type[0] == '_') // _int64
		{
				//std::cout << "Logging _int64 " << dataItem << "\n" ;
			data.longlongData = dataItem;
		}
		else if ( type.compare("std::string") == 0 ) // probably does not work
		{
			std::cout << "Found a string type" << std::endl;
			data.string = dataItem;
		}
		else
		{
			std::cerr << "ERROR: addLogData() Unknown data type " << type <<"\n" ;
		}
		// TODO ADD OTHER DATA TYPES HERE

		 _record.push_back(data);
	}

	void addLogDataItem (DataItem dataItem)
	{
		_record.push_back(dataItem);
	}

	const DataItem& getLogData(unsigned int index) const;
	const size_t getRecordSize () const; 

	LogObject * copy()
	{
		LogObject * tmp_logobj = new LogObject();
		for (size_t i=0; i < this->getRecordSize(); i++)
		{
			tmp_logobj->addLogDataItem(this->getLogData(i));
		}
		return tmp_logobj;
	}

private:

	std::vector<DataItem> _record;
};

#endif
