//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

// TODO: Update the copyright?

/// @file steertrain/src/Main.cpp
/// @brief Entry point of SteerTrain.
///
/// The %SteerBench utility is essentially a command-line wrapper for the BenchmarkEngine class.
///

#include "SteerLib.h"
// #include "navmeshdatabase/NavMeshEngine.h"

#include "core/GLFWEngineDriver.h"
#include "SimulationPlugin.h"
#include "core/SteerSim.h"

using namespace std;
using namespace Util;
using namespace SteerLib;

#define DEFAULT_BENCHMARK_TECHNIQUE "composite01"

int main(int argc, char** argv)
{

	// save the original cerr streambuf, so that we can restore it on an exception.
	std::streambuf * cerrOriginalStreambuf = std::cerr.rdbuf();

	std::ofstream coutRedirection;
	std::ofstream cerrRedirection;
	std::ofstream clogRedirection;

	try {
		CommandLineParser * cp = new CommandLineParser();

		// options initialized with defaults, which can be overridden by command line arguments
		std::string navmeshName = DEFAULT_BENCHMARK_TECHNIQUE;
		unsigned int agentToBenchmark = 0;
		bool benchmarkSingleAgent = false;
		std::string testCaseSearchPath = "";
		bool validateRecFile = false;
		unsigned int frameToDumpMetrics = 0;
		bool printMetricsForEnd = false;
		bool printMetricsForFrame = false;
		bool printMetricsForAllFrames = false;
		bool printScoreDetails = false;
		bool printNumericalScoreOnly = false;
		std::vector<char*> recFilesToBenchmark;
		/// @todo add options to redirect output streams to anywhere the user requests, not only cout (default).
		std::ostream metricsOutputStream(cout.rdbuf());
		std::ostream scoreOutputStream(cout.rdbuf());
		
		
		cp->addOption("-navmesh", &navmeshName, OPTION_DATA_TYPE_STRING);
		cp->addOption("-agent", &agentToBenchmark, OPTION_DATA_TYPE_UNSIGNED_INT, 1, &benchmarkSingleAgent, true);
		cp->addOption("-testcasepath", &testCaseSearchPath, OPTION_DATA_TYPE_STRING);
		cp->addOption("-testCasePath", &testCaseSearchPath, OPTION_DATA_TYPE_STRING);
		cp->addOption("-validate", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &validateRecFile, true);
		cp->addOption("-m", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &printMetricsForEnd, true);
		cp->addOption("-fm", &frameToDumpMetrics, OPTION_DATA_TYPE_UNSIGNED_INT, 1, &printMetricsForFrame, true);
		cp->addOption("-am", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &printMetricsForAllFrames, true);
		cp->addOption("-details", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &printScoreDetails, true);
		cp->addOption("-scoreonly", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &printNumericalScoreOnly, true);
		cp->addOption("-scoreOnly", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &printNumericalScoreOnly, true);

		// the first arg will be ignored cause it is the exectuable binary itself.
		cp->parse(argc, argv, true, recFilesToBenchmark);


		if (!printNumericalScoreOnly) {
			std::cout << "mesh file: " << navmeshName << "\n";
		}

		SimulationOptions simulationOptions;
		initializeOptionsFromCommandLine( argc, argv, simulationOptions );


		//
		// after initializing from command line,
		// the simulationOptions data structure is properly initialized
		// and is the only source of options from now on.
		//


		// re-direct cout, cerr, and clog, if the user specified it through options.
		if (simulationOptions.globalOptions.coutRedirectionFilename != "") {
			coutRedirection.open(simulationOptions.globalOptions.coutRedirectionFilename.c_str());
			std::cout.rdbuf( coutRedirection.rdbuf() );
		}
		if (simulationOptions.globalOptions.cerrRedirectionFilename != "") {
			cerrRedirection.open(simulationOptions.globalOptions.cerrRedirectionFilename.c_str());
			std::cerr.rdbuf( cerrRedirection.rdbuf() );
		}
		if (simulationOptions.globalOptions.clogRedirectionFilename != "") {
			clogRedirection.open(simulationOptions.globalOptions.clogRedirectionFilename.c_str());
			std::clog.rdbuf( clogRedirection.rdbuf() );
		}



		// Run this stuff
		GLFWEngineDriver * driver = GLFWEngineDriver::getInstance();
		driver->init(&simulationOptions);
		driver->run();
		driver->finish();

	}
	catch (std::exception &e) {

		std::cerr << "\nERROR: exception caught in main:\n" << e.what() << "\n";

		// there is a chance that cerr was re-directed.  If this is true, then also echo
		// the error to the original cerr.
		if (std::cerr.rdbuf() != cerrOriginalStreambuf) {
			std::cerr.rdbuf(cerrOriginalStreambuf);
			std::cerr << "\nERROR: exception caught in main:\n" << e.what() << "\n";
		}

		if (coutRedirection.is_open()) coutRedirection.close();
		if (cerrRedirection.is_open()) cerrRedirection.close();
		if (clogRedirection.is_open()) clogRedirection.close();

		return EXIT_FAILURE;
	}

	if (coutRedirection.is_open()) coutRedirection.close();
	if (cerrRedirection.is_open()) cerrRedirection.close();
	if (clogRedirection.is_open()) clogRedirection.close();

	return EXIT_SUCCESS;
}
