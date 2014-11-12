//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file RecFileWriter.cpp
/// @brief Implements the SteerLib::RecFileWriter class.

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "util/GenericException.h"
#include "util/Misc.h"
#include "recfileio/RecFileIO.h"

using namespace std;
using namespace SteerLib;
using namespace Util;

//
// constructor
// note that the base constructor is also used
//
RecFileWriter::RecFileWriter()
{
	_filename = "";
	_opened = false;
	_writingFrame = false;
	_version = 2;

	_header = NULL;
	_obstacleList.clear();
	_cameraList.clear();
	_frameTable.clear();
	_agentsInCurrentFrame = NULL;
}


//
// destructor
//
RecFileWriter::~RecFileWriter()
{
	if (_opened) {
		// the user can't query the error stream after we destruct, so just output an error.
		cerr << "WARNING: de-allocating RecFileWriter before the playback file was completed." << endl;
		cerr << "         Make sure to call close()." << endl;
	}

	if (_playbackFile.is_open()) _playbackFile.close();
	if (_header != NULL) delete _header;
	if (_agentsInCurrentFrame != NULL) delete [] _agentsInCurrentFrame;

	_header = NULL;
	_obstacleList.clear();
	_cameraList.clear();
	_agentsInCurrentFrame = NULL;
	_frameTable.clear();
}


//
// startRecording(): initializes and writes header and some preliminary stuff.  this data will be overwritten later anyway, but is
//                   mainly used to get the file to the appropriate position for writing frames.
//
void RecFileWriter::startRecording(size_t numAgents, const std::string & filename, const std::string & testCaseName)
{
	if ( _opened ) {
		throw GenericException("RecFileWriter::startRecording(): a previous recording is already in progress.");
	}

	//
	// open the file
	//
	_playbackFile.open(filename.c_str(), ios::binary);
	if (!_playbackFile.is_open()) {
		throw GenericException("RecFileWriter::startRecording(): could not open file \"" + filename + "\".");
	}

	//
	// allocate the _header, _cameraList, _obstacleList, and _frameTable
	// note that the frameTable needs to be variable size (i.e. STL vector) because 
	// we do not know the number of frames that will be written to the file.
	//
	_header = new RecFileHeader();
	_agentsInCurrentFrame = new RecFileAgentInfo[numAgents];
	_frameTable.clear();
	_obstacleList.clear();
	_cameraList.clear();
	if ((_header == NULL) || (_agentsInCurrentFrame == NULL)) {
		throw GenericException("RecFileWriter::startRecording(): could not allocate memory for data structures.");
	}

	//
	// initialize the header and write the incomplete data up to the point of the first frame.
	// the incomplete data will be filled in properly when close() is called, when we know the number of frames that were written.
	// when reading a file, if frameTableOffset is 0, most likely that means the file was partially written and incomplete (i.e. user did not call close()).
	//
	_header->magic = RECFILE_MAGIC_NUMBER;
	_header->version = _version;
	_header->headerSize = sizeof(RecFileHeader);
	_header->frameSize = sizeof(RecFileAgentInfo) * numAgents;
	_header->numAgents = numAgents;
	_header->testCaseNameOffset = _header->headerSize;

	// write the header
	_playbackFile.write((char*)_header, _header->headerSize);

	// write the test case name associated with the recFile
	assert(_header->testCaseNameOffset == _playbackFile.tellp());
	_playbackFile.write(testCaseName.c_str(), testCaseName.length()+1);
	unsigned int numExtraBytes = 4 - ((testCaseName.length()+1) % 4);
	_playbackFile.write( "\0\0\0\0", numExtraBytes ); // pad the string to 4-byte alignment
	assert(_playbackFile.tellp()%4 == 0);


	_header->firstFrameOffset = _playbackFile.tellp();

	// the rest of the header variables are unknown until after we know the number of frames.
	_header->numFrames = 0;
	_header->numCameraViews = 0;
	_header->numObstacles = 0;
	_header->totalPlaybackTime = 0;
	_header->frameTableSize = 0;
	_header->cameraListSize = 0;
	_header->obstacleListSize = 0;
	_header->frameTableOffset = 0;
	_header->cameraListOffset = 0;
	_header->obstacleListOffset = 0;

	//
	// initialize the remaning member variables
	//
	_filename = filename;
	_opened = true;
	_writingFrame = false;

}


