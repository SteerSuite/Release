//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_RECFILE_IO_PRIVATE_H__
#define __STEERLIB_RECFILE_IO_PRIVATE_H__

/// @file RecFileIOPrivate.h
/// @brief Defines the private functionality for reading/writing rec files.

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fstream>
#include <vector>
#include "Globals.h"
#include "util/MemoryMapper.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

//
// ---------------------------------
// FEATURES of version 1 recfile:
//
// user can load/store the following:
//  - position, orientation, and goal location of any agent at any valid timestamp.
//  - bounding box of static obstacles.
//  - a list of camera views
//  - various global information such as num agents, num frames, total elapsed time of simulation, etc.
//
// internally, it stores 5 major sections:
//  - header
//  - list of camera views
//  - list of static obstacles
//  - all the playback data for each frame.
//  - frame table, giving basic info for each frame, including the location in the file of that particular frame
//
// ---------------------------------
// FEATURES of version 2 recfile:
//
//  - all the same features as version 1, but in-between the header and first frame exists an 
//    extra nul-terminated string that represents the test case filename (may be empty).
//
// ---------------------------------
//

namespace SteerLib {

	/// The "magic number" placed at the beginning of every rec file; used to identify rec files and to check big-endian/little-endian issues.
	const unsigned int RECFILE_MAGIC_NUMBER   = 0x0f8c2951;


	/**
	 * @brief The header data contained in the very beginning of a rec file.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileHeader {
		/// A unique number to helps identify a binary %SteerSuite rec file, and to detect big-endian/little-endian issues.
		unsigned int magic;
		/// Integer number representing the version of the rec file.
		unsigned int version;

		/// Size in bytes of this data structure.
		unsigned int headerSize;
		/// Size in bytes of all suggested camera info.
		unsigned int cameraListSize;
		/// Size in bytes of all obstacle info.
		unsigned int obstacleListSize;
		/// Size in bytes of all agents info, for a single frame.
		unsigned int frameSize;
		/// Size in bytes of the frame table; not known until all frames have been written.
		unsigned int frameTableSize;

		/// Number of suggested camera views in the rec file.
		unsigned int numCameraViews;
		/// Number of obstacles in the rec file
		unsigned int numObstacles;
		/// Number of agents in the rec file
		unsigned int numAgents;
		/// Number of total frames in the rec file; not known until all frames have been written.
		unsigned int numFrames;
		/// Total time elapsed in the recording; not known until all frames have been written.
		float totalPlaybackTime;

		/// Offset in bytes from the beginning of the file, where the test case string (possibly empty) is located;  if the offset is 0, no test case name was provided.
		unsigned int testCaseNameOffset;
		/// Offset in bytes from the beginning of the file, where the array of camera info is listed.
		unsigned int cameraListOffset;
		/// Offset in bytes from the beginning of the file, where the array of camera views is located.
		unsigned int obstacleListOffset;
		/// Offset in bytes from the beginning of the file, where the frame table is located; not known until all frames have been written.
		unsigned int frameTableOffset;
		/// Offset in bytes from the beginning of the file, where the first frame is located.
		unsigned int firstFrameOffset;
	};

	/**
	 * @brief A point data structure used for reading/writing rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFilePointData {
		float x, y, z;
	};

	/**
	 * @brief A vector data structure used for reading/writing rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileVectorData {
		float x, y, z;
	};

	/**
	 * @brief A axis-aligned box data structure used for reading/writing rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileBoxData {
		float xmin, xmax, ymin, ymax, zmin, zmax;
	};

	/**
	 * @brief The data recorded for each static obstacle, used for reading/writing static obstacles in rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileObstacleInfo {
		RecFileBoxData bounds;  // bounding box of the obstacle.
	};

	/**
	 * @brief A data structure used for reading/writing camera info in rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileCameraInfo {
		/// The position of the camera.
		RecFilePointData origin;
		/// The lookat location, that the camera points towards.
		RecFilePointData lookat;
		/// The up orientation of the camera.
		RecFileVectorData up;
	};

	/**
	 * @brief An entry of the frame table, used for reading/writing rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileFrameInfo {
		/// The time stamp of the frame associated with this frame table entry.
		float timeStamp;
		/// The time between this frame and the next frame.
		float dtToNextFrame;
		/// The offset in bytes from the beginning of the file, where the frame associated with this frame table entry is located.
		unsigned int frameOffset;
	};

	/**
	 * @brief The data recorded for each agent for each frame, used for reading/writing rec files.
	 *
	 * <b>This struct is used for file IO, so do not reorder items.</b>
	 */
	struct RecFileAgentInfo {
		/// True indicates the agent is active (i.e. data in this struct are valid) for a given frame.  Typically starts true, and "flips" to false only once, and stays false to the end of the recording.
		bool enabled;
		/// Position of the agent for the corresponding frame.
		RecFilePointData pos;
		/// Orientation of the agent for the corresponding frame.
		RecFileVectorData dir;
		/// Location of the agent's goal for the corresponding frame.
		RecFilePointData goal;
		/// The radius of the agent, typically should remain constant for all frames recorded.
		float radius;
	};



	/** 
	 * @brief The protected data and member functions used by the RecFileReader class.
	 *
	 * This class should not be used directly.  Instead, use the RecFileReader public interface that
	 * inherits from this class.
	 */
	class STEERLIB_API RecFileReaderPrivate {
	protected:
		/// Protected constructor enforces that users cannot publically instantiate this class.
		RecFileReaderPrivate() { }

		void _getFramesForTime(float time, unsigned int &frameIndex1, unsigned int &frameIndex2);

		std::string _filename;
		std::string _testCaseName;
		unsigned int _version;
		bool _opened;

		Util::MemoryMapper _fileMap;
		RecFileHeader * _header;
		RecFileObstacleInfo * _obstacleList;
		RecFileCameraInfo * _cameraList;
		RecFileFrameInfo * _frameTable;
		RecFileAgentInfo ** _frames;

		unsigned int f1_used_in_getFramesForTimeFunction, f2_used_in_getFramesForTimeFunction;
		float prevTime_used_in_getFramesForTimeFunction;
	};



	/**
	 * @brief The protected data and member functions used by the RecFileWriter class.
	 *
	 * This class should not be used directly.  Instead, use the RecFileWriter public interface that
	 * inherits from this class.
	 */
	class STEERLIB_API RecFileWriterPrivate {
	protected:
		/// Protected constructor enforces that users cannot publically instantiate this class.
		RecFileWriterPrivate() { }

		std::string _filename;
		unsigned int _version;
		bool _opened;
		bool _writingFrame;

		std::ofstream _playbackFile;
		RecFileHeader * _header;
		std::vector<RecFileObstacleInfo> _obstacleList;
		std::vector<RecFileCameraInfo> _cameraList;
		RecFileAgentInfo * _agentsInCurrentFrame;
		std::vector<RecFileFrameInfo> _frameTable;
	};


} // end namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif

