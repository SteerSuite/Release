//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file TestCaseReader.cpp
/// @brief Implements the SteerLib::TestCaseReader class.

#include "testcaseio/TestCaseIO.h"
#include "util/GenericException.h"
#include "util/Misc.h"
#include "mersenne/MersenneTwister.h"
#include "griddatabase/GridDatabase2D.h"

using namespace std;
using namespace SteerLib;
using namespace Util;


TestCaseReader::TestCaseReader()
{
	_randomNumberGenerator.seed(2);
}

void TestCaseReader::readTestCaseFromFile( const std::string & testCaseFilename )
{
	_header.description = "";
	_header.name = "";
	_header.passingCriteria = "";
	_header.version = "";
	_header.worldBounds = AxisAlignedBox(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);


	//
	// first, parse the test case and get the raw data from it
	//
	ticpp::Document doc(testCaseFilename);
	doc.LoadFile();
	ticpp::Element * root = doc.FirstChildElement();
	std::string rootTagName = root->Value();

	// if the root tag doesn't match our expected root tag, its an error.
	if (rootTagName != "SteerBenchTestCase" &&
			( rootTagName != "SteerSuiteSubSpace" ) )
	{
		throw GenericException("XML file " + testCaseFilename + " does not seem to be a Valid SteerSuite test case.\n");
	}
	
	// recursively parse each tag.
	_parseTestCaseDOM( root );


	// 
	// then, initialize agents and obstacles based on the test case specs.
	//

	// create a temporary grid database used for randomly placing agents
	GridDatabase2D * testCaseDB = new GridDatabase2D(_header.worldBounds.xmin, _header.worldBounds.xmax, _header.worldBounds.zmin, _header.worldBounds.zmax, 200, 200, 10, true);
#ifdef _DEBUG
	std::cout << "num raw obstacles: " << _rawObstacles.size() << std::endl;
#endif
	// init all non-random obstacles and agents first, because they constrain where we can randomly place other objects.
	// allow the non-random obstacles to overlap, so that people can create arbitrary layouts more easily.
	for (unsigned int i=0; i<_rawObstacles.size(); i++) {

		// don't process random obstacles on the first round
		if (_rawObstacles[i]->isObstacleRandom) {
			// std::cout << "random obstacle" << std::endl;

			continue;
		}

		// init the obstacle, and add it to a temporary database
		/*ObstacleInitialConditions newObstacle;
		_initObstacleInitialConditions(newObstacle, _rawObstacles[i].obstacleBounds);*/
		ObstacleInitialConditions *newObstacle = _rawObstacles[i]->getObstacleInitialConditions();
		_initializedObstacles.push_back(newObstacle);
		testCaseDB->addObject(_rawObstacles[i], _rawObstacles[i]->obstacleBounds);
	}


	// Then add all non-random agents, making sure they don't overlap anything.
	// "non-random" is for position; random directions and goals are OK.
	for (unsigned int i=0; i<_rawAgents.size(); i++) {

		// don't process randomly placed agents on the first round
		if (_rawAgents[i].isPositionRandom) {
			continue;
		}

		// first, check if the agent overlaps anything that already exists in the database.
		/** Temporary commented out -- CORY
		float xpos = _rawAgents[i].position.x;
		float ypos = _rawAgents[i].position.y;
		float zpos = _rawAgents[i].position.z;
		float radius = _rawAgents[i].radius;
		AxisAlignedBox agentBounds = AxisAlignedBox(xpos-radius, xpos+radius, ypos-radius, ypos+radius, zpos-radius, zpos+radius);
		std::set<SpatialDatabaseItemPtr> existingObjects;
		testCaseDB->getItemsInRange(existingObjects, agentBounds.xmin, agentBounds.xmax, agentBounds.zmin, agentBounds.zmax, NULL);
		std::set<SpatialDatabaseItemPtr>::iterator existingObj;
		for (existingObj=existingObjects.begin(); existingObj != existingObjects.end(); ++existingObj) {
			if ((*existingObj)->overlaps(_rawAgents[i].position, _rawAgents[i].radius)) {
				throw GenericException("Agent \"" + _rawAgents[i].name + "\" overlaps with another object.  Probably need to fix the test case.");
			}
		}
		***/
	
		// init the agent, and add it to a temporary database
		AgentInitialConditions newAgent;
		newAgent.fromRandom = false;
		_initAgentInitialConditions(newAgent, _rawAgents[i]);
		_initializedAgents.push_back(newAgent);
		
		/** Temporary commented out -- CORY  testCaseDB->addObject(&(_rawAgents[i]), agentBounds); **/
	}

	// After non-random obstacles and agents are initialized, we can then assign the random obstacles and agents.
	// first init random obstacles

	/// TODO ASAP: Fix this for circular obstacles!
	
	/*for (unsigned int i=0; i<_rawObstacles.size(); i++) {


		if (!_rawObstacles[i].isObstacleRandom) {
			continue;  // don't process non-random obstacles on the second round
		}

		Point newPosition = testCaseDB->randomPositionInRegionWithoutCollisions( _rawObstacles[i].regionBounds, _rawObstacles[i].size, false, _randomNumberGenerator);
		float offset = _rawObstacles[i].size * 0.5f;
		_rawObstacles[i].obstacleBounds = AxisAlignedBox(newPosition.x-offset, newPosition.x+offset, 0.0f, _rawObstacles[i].height, newPosition.z-offset, newPosition.z+offset);

		// init the obstacle, and add it to a temporary database
		ObstacleInitialConditions newObstacle;
		_initObstacleInitialConditions(newObstacle, _rawObstacles[i].obstacleBounds);
		_initializedObstacles.push_back(newObstacle);
		testCaseDB->addObject(&(_rawObstacles[i]), _rawObstacles[i].obstacleBounds);
	}*/


	// Finally, add all random agents.
	for (unsigned int i=0; i<_rawAgents.size(); i++)
	{

		if (!_rawAgents[i].isPositionRandom)
		{
			continue;  // don't process non-random agents on the second round
		}
		AgentInitialConditions newAgent;
		newAgent.fromRandom = true;
		_rawAgents[i].position = testCaseDB->randomPositionInRegionWithoutCollisions( _rawAgents[i].regionBounds, _rawAgents[i].radius, false, _randomNumberGenerator);
		_rawAgents[i].isPositionRandom = false;

		float xpos = _rawAgents[i].position.x;
		float ypos = _rawAgents[i].position.y;
		float zpos = _rawAgents[i].position.z;
		float radius = _rawAgents[i].radius;
		AxisAlignedBox agentBounds = AxisAlignedBox(xpos-radius, xpos+radius, ypos-radius, ypos+radius, zpos-radius, zpos+radius);
		
		// if it doesnt overlap, then add it to the database
		_initAgentInitialConditions(newAgent, _rawAgents[i]);
		_initializedAgents.push_back(newAgent);
		testCaseDB->addObject(&(_rawAgents[i]), agentBounds);
	}

	// initial conditions are established, so the local instance of the spatial database is no longer needed after this function returns.
	delete testCaseDB;
}