//
// finishRecording(): finishes writing all headers, lists, and tables to the file.
//
void RecFileWriter::finishRecording()
{
	if (!_opened) {
		throw GenericException("RecFileWriter::finishRecording(): no recording in progress to be finished.");
	}

	//
	// now we can fill in the rest of the header info
	//
	_header->numFrames = (unsigned int)_frameTable.size();      // number of frames, NOT the size in bytes
	_header->numCameraViews = (unsigned int)_cameraList.size(); // number of camera views, NOT size in bytes.
	_header->numObstacles = (unsigned int) _obstacleList.size();    // number of obstacles, NOT size in bytes
	if ( _frameTable.size() > 0 ) 
		_header->totalPlaybackTime = _frameTable[_header->numFrames-1].timeStamp - _frameTable[0].timeStamp;
	_header->frameTableSize = (unsigned int)_frameTable.size() * sizeof(RecFileFrameInfo);   // size measured in bytes
	_header->cameraListSize = (unsigned int)_cameraList.size() * sizeof(RecFileCameraInfo);  // size measured in bytes
	_header->obstacleListSize = (unsigned int) _obstacleList.size() * sizeof(RecFileObstacleInfo); // size measured in bytes

	//
	// write the camera list, obstacle list, and frame table at the end of the file
	//
	_header->cameraListOffset = _playbackFile.tellp();
	if (_header->cameraListSize != 0) _playbackFile.write((char*)(&(_cameraList[0])), _header->cameraListSize);

	_header->obstacleListOffset = _playbackFile.tellp();
	if (_header->obstacleListSize != 0) _playbackFile.write((char*)(&(_obstacleList[0])), _header->obstacleListSize);

	_header->frameTableOffset = _playbackFile.tellp();
	_playbackFile.write((char*)(&(_frameTable[0])), _header->frameTableSize);

	//
	// go back to the beginning of the file to overwrite the header with the correct info.
	//
	_playbackFile.seekp(0);
	_playbackFile.write((char*)_header, _header->headerSize);

	//
	// clean everything up.
	//
	_playbackFile.close();
	_filename = "";
	_opened = false;
	_writingFrame = false;

	if (_header != NULL) delete _header;
	if (_agentsInCurrentFrame != NULL) delete [] _agentsInCurrentFrame;

	_header = NULL;
	_obstacleList.clear();
	_cameraList.clear();
	_agentsInCurrentFrame = NULL;
	_frameTable.clear();

}


//
// startFrame(): indicates the beginning of a new frame of information.
//
void RecFileWriter::startFrame( float timeStamp, float timePassedSinceLastFrame )
{
	if (!_opened) {
		throw GenericException("RecFileWriter::startFrame(): no recording is in progress.  Make sure to use startRecording() and finishRecording() appropriately.");
	}

	if ( _writingFrame ) {
		throw GenericException("RecFileWriter::startFrame(): writing a frame is already in progress.  Make sure to call finishFrame() before starting the next frame.");
	}

	//
	// update the dtToNextFrame of the previous frame
	//
	unsigned int numFrames = (unsigned int) _frameTable.size();
	if (numFrames > 0) {
		/// @todo: need to verify that the difference between two timeStamps was somewhat close to the
		/// actual timePassedSinceLastFrame the user gave.
		/// perhaps use the following URL: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm

		// this was the old code, eventually use it to "verify" the value for timePassedSinceLastFrame
		//  float prevTimeStamp = _frameTable[numFrames-1].timeStamp;
		//  _frameTable[numFrames-1].dtToNextFrame = timeStamp - prevTimeStamp;

		// the previous frame's "dtToNextFrame" is the same as this frame's "dtToPreviousFrame" (i.e. timePassedSinceLastFrame).
		_frameTable[numFrames-1].dtToNextFrame = timePassedSinceLastFrame;
	}

	//
	// set up the frameInfo for the new frame
	//
	RecFileFrameInfo currentFrame;
	currentFrame.timeStamp = timeStamp;
	currentFrame.frameOffset = _playbackFile.tellp();
	currentFrame.dtToNextFrame = 0.0f; // unknown until the next frame is started;  for the very last frame, this remains 0.0.

	//
	// add the new entry to the frameTable
	//
	_frameTable.push_back(currentFrame);

	_writingFrame = true;

}


