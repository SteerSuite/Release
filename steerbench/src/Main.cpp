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

using namespace std;
using namespace Util;
using namespace SteerLib;

#define DEFAULT_BENCHMARK_TECHNIQUE "composite01"

/// Helper function to ask the benchmark engine to print metrics for all agents or a specific agent
void printCurrentMetrics(BenchmarkEngine * benchEngine, std::ostream & out, bool singleAgent, unsigned int agentIndex)
{
	if (singleAgent) {
		benchEngine->printCurrentMetrics(agentIndex, out);
	} else {
		for (unsigned int i=0; i<benchEngine->numAgents(); i++) {
			benchEngine->printCurrentMetrics(i, out);
		}
	}
}

int main(int argc, char** argv)
{
	try {
		CommandLineParser * cp = new CommandLineParser();

		// options initialized with defaults, which can be overridden by command line arguments
		std::string benchmarkTechniqueName = DEFAULT_BENCHMARK_TECHNIQUE;
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
		
		
		cp->addOption("-technique", &benchmarkTechniqueName, OPTION_DATA_TYPE_STRING);
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
			std::cout << "Benchmark technique: " << benchmarkTechniqueName << "\n";
		}

		for (unsigned int i=0; i<recFilesToBenchmark.size(); i++) {
			std::string tempFilename = std::string(recFilesToBenchmark[i]);
			if (! fileCanBeOpened( tempFilename )) {
				throw GenericException("Rec file \"" + tempFilename + "\" cannot be opened.");
			}
		}

		for (unsigned int i=0; i<recFilesToBenchmark.size(); i++) {
			std::string recFilename = std::string(recFilesToBenchmark[i]);
			if (!printNumericalScoreOnly) {
				std::cout << "Analyzing " << recFilename << "...\n";
			}

			BenchmarkTechniqueInterface * benchTechnique = createBenchmarkTechnique(benchmarkTechniqueName);

			BenchmarkEngine * benchEngine = new BenchmarkEngine(recFilename, benchTechnique);

			// If we were supposed to validate the rec files, then do that first.
			if (validateRecFile) {
				if (benchEngine->isValidTestCaseSimulation( testCaseSearchPath ) == false) {
					throw GenericException("Rec file \"" + recFilename + "\" does not match the corresponding test case.\n");
				}
			}

			// Otherwise, run the benchmark
			while (!benchEngine->isDone()) {
				benchEngine->stepOneFrame();
				if (  ((benchEngine->currentFrameNumber() == frameToDumpMetrics) && (printMetricsForFrame)  )  || (printMetricsForAllFrames)) {
					printCurrentMetrics(benchEngine, metricsOutputStream, benchmarkSingleAgent, agentToBenchmark);
				}
			}

			// print whatever the user wanted after benchmarking
			if (printMetricsForEnd) {
				printCurrentMetrics(benchEngine, metricsOutputStream, benchmarkSingleAgent, agentToBenchmark);
			}

			if (benchmarkSingleAgent) {
				if (printScoreDetails) {
					benchEngine->printAgentScoreDetails(agentToBenchmark, scoreOutputStream);
				}
				else {
					scoreOutputStream << benchEngine->getAgentBenchmarkScore(agentToBenchmark) << endl;
				}
			}
			else {
				if (printScoreDetails) {
					benchEngine->printTotalScoreDetails(scoreOutputStream);
				}
				else {
					scoreOutputStream << benchEngine->getTotalBenchmarkScore() << endl;
				}
			}
		}
	}
	catch (std::exception &e) {
		std::cerr << "\nERROR: exception caught in main:\n" << e.what() << "\n";
		exit(1);
	}

	return EXIT_SUCCESS;
}
