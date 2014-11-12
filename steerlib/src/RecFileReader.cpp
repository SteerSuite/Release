//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file RecFileReader.cpp
/// @brief Implements the SteerLib::RecFileReader class.

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "util/GenericException.h"
#include "util/MemoryMapper.h"
#include "util/Misc.h"
#include "recfileio/RecFileIO.h"


using namespace std;
using namespace SteerLib;
using namespace Util;


#define CHECK_MAX_INDEX(value,numItems,parameterName,functionName) \
	if ((value) >= (numItems)) { \
		std::stringstream ss; \
		ss << (functionName) << ": " << (parameterName) << "=" << (value) << " is out of bounds; there are only " << (numItems-1) << " items."; \
		throw GenericException(ss.str()); \
	} \


#define CHECK_BOUNDS(value,min,max,parameterName,functionName) \
	if (  ((value) < (min))  || ((value) > (max))  ) { \
		std::stringstream ss; \
		ss << (functionName) << ": " << (parameterName) << "=" << (value) << " is out of bounds; valid interval is (" << (min) << "," << (max) << ")."; \
		throw GenericException(ss.str()); \
	} \


void RecFileReaderPrivate::_getFramesForTime(float time, unsigned int &frameIndex1, unsigned int &frameIndex2)
{
	// the previous values of f1 and f2 are saved in the class, so we can just quickly test if the requested time
	// is still on these frames or one frame ahead.  This can save us from doing a logarithmic lookup.

	// simple aliases used to rename these member variables.
	unsigned int & f1 = f1_used_in_getFramesForTimeFunction;
	unsigned int & f2 = f2_used_in_getFramesForTimeFunction;
	float & prevTime = prevTime_used_in_getFramesForTimeFunction;
	unsigned int pivot;
	float t1, t2, tmid;

	if ((time >= _frameTable[f1].timeStamp) && (time <= _frameTable[f2].timeStamp)) {
		// nothing to do, f1 and f2 are still the correct answer.
	}
	else if ((time >= _frameTable[f2].timeStamp) && (time <= _frameTable[f2+1].timeStamp)) {
		// f1 and f2 are just the next interval of frames.
		f1 = f2;
		f2 = f2+1;
	}
	else {
		// we coulnd't find the right one by guessing, so actually perform the binary search.
		f1 = 0;
		f2 = _header->numFrames-1;

		while (f2-f1 > 1) {
			pivot = (f1 + f2) / 2;
			t1 = _frameTable[f1].timeStamp;
			t2 = _frameTable[f2].timeStamp;
			tmid = _frameTable[pivot].timeStamp;
			if (time == tmid) {
				f1 = pivot;
				f2 = pivot;
			}
			else if (time < tmid) {
				f2 = pivot;
			}
			else {
				f1 = pivot;
			}
		}
	}

	frameIndex1 = f1;
	frameIndex2 = f2;
	prevTime = time;

	if ((frameIndex1 > frameIndex2) || (time < _frameTable[frameIndex1].timeStamp) || (time > _frameTable[frameIndex2].timeStamp)) {
		cerr << "INTERNAL ERROR in RecFileReaderPrivate::_getFramesForTime(), invalid args specified.\n";
		assert(false);
	}

	return;
}



//===========================================================================
//===========================================================================

//
// constructor
//
RecFileReader::RecFileReader()
{
	_filename = "";
	_testCaseName = "";
	_opened = false;
	_version = 0;
	_header = NULL;
	_obstacleList = NULL;
	_cameraList = NULL;
	_frameTable = NULL;
	_frames = NULL;

	f1_used_in_getFramesForTimeFunction = 0;
	f2_used_in_getFramesForTimeFunction = 0;
	prevTime_used_in_getFramesForTimeFunction = 0.0f;
}

