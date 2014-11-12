//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_RECFILE_IO_H__
#define __STEERLIB_RECFILE_IO_H__

/// @file RecFileIO.h
/// @brief Defines the public interfaces for reading/writing rec files.

#include "Globals.h"
#include "util/Geometry.h"
#include "recfileio/RecFileIOPrivate.h"

namespace SteerLib {

	//added by Cory, we're starting to have a lot of rec file formats running around
	enum REC_FORMAT {
		OPEN_ERROR,
		FORMAT_ERROR,
		STD_REC,	//this was put here for backwards compatibility
		FOOT_REC,
		DATA_REC,
		SHADOW_REC
	};
	/** 
	 * @brief The public interface for reading SteerSuite rec files (recordings of agents steering).
	 *

	 * Use this class to read SteerSuite rec files.
	 *
	 * <h3> How to use this class </h3>
	 *
	 * Reading rec files is straightforward:
	 *  -# Instantiate the RecFileReader class.
	 *  -# If you did not specify the filename in the constructor, call #open().
	 *  -# Use any of the functions to read information about agents, obstacles, 
	 *     camera views, or meta-data.  Data does not have to be read in any particular order.
	 *  -# When done, call #close().
	 *
	 * <h3> Notes </h3>
	 *
	 * If any information is read before calling #open(), or after calling #close(), the results are undefined.
	 * This particular error-check was omitted for performance reasons.
	 *
	 * Internally, the rec file is memory mapped, so randomly accessing data at
	 * different frames or timestamps should still perform well.
	 *
	 * There are two different sets of agent queries.  The first returns exact values for position
	 * and orientation for a given recorded frame.  The second returns interpolated values for 
	 * position and orientation for a given time stamp.
	 *
	 * Additionally, many of the queries have two forms, one using the Util geometry 
	 * data types (e.g., Util::Vector, Util::Point), and another using general floating-point passed by reference.
	 * If you want to use your own data types without passing each element by reference (which can
	 * result in annoying extra lines of less-readable code)
	 * you can create your own wrapper functions.  In most cases performance differences will be 
	 * negligible, but the general floating-point functions (passed by reference) will be slightly faster.
	 *
	 * In the current version of the rec file format, obstacles are static and cannot
	 * change over different frames.  Therefore, the parameter for frame number or time stamp in 
	 * getObstacleBoundsAtFrame() and getObstacleBoundsAtTime() are not used yet.
	 *
	 * @see 
	 *  - RecFileWriter to write rec files
	 *
	 */
	class STEERLIB_API RecFileReader : public RecFileReaderPrivate {
	public:
		/// @name Constructors and destructors
		//@{
		RecFileReader();
		/// This constructor opens the rec file, so there is no need to call open()
		RecFileReader(const std::string & filename);
		~RecFileReader();
		//@}

		/// @name Opening and closing
		//@{
		/// Returns true if the specified filename seems to be a valid rec file.
		static bool isAValidRecFile(const std::string & filename);
		/// Opens the rec file from the filename.
		void open(const std::string & filename);
		/// Closes the rec file, if one was open.
		void close();
		//@}

		/// @name Meta data queries
		//@{
		/// Returns true if a rec file is currently open in this class.
		bool isOpen() { return _opened; }
		/// Returns the version of the opened rec file, or 0 if no rec file is open.
		unsigned int getVersion() { return _version; }
		/// Returns the filename of the opened file, or an empty string if no rec file is open.
		const std::string & getFilename() { return _filename; }
		/// Returns the name of the test case associated with the rec file.
		const std::string & getTestCaseName() { return _testCaseName; }
		/// Returns the total number of frames in the rec file.
		unsigned int getNumFrames();
		/// Returns the number of agents in the rec file.
		unsigned int getNumAgents();
		/// Returns the number of obstacles in the rec file.
		unsigned int getNumObstacles();
		/// Returns the number of suggested camera views in the rec file.
		unsigned int getNumCameraViews();
		/// Returns parameters of the particular camera view indexed by cameraIndex.
		void getCameraView( unsigned int cameraIndex, float &origx, float &origy, float &origz, float &lookatx, float &lookaty, float &lookatz);
		/// Returns the time stamp for a particular frame.
		float getTimeStampForFrame( unsigned int frameNumber );
		/// Returns the total time elapsed between the first and last frames of the rec file.
		float getTotalElapsedTime();
		/// Returns the time elapsed between any two frames of the rec file.
		float getElapsedTimeBetweenFrames( unsigned int startFrame, unsigned int endFrame);
		//@}

