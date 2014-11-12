//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file steertool/src/Main.cpp
/// @brief Entry point of SteerTool.
///
/// %SteerTool is a command-line utility to perform many useful tasks to manipulate rec files, test cases, unit testing, and more.
///
/// @todo
///   - move unit-testing to a different cpp file.
///

#include "SteerLib.h"
#include "UnitTest.h"


using namespace std;
using namespace Util;
using namespace SteerLib;

int main(int argc, char** argv)
{
	try {

		std::string unitTestName = "";
		std::string validationFileName = "";
		std::string infoFileName = "";
		std::string testCaseSearchPath = "";

		std::string endianFileNames[2];
		endianFileNames[0] = "";
		endianFileNames[1] = "";

		CommandLineParser opts;
		opts.addOption("-test",     &unitTestName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-unit",     &unitTestName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-unittest", &unitTestName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-unitTest", &unitTestName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-validate", &validationFileName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-verify",   &validationFileName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-info", &infoFileName, OPTION_DATA_TYPE_STRING);
		opts.addOption("-swapendian", endianFileNames, OPTION_DATA_TYPE_STRING, 2);
		opts.addOption("-swapEndian", endianFileNames, OPTION_DATA_TYPE_STRING, 2);
		opts.addOption("-testcasepath", &testCaseSearchPath, OPTION_DATA_TYPE_STRING);
		opts.addOption("-testCasePath", &testCaseSearchPath, OPTION_DATA_TYPE_STRING);

		opts.parse(argc, argv, true, true);
		
		if (unitTestName != "") {
			runUnitTest(unitTestName);
		}
		else if (validationFileName != "") {
			throw GenericException("Validating rec files is not implemented yet.");
		}
		else if (infoFileName != "") {
			if (SteerLib::RecFileReader::isAValidRecFile(infoFileName)) {
				SteerLib::RecFileReader recFile;
				recFile.open(infoFileName);
				std::cout << "           filename: " << basename(infoFileName,"") << "\n";
				std::cout << "            version: " << recFile.getVersion() << "\n";
				std::cout << " Test case recorded: " << recFile.getTestCaseName() << "\n";
				std::cout << "    Simulation time: " << recFile.getTotalElapsedTime() << "\n";
				std::cout << "   Number of frames: " << recFile.getNumFrames() << "\n";
				std::cout << "   Number of agents: " << recFile.getNumAgents() << "\n";
				std::cout << "Number of obstacles: " << recFile.getNumObstacles() << "\n";
			}
			else if (endsWith(infoFileName, ".xml")) {
				SteerLib::TestCaseReader testCase;
				testCase.readTestCaseFromFile(infoFileName);
				std::cout << "           filename: " << basename(infoFileName,"") << "\n";
				std::cout << "            version: " << testCase.getVersion() << "\n";
				std::cout << "     Test case name: " << testCase.getTestCaseName() << "\n";
				std::cout << "        Description: " << testCase.getDescription() << "\n";
				std::cout << "   Number of agents: " << testCase.getNumAgents() << "\n";
				std::cout << "Number of obstacles: " << testCase.getNumObstacles() << "\n";
				std::cout << "      X-axis bounds: " << testCase.getWorldBounds().xmin << " to " << testCase.getWorldBounds().xmax << "\n";
				std::cout << "      Y-axis bounds: " << testCase.getWorldBounds().ymin << " to " << testCase.getWorldBounds().ymax << "\n";
				std::cout << "      Z-axis bounds: " << testCase.getWorldBounds().zmin << " to " << testCase.getWorldBounds().zmax << "\n";
			}
			else {
				throw GenericException("Specified file does not seem to be a valid rec file or test case.");
			}


		}
		else if (endianFileNames[0] != "") {
			throw GenericException("Swapping endian-ness is not implemented yet.");
		}
		else {
			throw GenericException(std::string("Please specify an action for SteerTool.\nPossible actions include:\n")
				+ std::string("    -test <testName> - performs a hard-coded unit test\n")
				+ std::string("    -validate <filename> - validates a recording against the corresponding XML test case\n")
				+ std::string("    -info <filename> - outputs human-readable information of the recording or XML test case\n")
				+ std::string("    -swapendian <inputFilename> <outputFilename> - changes the endian-ness of a rec file\n"));
		}

	}
	catch (std::exception &e) {
		std::cerr << "\nERROR: exception caught in main:\n" << e.what() << "\n";
		exit(1);
	}

	return EXIT_SUCCESS;
}