//
// constructor asking to open a file
//
RecFileReader::RecFileReader(const std::string & filename)
{
	_filename = "";
	_testCaseName = "";
	_opened = false;
	_version = 0;
	_header = NULL;
	_obstacleList = NULL;
	_cameraList = NULL;
	_frameTable = NULL;
	_frames = NULL;

	f1_used_in_getFramesForTimeFunction = 0;
	f2_used_in_getFramesForTimeFunction = 0;
	prevTime_used_in_getFramesForTimeFunction = 0.0f;
	open(filename);
}


//
// destructor
//
RecFileReader::~RecFileReader()
{
	if (_fileMap.isOpen()) _fileMap.close();
	if (_frames != NULL) delete [] _frames;
}


//
// isAValidRecFile()
//
bool RecFileReader::isAValidRecFile(const std::string & filename)
{
	ifstream recFile;
	recFile.open( filename.c_str(), ios::binary );

	if (!recFile.is_open()) {
		return false;
	}

	unsigned int magic;

	recFile.read((char*)&magic, sizeof(unsigned int));
	recFile.close();

	if (magic != RECFILE_MAGIC_NUMBER) {
		return false;
	}

	return true;
}


//
// open()
//
void RecFileReader::open(const std::string & filename)
{
	if (isOpen()) {
		throw GenericException("This RecFileReader already has an opened rec file; close it before trying to open a new one.");
	}
	
	_filename = filename;

	_fileMap.open( _filename );

	_header = (RecFileHeader*)_fileMap.getBasePointer();
	if (_header->magic != RECFILE_MAGIC_NUMBER) {
		std::stringstream ss;
		ss << "RecFileReader::open(): invalid magic number at beginning of file.\n";
		ss << "  found: 0x" << hex << _header->magic << ", expected: 0x" << RECFILE_MAGIC_NUMBER << ".\n" << dec;
		ss << "If you really believe the playback file is valid, then\n";
		ss << "it may be a big-endian/little-endian incompatibility.\n";
		throw GenericException(ss.str());
	}

	// versions 1 and 2 are almost fully compatible, except that version 2 
	// adds a variable-length string immediately after the header.
	_version = _header->version;

	if (_header->version == 1) {
		_testCaseName = "";
	}
	else if (_header->version == 2) {
		_testCaseName = std::string((char*)(_fileMap.getPointerAtOffset(_header->testCaseNameOffset)));
	}
	else {
		throw GenericException("Version incompatibility; this RecFileReader implementation supports versions 1 and 2, but the file is version " + toString(_header->version));
	}
	
	_obstacleList = (RecFileObstacleInfo*)_fileMap.getPointerAtOffset(_header->obstacleListOffset);
	_cameraList = (RecFileCameraInfo*)_fileMap.getPointerAtOffset(_header->cameraListOffset);
	_frameTable = (RecFileFrameInfo*)_fileMap.getPointerAtOffset(_header->frameTableOffset);

	//
	// allocate an array of RecFileAgentInfo* pointers
	//
	_frames = new RecFileAgentInfo*[ _header->numFrames ];
	if (_frames == NULL) {
		throw GenericException("RecFileReader::open(): could not allocate _frames, (an array of pointers)");
	}

	//
	// initialize the array of pointers
	// _frames[i] will be an array of RecFileAgentInfo structures for frame i.
	//
	char * base = (char*)_fileMap.getBasePointer();
	for (unsigned int i=0; i<_header->numFrames; i++) {
		_frames[i] = (RecFileAgentInfo*)(base + _frameTable[i].frameOffset);
	}

	_opened = true;

}


//
// close()
//
void RecFileReader::close()
{
	if (_fileMap.isOpen()) _fileMap.close();
	if (_frames != NULL) delete [] _frames;

	_filename = "";
	_testCaseName = "";
	_opened = false;
	_version = 0;
	_header = NULL;
	_obstacleList = NULL;
	_cameraList = NULL;
	_frameTable = NULL;
	_frames = NULL;
}


//
// getNumFrames()
//
unsigned int RecFileReader::getNumFrames()
{
	return _header->numFrames;
}


//
// getNumAgents()
//
unsigned int RecFileReader::getNumAgents()
{
	return _header->numAgents;
}