		/// @name Agent/Obstacle queries by frame number
		/// @brief These functions give the exact data that was recorded on a particular frame.
		//@{
		/// Returns the agent location at the specified frame number.
		void getAgentLocationAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &posx, float &posy, float &posz );
		/// Returns the agent location at the specified frame number.
		inline Util::Point getAgentLocationAtFrame( unsigned int agentIndex, unsigned int frameNumber ) { Util::Point p; getAgentLocationAtFrame(agentIndex, frameNumber, p.x, p.y, p.z); return p;}

		/// Returns the agent orientation at the specified frame number
		void getAgentOrientationAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &dirx, float &diry, float &dirz );
		/// Returns the agent orientation at the specified frame number
		inline Util::Vector getAgentOrientationAtFrame( unsigned int agentIndex, unsigned int frameNumber ) { Util::Vector v; getAgentOrientationAtFrame(agentIndex, frameNumber, v.x, v.y, v.z); return v;}

		/// Returns the agent goal location at the specified frame number
		void getAgentGoalAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &goalx, float &goaly, float &goalz );
		/// Returns the agent goal location at the specified frame number
		inline Util::Point getAgentGoalAtFrame( unsigned int agentIndex, unsigned int frameNumber ) { Util::Point p; getAgentGoalAtFrame(agentIndex, frameNumber, p.x, p.y, p.z); return p;}

		/// Returns the agent radius at the specified frame number.
		float getAgentRadiusAtFrame( unsigned int agentIndex, unsigned int frameNumber );

		/// Returns true if the agent is enabled at the specified frame number.
		bool isAgentEnabledAtFrame( unsigned int agentIndex, unsigned int frameNumber );

		/// Returns the obstacle bounds of an obstacle at the specified frame number.
		void getObstacleBoundsAtFrame( unsigned int obstacleIndex, unsigned int frameNumber, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax );
		/// Returns the obstacle bounds of an obstacle at the specified frame number.
		inline Util::AxisAlignedBox getObstacleBoundsAtFrame( unsigned int obstacleIndex, unsigned int frameNumber ) { Util::AxisAlignedBox b; getObstacleBoundsAtFrame(obstacleIndex, frameNumber, b.xmin, b.xmax, b.ymin, b.ymax, b.zmin, b.zmax); return b; }
		//@}

		/// @name Agent/Obstacle queries by timestamp
		/// @brief These functions interpolate locations and orientations between frames, and they consider both surrounding frames to determine the agent's goal or enabled status appropriately.
		//@{
		/// Returns the agent location at the specified time stamp.
		void getAgentLocationAtTime( unsigned int agentIndex, float time, float &posx, float &posy, float &posz );
		/// Returns the agent location at the specified time stamp.
		inline Util::Point getAgentLocationAtTime( unsigned int agentIndex, float time ) { Util::Point p; getAgentLocationAtTime(agentIndex, time, p.x, p.y, p.z); return p;}

		/// Returns the agent orientation at the specified time stamp
		void getAgentOrientationAtTime( unsigned int agentIndex, float time, float &dirx, float &diry, float &dirz );
		/// Returns the agent orientation at the specified time stamp
		inline Util::Vector getAgentOrientationAtTime( unsigned int agentIndex, float time ) { Util::Vector v; getAgentOrientationAtTime(agentIndex, time, v.x, v.y, v.z); return v;}

		/// Returns the agent goal location at the specified time stamp
		void getAgentGoalAtTime( unsigned int agentIndex, float time, float &goalx, float &goaly, float &goalz );
		/// Returns the agent goal location at the specified time stamp
		inline Util::Point getAgentGoalAtTime( unsigned int agentIndex, float time ) { Util::Point p; getAgentGoalAtTime(agentIndex, time, p.x, p.y, p.z); return p;}

		/// Returns the agent radius at the specified time stamp.
		float getAgentRadiusAtTime( unsigned int agentIndex, float time );

		/// Returns true if the agent is enabled at the specified time stamp.
		bool isAgentEnabledAtTime( unsigned int agentIndex, float time );

		/// Returns the obstacle bounds of an obstacle at the specified time stamp.
		void getObstacleBoundsAtTime( unsigned int obstacleIndex, float time, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax );
		/// Returns the obstacle bounds of an obstacle at the specified time stamp.
		inline Util::AxisAlignedBox getObstacleBoundsAtTime( unsigned int obstacleIndex, float time ) { Util::AxisAlignedBox b; getObstacleBoundsAtTime(obstacleIndex, time, b.xmin, b.xmax, b.ymin, b.ymax, b.zmin, b.zmax); return b; }
		//@}
	};


	/** 
	 * @brief The public interface for writing %SteerSuite rec files  (recordings of agents steering).
	 *
	 * Use this class to record agents steering through a scenario, creating a %SteerSuite rec file.
	 *
	 * <h3> How to use this class </h3>
	 * Using the RecFileWriter is straightforward:
	 *   -# Call startRecording(), specifying the number of agents and the filename
	 *      of the rec file you wish to create.
	 *       - If you plan to benchmark the recording, you should specify the test case name that
	 *         is being recorded.
	 *   -# To write the next frame of the simulation, call startFrame(), and then call
	 *      setAgentInfoForCurrentFrame() for all agents, and then call finishFrame().
	 *       - <i><b>setAgentInfoForCurrentFrame() should be called for all agents, even if they are
	 *         inactive or disabled. Failing to do so may result in undefined behavior.</b></i>
	 *       - If you plan to benchmark the recording, the very first recorded frame should be the initial
	 *         conditions of the test case.
	 *   -# To finish recording the simulation, call finishRecording().
	 *
	 * <h3> Notes </h3>
	 *
	 * This class enforces correct calling sequence by throwing exceptions if functions are called at
	 * the wrong time.  For example, code cannot call startFrame() if the previous frame was not 
	 * finished with finishFrame().  It is recommended to catch these sorts of exceptions and to look at 
	 * the exception::what() message string.
	 *
	 * @see 
	 *  - SteerLib::RecFileReader to read rec files
	 *  - Util::GenericException class for an example of catching the exceptions and printing the useful error message.
	 */
	class STEERLIB_API RecFileWriter : public RecFileWriterPrivate {
	public:
		/// @name Constructors and destructors
		//@{
		RecFileWriter();
		~RecFileWriter();
		//@}

		/// @name Meta data queries
		//@{
		/// Returns the filename being written to
		const std::string & getFilename() { return _filename; }
		/// Returns the version being written
		unsigned int getVersion() { return _version; }
		/// Returns true if the RecFileWriter has a recfile open; this is true between startRecording() and finishRecording() calls.
		bool isOpen() { return _opened; }
		/// Returns true if the RecFileWriter has a recfile open; this is true between startRecording() and finishRecording() calls.
		bool isRecording() { return _opened; }
		/// Returns true if the RecFileWriter is currently writing a frame; this is true between startFrame() and finishFrame() calls.
		bool isWritingFrame() { return _writingFrame; }
		//@}

		/// @name Operations to write the rec file
		//@{
		/// Starts a new rec file to be recorded, optionally associated with a test case name; if you intend to benchmark this recording, you should provide the associated test case name.
		void startRecording(size_t numAgents, const std::string & filename, const std::string & testCaseName = "");
		/// Finishes a recording of a rec file.
		void finishRecording();
		/// Starts a new frame in the rec file to be recorded.
		void startFrame( float timeStamp, float timePassedSinceLastFrame );
		/// Finishes the current frame being recorded.
		void finishFrame();

		/// Sets the agent's info for the frame that is currently being recorded, must be called between startFrame() and finishFrame();
		void setAgentInfoForCurrentFrame( unsigned int agentIndex, float posx, float posy, float posz, float dirx, float diry, float dirz, float goalx, float goaly, float goalz, float radius, bool enabled );
		/// Sets the agent's info for the frame that is currently being recorded, must be called between startFrame() and finishFrame();
		inline void setAgentInfoForCurrentFrame( unsigned int agentIndex, const Util::Point & pos, const Util::Vector & dir, const Util::Point & goal, float radius, bool enabled ) { setAgentInfoForCurrentFrame(agentIndex, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, goal.x, goal.y, goal.z, radius, enabled); }

		/// Adds an obstacle's info to the recording.
		void addObstacleBoundingBox( float xmin, float xmax, float ymin, float ymax, float zmin, float zmax );
		/// Adds an obstacle's info to the recording.
		inline void addObstacleBoundingBox( const Util::AxisAlignedBox & bb ) { addObstacleBoundingBox(bb.xmin, bb.xmax, bb.ymin, bb.ymax, bb.zmin, bb.zmax); }

		/// Adds a suggested camera view to the recording.
		void addCameraView( float origx, float origy, float origz, float lookatx, float lookaty, float lookatz);
		/// Adds a suggested camera view to the recording.
		inline void addCameraView( const Util::Point & pos, const Util::Point & lookat ) { addCameraView( pos.x, pos.y, pos.z, lookat.x, lookat.y, lookat.z ); }
		//@}
	};


} // end namespace SteerLib

#endif