//
// finishFrame(): writes the current frame to disk
//
void RecFileWriter::finishFrame()
{
	if (!_opened) {
		throw GenericException("RecFileWriter::finishFrame(): no recording is in progress.  Make sure to use startRecording() and finishRecording() appropriately.");
	}

	if (!_writingFrame ) {
		throw GenericException("RecFileWriter::finishFrame(): no frame was started.");
	}

	_playbackFile.write((char*)_agentsInCurrentFrame, _header->frameSize);
	_writingFrame = false;

}


//
// setAgentInfoForCurrentFrame()
//
void RecFileWriter::setAgentInfoForCurrentFrame( unsigned int index, float posx, float posy, float posz, float dirx, float diry, float dirz, float goalx, float goaly, float goalz, float radius, bool enabled )
{
	if (!_opened) {
		throw GenericException("RecFileWriter::setAgentInfoForCurrentFrame(): no recording is in progress.  Make sure to use startRecording() and finishRecording() appropriately.");
	}

	if (!_writingFrame ) {
		throw GenericException("RecFileWriter::setAgentInfoForCurrentFrame(): no frame is in progress.  Make sure to call startFrame() and finishFrame() appropriately.");
	}

	if ( index >= _header->numAgents ) {
		throw GenericException("RecFileWriter::setAgentInfoForCurrentFrame(): index " + toString(index) + " is out of bounds; there are only " + toString(_header->numAgents) + " agents.");
	}

	_agentsInCurrentFrame[index].pos.x = posx;
	_agentsInCurrentFrame[index].pos.y = posy;
	_agentsInCurrentFrame[index].pos.z = posz;
	_agentsInCurrentFrame[index].dir.x = dirx;
	_agentsInCurrentFrame[index].dir.y = diry;
	_agentsInCurrentFrame[index].dir.z = dirz;
	_agentsInCurrentFrame[index].goal.x = goalx;
	_agentsInCurrentFrame[index].goal.y = goaly;
	_agentsInCurrentFrame[index].goal.z = goalz;
	_agentsInCurrentFrame[index].radius = radius;
	_agentsInCurrentFrame[index].enabled = enabled;

}


//
// addObstacleBoundingBox(): set the bounding box of an obstacle.
//
void RecFileWriter::addObstacleBoundingBox( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax )
{
	if (!_opened) {
		throw GenericException("RecFileWriter::addObstacleBoundingBox(): no recording is in progress.  Make sure to use startRecording() and finishRecording() appropriately.");
	}

	RecFileObstacleInfo newObstacle;
	newObstacle.bounds.xmin = xmin;
	newObstacle.bounds.xmax = xmax;
	newObstacle.bounds.ymin = ymin;
	newObstacle.bounds.ymax = ymax;
	newObstacle.bounds.zmin = zmin;
	newObstacle.bounds.zmax = zmax;
	_obstacleList.push_back(newObstacle);

}


//
// addCameraView(): set the info for one of the suggest camera views
//
void RecFileWriter::addCameraView( float origx, float origy, float origz, float lookatx, float lookaty, float lookatz )
{
	if (!_opened) {
		throw GenericException("RecFileWriter::addCameraView(): no recording is in progress.  Make sure to use startRecording() and finishRecording() appropriately.");
	}

	RecFileCameraInfo newCameraView;
	newCameraView.origin.x = origx;
	newCameraView.origin.y = origy;
	newCameraView.origin.z = origz;
	newCameraView.lookat.x = lookatx;
	newCameraView.lookat.y = lookaty;
	newCameraView.lookat.z = lookatz;
	newCameraView.up.x = 0.0f;
	newCameraView.up.y = 1.0f;
	newCameraView.up.z = 0.0f;
	_cameraList.push_back(newCameraView);

}

