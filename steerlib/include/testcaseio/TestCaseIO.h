//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_TEST_CASE_IO_H__
#define __STEERLIB_TEST_CASE_IO_H__

/// @file TestCaseIO.h
/// @brief Defines the public interfaces for reading/writing SteerSuite test cases.

#ifdef _WIN32
// see steerlib/util/DrawLib.h for explanation
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include "Globals.h"
#include "TestCaseIOPrivate.h"
#include "util/XMLParser.h"
#include "obstacles/BoxObstacle.h"
#include "interfaces/EngineInterface.h"

namespace SteerLib {

	/**
	 * @brief The public interface for reading SteerSuite XML test cases.
	 *
	 * This class reads a SteerSuite XML test case, and initializes a list of agents and obstacles to their initial conditions.
	 *
	 * <h3> How to use this class </h3>
	 *
	 * Specify the desired SteerSuite XML test case using #readTestCaseFromFile().  This class automatically parses and
	 * loads all initial conditions.  Then, the rest of the functionality of the class can be used to query information
	 * about the test case.  In particular, #getAgentInitialConditions() and #getObstacleInitialConditions() allow users
	 * to query the initial conditions of each individual agent and obstacle.
	 *
	 * Most likely your initialization will have a loop that iterates over all agents, using #getAgentInitialConditions() for
	 * each agent, and a similar loop for obstacles.  For each agent, then, you would transfer the initial conditions of
	 * our data structure into your data structures.
	 *
	 * <h3> Notes </h3>
	 *
	 * The XML test cases support the definition of agent and obstacle "regions"; these regions are automatically expanded into
	 * their individual agents in #readTestCaseFromFile().  The XML test cases also support the definition of random initial
	 * conditions, these random initial conditions are also resolved in #readTestCaseFromFile(), using the Mersenne Twister random
	 * number generator.  Goals can also be random, but are NOT resolved here, since we cannot predict run-time conditions to
	 * determine valid random goals at intialization.
	 *
	 * If the test case is large, this class may consume a large amount of memory.  It is a good idea to de-allocate it
	 * as soon as you finish initializing your own data.
	 *
	 * @see
	 *  - Documentation of AgentInitialConditions, which is the return value of #getAgentInitialConditions()
	 *  - Documentation of ObstacleInitialConditions, which is the return value of #getObstacleInitialConditions()
	 *  - Documentation of CameraView, which is the return value of #getCameraView()
	 */
	class STEERLIB_API TestCaseReader : public TestCaseReaderPrivate {
	public:
		TestCaseReader();
		/// Parses the specified XML test case; after this function returns the class contains all initialized information about the test case.
		void readTestCaseFromFile( const std::string & testCaseFilename );

		/// @name General queries about the test case
		//@{
		/// Returns the total number of agents specified by the test case.
		inline size_t getNumAgents() const { return _initializedAgents.size(); }
		/// Returns the total number of agent emitters specified by the test case.
		inline size_t getNumAgentEmitters() const { return _initializedAgentEmitters.size(); }
		/// Returns the total number of obstacles specified by the test case.
		inline size_t getNumObstacles() const { return _initializedObstacles.size(); }
		/// Returns the total number of suggested camera views specified by the test case.
		inline size_t getNumCameraViews() const { return _cameraViews.size(); }
		/// Returns the test case name (not the filename) specified by the test case.
		inline const std::string & getTestCaseName() const { return _header.name; }
		/// Returns the description specified by the test case.
		inline const std::string & getDescription() { return _header.description; }
		/// Returns a string indicating the version of this test case.
		inline const std::string & getVersion() { return _header.version; }
		/// Returns a human-readable string desciribing the criteria for passing a particular test case; In the future this criteria may become more elaborate and automated.
		inline const std::string & getPassingCriteria() { return _header.passingCriteria; }
		/// Returns a data structure containing information about one suggested camera view.
		inline const CameraView & getCameraView(unsigned int cameraIndex) { return _cameraViews[cameraIndex]; }
		/// Returns the world boundaries specified by the test case.
		inline const Util::AxisAlignedBox & getWorldBounds() const { return _header.worldBounds; }
		#ifdef VARIABLE_SPAWN_TIME
		inline const float getAgentSpawnTime(unsigned int agentIndex){return }
		#endif
		//@}

		/// @name Queries about the initial conditions of the test case.
		//@{
		/// Returns a data structure containing the agent's initial conditions for the parsed test case
		inline const AgentInitialConditions & getAgentInitialConditions(unsigned int agentIndex) const { return _initializedAgents.at(agentIndex); }
		/// Returns a data structure containing the agent emitter's initial conditions for the parsed test case
		inline const AgentInitialConditions & getAgentEmitterInitialConditions(unsigned int agentEmitterIndex) const { return _initializedAgentEmitters.at(agentEmitterIndex); }
		/// Returns a data structure containing the obstacle's initial conditions for the parsed test case
		inline const ObstacleInitialConditions * getObstacleInitialConditions(unsigned int obstacleIndex) const { return _initializedObstacles.at(obstacleIndex); }
		//@}
	};

    /**
     * @brief The public interface for writing the current scenario as a SteerSuite XML test case.
     *
     * This class is given a scenario name, a list of AgentIntialConditions, a list of obstacles,
     * and an engine. The engine is used to write out the bounds of the world only.
     * The name of the file is the  name of the scenario with ".xml" added to it.
    **/
    class STEERLIB_API TestCaseWriter {
    public:
        TestCaseWriter() ;
        /// Writes out agents, obstacles and cameraview as an xml testcase file.
        void writeTestCaseToFile(
		const std::string & testCaseName,
        	std::vector<SteerLib::AgentInitialConditions> & agents,
        	std::vector<SteerLib::BoxObstacle> & obstacles,
		SteerLib::EngineInterface *engineInfo
	) ;
        /// Writes out agents, obstacles and cameraview as an xml testcase file.
        void writeTestCaseToFile( FILE *testCaseFilePointer,
		std::vector<SteerLib::AgentInitialConditions> & agents,
                std::vector<SteerLib::BoxObstacle> & obstacles,
                SteerLib::EngineInterface *engineInfo) ;

        void writeTestCaseToFile(
        	const std::string & testCaseName,
        	std::vector<SteerLib::AgentInitialConditions> & agents,
                std::vector<SteerLib::ObstacleInitialConditions*> & obstacles,
        	SteerLib::EngineInterface *engineInfo) ;

        void writeTestCaseToFile(FILE *fp,
        	std::vector<SteerLib::AgentInitialConditions> & agents,
                std::vector<SteerLib::ObstacleInitialConditions*> & obstacles,
        	SteerLib::EngineInterface *engineInfo) ;

        void writeTestCaseToFile(
        	const std::string & testCaseName,
        	std::vector<SteerLib::AgentInitialConditions> & agents,
                std::vector<SteerLib::ObstacleInterface*> & obstacles,
        	SteerLib::EngineInterface *engineInfo) ;

        void writeTestCaseToFile(FILE *fp,
			std::vector<SteerLib::AgentInitialConditions> & agents,
		        std::vector<SteerLib::ObstacleInterface*> & obstacles,
			SteerLib::EngineInterface *engineInfo) ;

    private:
        /// The name of the testcase
        std::string _testCaseName ;
    } ;


} // end namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