//
// getNumCameraViews()
//
unsigned int RecFileReader::getNumCameraViews()
{
	return _header->numCameraViews;
}


//
// getNumObstacles()
//
unsigned int RecFileReader::getNumObstacles()
{
	return _header->numObstacles;
}


//
// getTotalElapsedTime()
//
float RecFileReader::getTotalElapsedTime()
{
	return _header->totalPlaybackTime;
}


//
// getTimeStampForFrame()
//
float RecFileReader::getTimeStampForFrame( unsigned int frameNumber )
{
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "getTimeStampForFrame()");

	return _frameTable[frameNumber].timeStamp;
}


//
// getElapsedTimeBetweenFrames()
//
float RecFileReader::getElapsedTimeBetweenFrames( unsigned int startFrame, unsigned int endFrame)
{
	if (endFrame-startFrame == 1) {
		// For two consecutive frames, use the fully accurate dt instead of subtracting timestamps.
		return _frameTable[startFrame].dtToNextFrame;
	}
	else {
		return getTimeStampForFrame(endFrame) - getTimeStampForFrame(startFrame);
	}
}


//
// getAgentLocationAtFrame()
//
void RecFileReader::getAgentLocationAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &posx, float &posy, float &posz )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentLocationAtFrame()");
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "getAgentLocationAtFrame()");

	posx = _frames[frameNumber][agentIndex].pos.x;
	posy = _frames[frameNumber][agentIndex].pos.y;
	posz = _frames[frameNumber][agentIndex].pos.z;
}


//
// getAgentOrientationAtFrame()
//
void RecFileReader::getAgentOrientationAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &dirx, float &diry, float &dirz )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentOrientationAtFrame()");
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "getAgentOrientationAtFrame()");

	dirx = _frames[frameNumber][agentIndex].dir.x;
	diry = _frames[frameNumber][agentIndex].dir.y;
	dirz = _frames[frameNumber][agentIndex].dir.z;
}


//
// getAgentGoalAtFrame()
//
void RecFileReader::getAgentGoalAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &goalx, float &goaly, float &goalz )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentGoalAtFrame()");
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "getAgentGoalAtFrame()");

	goalx = _frames[frameNumber][agentIndex].goal.x;
	goaly = _frames[frameNumber][agentIndex].goal.y;
	goalz = _frames[frameNumber][agentIndex].goal.z;
}


//
// getAgentRadiusAtFrame()
//
float RecFileReader::getAgentRadiusAtFrame( unsigned int agentIndex, unsigned int frameNumber )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentMiscInfoAtFrame()");
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "getAgentMiscInfoAtFrame()");

	return _frames[frameNumber][agentIndex].radius;
}


//
// getObstacleBoundsAtFrame()
//
void RecFileReader::getObstacleBoundsAtFrame( unsigned int obstacleIndex, unsigned int frameNumber, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax )
{
	CHECK_MAX_INDEX(obstacleIndex, _header->numObstacles, "obstacleIndex", "getObstacleBoundsAtFrame()");
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "getObstacleBoundsAtFrame()");

	xmin = _obstacleList[obstacleIndex].bounds.xmin;
	xmax = _obstacleList[obstacleIndex].bounds.xmax;
	ymin = _obstacleList[obstacleIndex].bounds.ymin;
	ymax = _obstacleList[obstacleIndex].bounds.ymax;
	zmin = _obstacleList[obstacleIndex].bounds.zmin;
	zmax = _obstacleList[obstacleIndex].bounds.zmax;
}


//
// isAgentEnabledAtFrame()
//
bool RecFileReader::isAgentEnabledAtFrame( unsigned int agentIndex, unsigned int frameNumber )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "isAgentEnabledAtFrame()");
	CHECK_MAX_INDEX(frameNumber, _header->numFrames, "frameNumber", "isAgentEnabledAtFrame()");

	return _frames[frameNumber][agentIndex].enabled;
}


