//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file TestCaseWriter.cpp
/// @brief Implements the SteerLib::TestCaseWriter class
///

#include "util/GenericException.h"
#include "util/Misc.h"
#include "mersenne/MersenneTwister.h"
#include "griddatabase/GridDatabase2D.h"

#include "SteerLib.h"

using namespace std;
using namespace SteerLib;
using namespace Util;

TestCaseWriter::TestCaseWriter()
{
    _testCaseName = "" ;
    return ;
}


void TestCaseWriter::writeTestCaseToFile(
	const std::string & testCaseName, 
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::BoxObstacle> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	std::string testCaseFilename = testCaseName + ".xml" ;
    FILE * fp = fopen(testCaseFilename.c_str(), "w") ;
    if( !fp ) {
        fprintf(stderr, "TestCaseWriter::writeTestCaseToFile: ERROR: Cannot open file %s\n", testCaseFilename.c_str()) ;
        return ;
    }

    _testCaseName = testCaseName ;
    writeTestCaseToFile(fp, agents, obstacles,engineInfo) ;
    fclose(fp) ;
    return ;
}

void TestCaseWriter::writeTestCaseToFile(
	const std::string & testCaseName,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::ObstacleInitialConditions*> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	std::string testCaseFilename = testCaseName + ".xml" ;
    FILE * fp = fopen(testCaseFilename.c_str(), "w") ;
    if( !fp ) {
        fprintf(stderr, "TestCaseWriter::writeTestCaseToFile: ERROR: Cannot open file %s\n", testCaseFilename.c_str()) ;
        return ;
    }

    _testCaseName = testCaseName ;
    writeTestCaseToFile(fp, agents, obstacles,engineInfo) ;
    fclose(fp) ;
    return ;
}

void TestCaseWriter::writeTestCaseToFile(FILE *fp,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::ObstacleInitialConditions*> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	if( engineInfo == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR engine is null\n") ;
	}
	SteerLib::GridDatabase2D * grid = engineInfo->getSpatialDatabase() ;
	if( grid == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR no grid database\n") ;
	}

	/// Write header
	fprintf(fp, "<!-- \n Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman\n See license.txt for complete license.\n  -->\n") ;


	fprintf(fp, "<SteerBenchTestCase xmlns=\"http://www.magix.ucla.edu/steerbench\"\n"
                    "\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                    "\t\txsi:schemaLocation=\"http://www.magix.ucla.edu/steerbench\n"
                    "\t\t\t\tTestCaseSchema.xsd\">") ;

	fprintf(fp,"<header>\n\t<version>1.0</version>\n") ;
	fprintf(fp,"\t<name>%s</name>\n", _testCaseName.c_str()) ;

	/// write the world bounds
	float originX = grid->getOriginX() ;
	float originZ = grid->getOriginZ() ;
	float sizeX = grid->getOriginX() ;
	float sizeZ = grid->getOriginZ() ;
	float xmin = originX ;
	float zmin = originZ ;
	float xmax = originX + sizeX ;
	float zmax = originZ + sizeZ ;
	fprintf(fp,"\t<worldBounds>\n") ;
	fprintf(fp,"\t\t<xmin>%f</xmin>\n", xmin) ;
	fprintf(fp,"\t\t<xmax>%f</xmax>\n", xmax) ;

	fprintf(fp,"\t\t<ymin>%f</ymin>\n", 0.0f) ; // @TODO FIXME
	fprintf(fp,"\t\t<ymax>%f</ymax>\n", 0.0f) ; // @TODO FIXME

	fprintf(fp,"\t\t<zmin>%f</zmin>\n", zmin) ;
	fprintf(fp,"\t\t<zmax>%f</zmax>\n", zmax) ;
	fprintf(fp,"\t</worldBounds>\n") ;

	fprintf(fp,"</header>\n") ;

	/// write the obstacles
	std::vector<SteerLib::ObstacleInitialConditions> ::iterator iter ;
	for( unsigned int i = 0 ; i < obstacles.size() ;  i++)
	{

		if (typeid(*obstacles[i]) == typeid(SteerLib::BoxObstacleInitialConditions))
		{
			const Util::AxisAlignedBox & box = obstacles[i]->createObstacle()->getBounds() ;
			fprintf(fp,"\t<obstacle>\n") ;
			fprintf(fp,"\t\t<xmin>%f</xmin>\n", box.xmin) ;
			fprintf(fp,"\t\t<xmax>%f</xmax>\n", box.xmax) ;
			fprintf(fp,"\t\t<ymin>%f</ymin>\n", box.ymin) ;
			fprintf(fp,"\t\t<ymax>%f</ymax>\n", box.ymax) ;
			fprintf(fp,"\t\t<zmin>%f</zmin>\n", box.zmin) ;
			fprintf(fp,"\t\t<zmax>%f</zmax>\n", box.zmax) ;
			fprintf(fp,"\t</obstacle>\n") ;
		}
		else if (typeid(*obstacles[i]) == typeid(SteerLib::CircleObstacleInitialConditions))
		{
			assert(false && "No support for writing this kind of obstacle");
		}
		else
		{
			assert(false && "No support for writing this kind of obstacle");
		}

	}

	/// write agents
	for( unsigned int i = 0 ; i < agents.size() ; i++)
	{
		SteerLib::AgentInitialConditions *ic = &agents[i] ;

		fprintf(fp,"\t<agent>\n") ;
		fprintf(fp,"\t<name>%s</name>\n", ic->name.c_str()) ;
		fprintf(fp,"<initialConditions>\n") ;
		fprintf(fp,"\t\t<radius>%f</radius>\n", ic->radius) ;
		fprintf(fp,"\t\t<position> <x>%f</x> <y>%f</y> <z>%f</z> </position>\n",
			ic->position[0],ic->position[1],ic->position[2]) ;
		fprintf(fp,"\t\t<direction> <x>%f</x> <y>%f</y> <z>%f</z> </direction>\n",
			ic->direction[0],ic->direction[1],ic->direction[2]) ;
		fprintf(fp,"\t\t<speed>%f</speed>\n", ic->speed) ;
		fprintf(fp,"\t</initialConditions>\n") ;

		#ifdef _DEBUG
		std::cout << "agent desired speed " << agents[i].goals[0].desiredSpeed << "\n";
		#endif
		/// write goals of each agent
		fprintf(fp,"\t<goalSequence>\n") ;
		for( unsigned int g = 0 ; g <  agents[i].goals.size() ; g++)
		{
			std::string goalType = "seekStaticTarget" ;
			switch ( agents[i].goals[g].goalType){
				case GOAL_TYPE_SEEK_STATIC_TARGET:
					goalType = "seekStaticTarget" ;
					break ;
				case GOAL_TYPE_FLEE_STATIC_TARGET:
					goalType = "fleeStaticTarget" ;
					break ;
				case GOAL_TYPE_SEEK_DYNAMIC_TARGET:
					goalType = "seekDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLEE_DYNAMIC_TARGET:
					goalType = "fleeDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLOW_STATIC_DIRECTION:
					goalType = "flowStaticDirection" ;
					break ;
				case GOAL_TYPE_FLOW_DYNAMIC_DIRECTION:
					goalType = "flowDynamicDirection" ;
					break ;
				case GOAL_TYPE_IDLE:
					goalType = "idle" ;
				default: break ;
			}
			fprintf(fp,"\t\t<%s>\n", goalType.c_str()) ;
			fprintf(fp,"\t\t\t<targetLocation> <x>%f</x> <y>%f</y> <z>%f</z> </targetLocation>\n",
			agents[i].goals[g].targetLocation[0],agents[i].goals[g].targetLocation[1],agents[i].goals[g].targetLocation[2]) ;
			fprintf(fp,"\t\t\t<desiredSpeed>%f</desiredSpeed>\n", agents[i].goals[g].desiredSpeed) ;
			fprintf(fp,"\t\t\t<timeDuration>%f</timeDuration>\n",  agents[i].goals[g].timeDuration) ;

			//if ( !goal->targetName.empty() )
			//	fprintf(fp,"\t\t\t<targetName>%s</targetName>\n", goal->targetName.c_str()) ;

			fprintf(fp,"\t\t\t<targetDirection> <x>%f</x> <y>%f</y> <z>%f</z> </targetDirection>\n",
			agents[i].goals[g].targetDirection[0],agents[i].goals[g].targetDirection[1],agents[i].goals[g].targetDirection[2]) ;
			fprintf(fp,"\t\t\t<flowType>%s</flowType>\n", agents[i].goals[g].flowType.c_str()) ;
			std::string isRandom = "false" ;
			if( agents[i].goals[g].targetIsRandom ) isRandom = "true" ;
			fprintf(fp,"\t\t\t<random>%s</random>\n", isRandom.c_str()) ;

			if (agents[i].goals[g].targetBehaviour.getSteeringAlg() != "" )
			{
				// std::cout << "Found a Behavior while writing testcase" << std::endl;
				fprintf(fp,"\t\t\t<Behaviour>\n") ;
					fprintf(fp,"\t\t\t\t<SteeringAlgorithm>%s</SteeringAlgorithm>\n",
							agents[i].goals[g].targetBehaviour.getSteeringAlg().c_str()) ;

					if ( agents[i].goals[g].targetBehaviour.getParameters().size() > 0)
					{
						fprintf(fp,"\t\t\t\t<Parameters>\n") ;
						int p;
						for (p=0; p<agents[i].goals[g].targetBehaviour.getParameters().size(); p++ )
						{
							fprintf(fp,"\t\t\t\t\t<parameter>\n") ;
								fprintf(fp,"\t\t\t\t\t\t<key>%s</key>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).key.c_str()) ;
								fprintf(fp,"\t\t\t\t\t\t<value>%s</value>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).value.c_str()) ;
							fprintf(fp,"\t\t\t\t\t</parameter>\n") ;
						}
						fprintf(fp,"\t\t\t\t</Parameters>\n") ;
					}
				fprintf(fp,"\t\t\t</Behaviour>\n") ;
			}

			fprintf(fp,"\t\t</%s>\n", goalType.c_str()) ;
		}
		fprintf(fp,"\t</goalSequence>\n") ;
		fprintf(fp,"</agent>\n") ;
	}
		fprintf(fp,"</SteerBenchTestCase>\n") ;

}

void TestCaseWriter::writeTestCaseToFile(FILE *fp,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::BoxObstacle> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	if( engineInfo == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR engine is null\n") ;
	}
	SteerLib::GridDatabase2D * grid = engineInfo->getSpatialDatabase() ;
	if( grid == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR no grid database\n") ;
	}	

	/// Write header
	fprintf(fp, "<!-- \n Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman\n See license.txt for complete license.\n  -->\n") ;
	

	fprintf(fp, "<SteerBenchTestCase xmlns=\"http://www.magix.ucla.edu/steerbench\"\n"
                    "\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                    "\t\txsi:schemaLocation=\"http://www.magix.ucla.edu/steerbench\n"
                    "\t\t\t\tTestCaseSchema.xsd\">") ;

	fprintf(fp,"<header>\n\t<version>1.0</version>\n") ;
	fprintf(fp,"\t<name>%s</name>\n", _testCaseName.c_str()) ;
	
	/// write the world bounds
	float originX = grid->getOriginX() ;
	float originZ = grid->getOriginZ() ;
	float sizeX = grid->getOriginX() ;
	float sizeZ = grid->getOriginZ() ;
	float xmin = originX ;
	float zmin = originZ ;
	float xmax = originX + sizeX ;
	float zmax = originZ + sizeZ ;
	fprintf(fp,"\t<worldBounds>\n") ;
	fprintf(fp,"\t\t<xmin>%f</xmin>\n", xmin) ;
	fprintf(fp,"\t\t<xmax>%f</xmax>\n", xmax) ;

	fprintf(fp,"\t\t<ymin>%f</ymin>\n", 0.0f) ; // @TODO FIXME
	fprintf(fp,"\t\t<ymax>%f</ymax>\n", 0.0f) ; // @TODO FIXME

	fprintf(fp,"\t\t<zmin>%f</zmin>\n", zmin) ;
	fprintf(fp,"\t\t<zmax>%f</zmax>\n", zmax) ;
	fprintf(fp,"\t</worldBounds>\n") ;

	fprintf(fp,"</header>\n") ;
	
	/// write the obstacles
	std::vector<SteerLib::BoxObstacle> ::iterator iter ;
	for( unsigned int i = 0 ; i < obstacles.size() ;  i++)
	{
		const Util::AxisAlignedBox & box = obstacles[i].getBounds() ;
		fprintf(fp,"\t<obstacle>\n") ;
		fprintf(fp,"\t\t<xmin>%f</xmin>\n", box.xmin) ;
		fprintf(fp,"\t\t<xmax>%f</xmax>\n", box.xmax) ;
		fprintf(fp,"\t\t<ymin>%f</ymin>\n", box.ymin) ;
		fprintf(fp,"\t\t<ymax>%f</ymax>\n", box.ymax) ;
		fprintf(fp,"\t\t<zmin>%f</zmin>\n", box.zmin) ;
		fprintf(fp,"\t\t<zmax>%f</zmax>\n", box.zmax) ;
		fprintf(fp,"\t</obstacle>\n") ;
	}
	
	/// write agents
	for( unsigned int i = 0 ; i < agents.size() ; i++)
	{
		SteerLib::AgentInitialConditions *ic = &agents[i] ;

		fprintf(fp,"\t<agent>\n") ;
		fprintf(fp,"\t<name>%s</name>\n", ic->name.c_str()) ;
		fprintf(fp,"<initialConditions>\n") ;
		fprintf(fp,"\t\t<radius>%f</radius>\n", ic->radius) ;
		fprintf(fp,"\t\t<position> <x>%f</x> <y>%f</y> <z>%f</z> </position>\n", 
			ic->position[0],ic->position[1],ic->position[2]) ;
		fprintf(fp,"\t\t<direction> <x>%f</x> <y>%f</y> <z>%f</z> </direction>\n", 
			ic->direction[0],ic->direction[1],ic->direction[2]) ;
		fprintf(fp,"\t\t<speed>%f</speed>\n", ic->speed) ;
		fprintf(fp,"\t</initialConditions>\n") ;
		
		#ifdef _DEBUG
		std::cout << "agent desired speed " << agents[i].goals[0].desiredSpeed << "\n";
		#endif
		/// write goals of each agent
		fprintf(fp,"\t<goalSequence>\n") ;
		for( unsigned int g = 0 ; g <  agents[i].goals.size() ; g++)
		{
			std::string goalType = "seekStaticTarget" ;
			switch ( agents[i].goals[g].goalType){
				case GOAL_TYPE_SEEK_STATIC_TARGET:
					goalType = "seekStaticTarget" ;
					break ;
				case GOAL_TYPE_FLEE_STATIC_TARGET:
					goalType = "fleeStaticTarget" ;
					break ;
				case GOAL_TYPE_SEEK_DYNAMIC_TARGET:
					goalType = "seekDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLEE_DYNAMIC_TARGET:
					goalType = "fleeDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLOW_STATIC_DIRECTION:
					goalType = "flowStaticDirection" ;
					break ;
				case GOAL_TYPE_FLOW_DYNAMIC_DIRECTION:
					goalType = "flowDynamicDirection" ;
					break ;
				case GOAL_TYPE_IDLE:
					goalType = "idle" ;
				default: break ;
			}
			fprintf(fp,"\t\t<%s>\n", goalType.c_str()) ;
			fprintf(fp,"\t\t\t<targetLocation> <x>%f</x> <y>%f</y> <z>%f</z> </targetLocation>\n", 
			agents[i].goals[g].targetLocation[0],agents[i].goals[g].targetLocation[1],agents[i].goals[g].targetLocation[2]) ;
			fprintf(fp,"\t\t\t<desiredSpeed>%f</desiredSpeed>\n", agents[i].goals[g].desiredSpeed) ;
			fprintf(fp,"\t\t\t<timeDuration>%f</timeDuration>\n",  agents[i].goals[g].timeDuration) ;
			
			//if ( !goal->targetName.empty() )
			//	fprintf(fp,"\t\t\t<targetName>%s</targetName>\n", goal->targetName.c_str()) ;
			
			fprintf(fp,"\t\t\t<targetDirection> <x>%f</x> <y>%f</y> <z>%f</z> </targetDirection>\n", 
			agents[i].goals[g].targetDirection[0],agents[i].goals[g].targetDirection[1],agents[i].goals[g].targetDirection[2]) ;
			fprintf(fp,"\t\t\t<flowType>%s</flowType>\n", agents[i].goals[g].flowType.c_str()) ;
			std::string isRandom = "false" ;
			if( agents[i].goals[g].targetIsRandom ) isRandom = "true" ;	
			fprintf(fp,"\t\t\t<random>%s</random>\n", isRandom.c_str()) ;

			if (agents[i].goals[g].targetBehaviour.getSteeringAlg() != "" )
			{
				// std::cout << "Found a Behavior while writing testcase" << std::endl;
				fprintf(fp,"\t\t\t<Behaviour>\n") ;
					fprintf(fp,"\t\t\t\t<SteeringAlgorithm>%s</SteeringAlgorithm>\n",
							agents[i].goals[g].targetBehaviour.getSteeringAlg().c_str()) ;

					if ( agents[i].goals[g].targetBehaviour.getParameters().size() > 0)
					{
						fprintf(fp,"\t\t\t\t<Parameters>\n") ;
						int p;
						for (p=0; p<agents[i].goals[g].targetBehaviour.getParameters().size(); p++ )
						{
							fprintf(fp,"\t\t\t\t\t<parameter>\n") ;
								fprintf(fp,"\t\t\t\t\t\t<key>%s</key>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).key.c_str()) ;
								fprintf(fp,"\t\t\t\t\t\t<value>%s</value>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).value.c_str()) ;
							fprintf(fp,"\t\t\t\t\t</parameter>\n") ;
						}
						fprintf(fp,"\t\t\t\t</Parameters>\n") ;
					}
				fprintf(fp,"\t\t\t</Behaviour>\n") ;
			}

			fprintf(fp,"\t\t</%s>\n", goalType.c_str()) ;
		}
		fprintf(fp,"\t</goalSequence>\n") ;
		fprintf(fp,"</agent>\n") ;
	}
		fprintf(fp,"</SteerBenchTestCase>\n") ;
	    
}

// std::string BehaviorToXML