//
// getCameraView()
//
void RecFileReader::getCameraView( unsigned int cameraIndex, float &origx, float &origy, float &origz, float &lookatx, float &lookaty, float &lookatz)
{
	CHECK_MAX_INDEX(cameraIndex, _header->numCameraViews, "cameraIndex", "getCameraView()");

	origx = _cameraList[cameraIndex].origin.x;
	origy = _cameraList[cameraIndex].origin.y;
	origz = _cameraList[cameraIndex].origin.z;
	lookatx = _cameraList[cameraIndex].origin.x;
	lookaty = _cameraList[cameraIndex].lookat.y;
	lookatz = _cameraList[cameraIndex].lookat.z;

}




//
// getAgentLocationAtTime()
//
void RecFileReader::getAgentLocationAtTime( unsigned int agentIndex, float time, float &posx, float &posy, float &posz )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentLocationAtTime()");
	CHECK_BOUNDS(time,_frameTable[0].timeStamp,_frameTable[_header->numFrames-1].timeStamp, "time", "getAgentLocationAtTime()");

	unsigned int frameIndex1, frameIndex2;
	_getFramesForTime(time, frameIndex1, frameIndex2);
	
	RecFilePointData p1 = _frames[frameIndex1][agentIndex].pos;
	RecFilePointData p2 = _frames[frameIndex2][agentIndex].pos;

	float beta = (time-_frameTable[frameIndex1].timeStamp) / _frameTable[frameIndex1].dtToNextFrame;
	float alpha = 1.0f - beta;

	posx = alpha*p1.x + beta*p2.x;
	posy = alpha*p1.y + beta*p2.y;
	posz = alpha*p1.z + beta*p2.z;
}


//
// getAgentOrientationAtTime()
//
void RecFileReader::getAgentOrientationAtTime( unsigned int agentIndex, float time, float &dirx, float &diry, float &dirz )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentOrientationAtTime()");
	CHECK_BOUNDS(time,_frameTable[0].timeStamp,_frameTable[_header->numFrames-1].timeStamp, "time", "getAgentOrientationAtTime()");

	unsigned int frameIndex1, frameIndex2;
	_getFramesForTime(time, frameIndex1, frameIndex2);
	
	RecFileVectorData v1 = _frames[frameIndex1][agentIndex].dir;
	RecFileVectorData v2 = _frames[frameIndex2][agentIndex].dir;
	RecFileVectorData r1;

	// WARNING: assuming 2-d x-z plane only right now.  eventually NEED to fix this to be generally 3D.
	if ((v1.y != 0.0f) && (v2.y != 0.0f)) {
		throw GenericException("currently assuming that orientation is 2D in the x-z plane - cannot interpolate if y component is non-zero.");
	}

	r1.x = -v1.z;
	r1.y = v1.y;
	r1.z = v1.x;

	double invNorm1 = 1.0f / sqrtf(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
	double invNorm2 = 1.0f / sqrtf(v2.x*v2.x + v2.y*v2.y + v2.z*v2.z);

	double alpha = (time-_frameTable[frameIndex1].timeStamp) / _frameTable[frameIndex1].dtToNextFrame;
	double cosRatio = ((double)(v1.x*v2.x + v1.y*v2.y + v1.z*v2.z)) * invNorm1 * invNorm2;  // cos x = v1 dot v2 / (|v1| |v2|)

	if (cosRatio > 1.0) cosRatio = 1.0;
	if (cosRatio < -1.0) cosRatio = -1.0;

	if ( (r1.x*v2.x + r1.y*v2.y + r1.z*v2.z) < 0)
		alpha = -alpha;
		

	double angle;
	if (cosRatio >= 1.0f)
		angle = alpha * acos( cosRatio );
	else
		angle = alpha * acos( cosRatio );


	dirx = (float)(cos(angle) * _frames[frameIndex1][agentIndex].dir.x - sin(angle) * _frames[frameIndex1][agentIndex].dir.z);
	diry = _frames[frameIndex1][agentIndex].dir.y;
	dirz = (float)(sin(angle) * _frames[frameIndex1][agentIndex].dir.x + cos(angle) * _frames[frameIndex1][agentIndex].dir.z);

	// for debugging - return non-interpolated vectors
	//dirx = _frames[frameIndex1][agentIndex].dir.x;
	//diry = _frames[frameIndex1][agentIndex].dir.y;
	//dirz = _frames[frameIndex1][agentIndex].dir.z;
}


//
// getAgentGoalAtTime()
//
void RecFileReader::getAgentGoalAtTime( unsigned int agentIndex, float time, float &goalx, float &goaly, float &goalz )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentGoalAtTime()");
	CHECK_BOUNDS(time,_frameTable[0].timeStamp,_frameTable[_header->numFrames-1].timeStamp, "time", "getAgentGoalAtTime()");

	unsigned int frameIndex1, frameIndex2;
	_getFramesForTime(time, frameIndex1, frameIndex2);

	// goal does not interpolate.  use the future time.
	goalx = _frames[frameIndex2][agentIndex].goal.x;
	goaly = _frames[frameIndex2][agentIndex].goal.y;
	goalz = _frames[frameIndex2][agentIndex].goal.z;
}


//
// getAgemtRadiusAtTime()
//
float RecFileReader::getAgentRadiusAtTime( unsigned int agentIndex, float time)
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "getAgentMiscInfoAtTime()");
	CHECK_BOUNDS(time,_frameTable[0].timeStamp,_frameTable[_header->numFrames-1].timeStamp, "time", "getAgentMiscInfoAtTime()");

	unsigned int frameIndex1, frameIndex2;
	_getFramesForTime(time, frameIndex1, frameIndex2);

	// TODO: should we interpolate the radius? 
	float beta = (time-_frameTable[frameIndex1].timeStamp) / _frameTable[frameIndex1].dtToNextFrame;
	float alpha = 1.0f - beta;
	float radius = alpha * _frames[frameIndex1][agentIndex].radius + beta * _frames[frameIndex2][agentIndex].radius;
	return radius;
}



//
// getObstacleBoundsAtTime()
//
void RecFileReader::getObstacleBoundsAtTime( unsigned int obstacleIndex, float time, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax )
{
	CHECK_MAX_INDEX(obstacleIndex, _header->numObstacles, "obstacleIndex", "getObstacleBoundsAtTime()");
	CHECK_BOUNDS(time,_frameTable[0].timeStamp,_frameTable[_header->numFrames-1].timeStamp, "time", "getObstacleBoundsAtTime()");

	//unsigned int frameIndex1, frameIndex2;
	//_getFramesForTime(time, frameIndex1, frameIndex2);

	// in the current file version, obstacles are always static.
	xmin = _obstacleList[obstacleIndex].bounds.xmin;
	xmax = _obstacleList[obstacleIndex].bounds.xmax;
	ymin = _obstacleList[obstacleIndex].bounds.ymin;
	ymax = _obstacleList[obstacleIndex].bounds.ymax;
	zmin = _obstacleList[obstacleIndex].bounds.zmin;
	zmax = _obstacleList[obstacleIndex].bounds.zmax;
}


bool RecFileReader::isAgentEnabledAtTime( unsigned int agentIndex, float time )
{
	CHECK_MAX_INDEX(agentIndex, _header->numAgents, "agentIndex", "isAgentEnabledAtTime()");
	CHECK_BOUNDS(time,_frameTable[0].timeStamp,_frameTable[_header->numFrames-1].timeStamp, "time", "isAgentEnabledAtTime()");

	unsigned int frameIndex1, frameIndex2;
	_getFramesForTime(time, frameIndex1, frameIndex2);

	// "enabled" does not interpolate.
	// both the time before and time after must be valid if the agent is considered enabled at the current time.
	bool enabled = (_frames[frameIndex1][agentIndex].enabled && _frames[frameIndex2][agentIndex].enabled);

	return enabled;
}



