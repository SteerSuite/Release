//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#include "core/SteerSim.h"

#include <iomanip>
#include <cctype>
#include <algorithm>


using namespace std;
using namespace SteerLib;
using namespace Util;


/*
 * A duplicate of the main function. Created for calling from Python
 */
/*
const char * steersuite_init(int argc, char ** argv)
{
	// save the original cerr streambuf, so that we can restore it on an exception.
	std::streambuf * cerrOriginalStreambuf = std::cerr.rdbuf();

	const char * outData;

	std::ofstream coutRedirection;
	std::ofstream cerrRedirection;
	std::ofstream clogRedirection;

	try {
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
		//
		// allocate and use the engine driver
		//
		if (simulationOptions.globalOptions.engineDriver == "commandline") {
			std::cout << "Using command line engine Driver." << std::endl;
			CommandLineEngineDriver * cmd = new CommandLineEngineDriver();
			cmd->init(&simulationOptions);
			std::cout << "Running command line engine Driver." << std::endl;
			cmd->run();
			std::cout << "Finishing command line engine Driver." << std::endl;
			outData = cmd->getData();
			cmd->finish();
			std::cout << "finished command line engine driver" << std::endl;
		}
		else if (simulationOptions.globalOptions.engineDriver == "glfw") {
#ifdef ENABLE_GUI
#ifdef ENABLE_GLFW
			GLFWEngineDriver * driver = GLFWEngineDriver::getInstance();
			driver->init(&simulationOptions);
			driver->run();
			outData = driver->getData();
			driver->finish();
#else
			throw GenericException("GLFW functionality is not compiled into this version of SteerSim.");
#endif
#else
			throw GenericException("GUI functionality is not compiled into this version of SteerSim. Use the -commandline option.");
#endif // ifdef ENABLE_GUI
		}
		else if (simulationOptions.globalOptions.engineDriver == "qt") {
#ifdef ENABLE_GUI
#ifdef ENABLE_QT
			SteerSimQt::QtEngineDriver * driver = SteerSimQt::QtEngineDriver::getInstance();
			driver->init(options);
			driver->run();
			std::cout << "finished qt engine driver" << std::endl;
			driver->finish();
#else
			throw GenericException("Qt functionality is not compiled into this version of SteerSim.");
#endif
#else
			throw GenericException("GUI functionality is not compiled into this version of SteerSim. Use the -commandline option.");
#endif
		}
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
		char * out = (char *) malloc(sizeof(char)*20);
		strncpy(out, "EXIT_FAILURE", 20);
		return out;
	}

	if (coutRedirection.is_open()) coutRedirection.close();
	if (cerrRedirection.is_open()) cerrRedirection.close();
	if (clogRedirection.is_open()) clogRedirection.close();

	// This needs to be left available for the Python process to use this data
	return outData;
}
*/
/*
 * A duplicate of the main function. Created for calling from Python
 */
LogData * init_steersuite(int argc, char ** argv)
{
	// save the original cerr streambuf, so that we can restore it on an exception.
	std::streambuf * cerrOriginalStreambuf = std::cerr.rdbuf();

	LogData * outData = NULL;

	std::ofstream coutRedirection;
	std::ofstream cerrRedirection;
	std::ofstream clogRedirection;

	try {
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
		//
		// allocate and use the engine driver
		//
		if (simulationOptions.globalOptions.engineDriver == "commandline") {
			std::cout << "Using command line engine Driver." << std::endl;
			CommandLineEngineDriver * cmd = new CommandLineEngineDriver();
			cmd->init(&simulationOptions);
			std::cout << "Running command line engine Driver." << std::endl;
			cmd->run();
			std::cout << "Finishing command line engine Driver." << std::endl;
			outData = cmd->getLogData();
			cmd->finish();
			std::cout << "finished command line engine driver" << std::endl;
		}
		else if (simulationOptions.globalOptions.engineDriver == "glfw") {
#ifdef ENABLE_GUI
#ifdef ENABLE_GLFW
			GLFWEngineDriver * driver = GLFWEngineDriver::getInstance();
			driver->init(&simulationOptions);
			driver->run();
			outData = driver->getLogData();
			driver->finish();
#else
			throw GenericException("GLFW functionality is not compiled into this version of SteerSim.");
#endif
#else
			throw GenericException("GUI functionality is not compiled into this version of SteerSim. Use the -commandline option.");
#endif // ifdef ENABLE_GUI
		}
		else if (simulationOptions.globalOptions.engineDriver == "qt") {
#ifdef ENABLE_GUI
#ifdef ENABLE_QT
			SteerSimQt::QtEngineDriver * driver = SteerSimQt::QtEngineDriver::getInstance();
			driver->init(options);
			driver->run();
			std::cout << "finished qt engine driver" << std::endl;
			driver->finish();
#else
			throw GenericException("Qt functionality is not compiled into this version of SteerSim.");
#endif
#else
			throw GenericException("GUI functionality is not compiled into this version of SteerSim. Use the -commandline option.");
#endif
		}
	}
	catch (std::exception &e) {

		std::cerr << "\nERROR: exception caught in Main:\n" << e.what() << "\n";
		return outData;
		// there is a chance that cerr was re-directed.  If this is true, then also echo
		// the error to the original cerr.
		/*
		if (std::cerr.rdbuf() != cerrOriginalStreambuf) {
			std::cerr.rdbuf(cerrOriginalStreambuf);
			std::cerr << "\nERROR: exception caught in Main:\n" << e.what() << "\n";
		}

		if (coutRedirection.is_open()) coutRedirection.close();
		if (cerrRedirection.is_open()) cerrRedirection.close();
		if (clogRedirection.is_open()) clogRedirection.close();
		*/

		// return outData;
	}

	if (coutRedirection.is_open()) coutRedirection.close();
	if (cerrRedirection.is_open()) cerrRedirection.close();
	if (clogRedirection.is_open()) clogRedirection.close();

	// This needs to be left available for the Python process to use this data
	return outData;
}


void initializeOptionsFromCommandLine( int argc, char **argv, SimulationOptions & simulationOptions )
{
	//
	// BEFORE PARSING NORMAL COMMAND LINE: check for special-case command-line options.
	//
	//   - if a config file was specified by the command line,
	//     the config file should be loaded first, so that all other
	//     command-line options can override the config file options.
	//
	//   - if the user requested to list modules, print them out and exit.
	//
	std::string inputConfigFileName = "";
	bool listModulesAndExit = false;
	CommandLineParser specialOpts;
	specialOpts.addOption("-config", &inputConfigFileName, OPTION_DATA_TYPE_STRING);
	specialOpts.addOption("-listModules", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &listModulesAndExit, true);
	specialOpts.addOption("-listmodules", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &listModulesAndExit, true);
	specialOpts.parse(argc, argv, true, false);


	// If a config file was specified, initialize options from it.
	if (inputConfigFileName != "") {
		simulationOptions.loadOptionsFromConfigFile(inputConfigFileName);
	}


	// If user requested to list available modules, then do that and exit.
	if (listModulesAndExit) {

		// instantiate a dummy engine
		SimulationEngine * dummyEngine = new SimulationEngine();
		std::vector<std::string> moduleNames;

		dummyEngine->getListOfKnownBuiltInModules(moduleNames);
		std::cout << "Built-in modules:\n";
		for(unsigned int i=0; i<moduleNames.size(); ++i) {
			std::cout << "  " <<  moduleNames[i] << "\n";
		}

		// simulate the same semantics as the real parsing
		// above, hard-coded defaults may have been overridden by loading an input config file
		// here, command line options may override the module search path again.
		std::string searchPath = simulationOptions.engineOptions.moduleSearchPath;
		CommandLineParser opts;
		opts.addOption( "-moduleSearchPath", &searchPath, OPTION_DATA_TYPE_STRING);
		opts.addOption( "-modulesearchpath", &searchPath, OPTION_DATA_TYPE_STRING);
		opts.parse(argc, argv, true, false);


		moduleNames.clear();
		dummyEngine->getListOfKnownPlugInModules(moduleNames,searchPath);
		std::cout << "Plug-in modules:\n";
		if (moduleNames.size() == 0 ) {
			std::cout << "  <no modules found>\n";
		}
		for(unsigned int i=0; i<moduleNames.size(); ++i) {
			std::cout << "  " <<  moduleNames[i] << "\n";
		}

		delete dummyEngine;
		exit(0);
	}


	//
	// parse the command line options.
	//
	// Previously the following code only parsed some of the options,
	// and left other options to be parsed by engine drivers (or the engine itself).
	// Now this has changed, ALL command-line options must be parsed here,
	// and only a SimulationOptions data structure is passed to the engine driver.
	//
	bool qtSpecified = false;
	bool glfwSpecified = false;
	bool commandLineSpecified = false;
	std::string engineDriverName = "";
	std::string generateConfigFilename = "";

	std::string aiModuleName = "";
	std::string testCaseFilename = "";
	std::string recordingFilename = "";
	std::string frameDumpDirectory = "";
	std::string replayingFilename = "";
	std::vector<std::string> modulesGivenOnCommandLine;


	CommandLineParser opts;

	// The following several options must be "post-processed" to figure out exactly
	// what to put into the options data structure.
	opts.addOption("-Qt", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &qtSpecified, true);
	opts.addOption("-qt", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &qtSpecified, true);
	opts.addOption("-glfw", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &glfwSpecified, true);
	opts.addOption("-GLFW", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &glfwSpecified, true);
	opts.addOption("-commandLine", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &commandLineSpecified, true);
	opts.addOption("-commandline", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &commandLineSpecified, true);
	opts.addOption("-engineDriver", &engineDriverName, OPTION_DATA_TYPE_STRING);
	opts.addOption("-enginedriver", &engineDriverName, OPTION_DATA_TYPE_STRING);
	opts.addOption("-generateConfig", &generateConfigFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption("-generateconfig", &generateConfigFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-module", &modulesGivenOnCommandLine, OPTION_DATA_TYPE_MULTI_INSTANCE_STRING);
	opts.addOption( "-testfile", &testCaseFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-testFile", &testCaseFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-testcase", &testCaseFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-testCase", &testCaseFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-ai", &aiModuleName, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-storeSimulation", &recordingFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-storesimulation", &recordingFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-playback", &replayingFilename, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-replay", &replayingFilename, OPTION_DATA_TYPE_STRING);

	// The following several options can initialize the options data structure directly.
	opts.addOption( "-numFrames", &simulationOptions.engineOptions.numFramesToSimulate, OPTION_DATA_TYPE_UNSIGNED_INT);
	opts.addOption( "-numframes", &simulationOptions.engineOptions.numFramesToSimulate, OPTION_DATA_TYPE_UNSIGNED_INT);
	opts.addOption( "-numThreads", &simulationOptions.engineOptions.numThreads, OPTION_DATA_TYPE_UNSIGNED_INT);
	opts.addOption( "-numthreads", &simulationOptions.engineOptions.numThreads, OPTION_DATA_TYPE_UNSIGNED_INT);
	opts.addOption( "-testCaseSearchPath", &simulationOptions.engineOptions.testCaseSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-testcasesearchpath", &simulationOptions.engineOptions.testCaseSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-testCasePath", &simulationOptions.engineOptions.testCaseSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-testcasepath", &simulationOptions.engineOptions.testCaseSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-moduleSearchPath", &simulationOptions.engineOptions.moduleSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-modulesearchpath", &simulationOptions.engineOptions.moduleSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-modulePath", &simulationOptions.engineOptions.moduleSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-modulepath", &simulationOptions.engineOptions.moduleSearchPath, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-startPaused", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &simulationOptions.glfwEngineDriverOptions.pausedOnStart, true);
	opts.addOption( "-startpaused", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &simulationOptions.glfwEngineDriverOptions.pausedOnStart, true);
	opts.addOption( "-saveFramesTo", &simulationOptions.engineOptions.frameDumpDirectory , OPTION_DATA_TYPE_STRING);
	opts.addOption( "-saveframesto", &simulationOptions.engineOptions.frameDumpDirectory, OPTION_DATA_TYPE_STRING);
	opts.addOption( "-blendingDemo", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &simulationOptions.globalOptions.blendingDemo, true);
	opts.addOption( "-parameterDemo", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &simulationOptions.globalOptions.parameterDemo, true);
	opts.addOption( "-noTweakBar", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &simulationOptions.globalOptions.noTweakBar, true);
	opts.addOption( "-dataFileName", &simulationOptions.globalOptions.dataFileName, OPTION_DATA_TYPE_STRING);

	// Dummy option parsing for the special options, but these are used earlier and ignored at this point.
	opts.addOption("-config", NULL, OPTION_DATA_TYPE_STRING);

	// This will parse the command line and initialize all the above variables as appropriate.
	opts.parse(argc, argv, true, true);



	//
	// NEXT: many of these command-line options need to be error-checked and "interpreted"
	//   to initialize the simulationOptions data structure
	//



	// figure out which engine the user specified on the command line, or use the default.
	// this code might seem "inflated", but actually it is an attempt to have user-friendly semantics
	// for several different ways of specifying the engine driver on the command line.

	if ( engineDriverName != "" ) {
		// convert the user's command-line input into lower case for the engine driver
		engineDriverName = Util::toLower(engineDriverName);

		if (engineDriverName == "qt") qtSpecified = true;
		else if (engineDriverName == "glfw") glfwSpecified = true;
		else if (engineDriverName == "commandline") commandLineSpecified = true;
	}

	unsigned int numGUIOptionsSpecified = 0;

	//
	// NOTE CAREFULLY!!!! The following MUST BE SEPARATE if statements, they MUST NOT be if-else statements.
	//
	if (qtSpecified) {
		numGUIOptionsSpecified++;
		engineDriverName = "qt";
	}

	if (glfwSpecified) {
		numGUIOptionsSpecified++;
		engineDriverName = "glfw";
	}

	if (commandLineSpecified) {
		numGUIOptionsSpecified++;
		engineDriverName = "commandline";
	}

	if (numGUIOptionsSpecified > 1) {
		throw GenericException("Multiple engine drivers were specified:"
			+ toString(commandLineSpecified ? " commandLine" : "")
			+ toString(glfwSpecified ? " glfw" : "")
			+ toString(qtSpecified ? " qt" : "")
			+ "; Please specify only one engine driver.");
	}

	// finally, after all that, if there is an engine driver name, then set the option, otherwise,
	// leave the hard-coded default.
	if (engineDriverName != "") {
		simulationOptions.globalOptions.engineDriver = engineDriverName;
	}
	else {
		// in this case, leave the simulationOptions.globalOptions.engineDriver
		// with the hard coded default.
	}



	//
	// take care of some shortcut options; these will be overridden by module options if the user specified conflicting options in both places.
	//
	if (testCaseFilename != "") {
		simulationOptions.moduleOptionsDatabase["testCasePlayer"]["testcase"] = testCaseFilename;
		simulationOptions.engineOptions.startupModules.insert("testCasePlayer");
		simulationOptions.engineOptions.startupModules.erase("recFilePlayer"); // removes the recFilePlayer module so that the overriding command-line option doesn't cause a conflict with default options.
	}

	if (aiModuleName != "") {
		simulationOptions.moduleOptionsDatabase["testCasePlayer"]["ai"] = aiModuleName.substr(0,aiModuleName.find(','));
		modulesGivenOnCommandLine.push_back(aiModuleName);
		simulationOptions.engineOptions.startupModules.insert("testCasePlayer");
		simulationOptions.engineOptions.startupModules.erase("recFilePlayer"); // removes the recFilePlayer module so that the overriding command-line option doesn't cause a conflict with default options.
	}

	if (replayingFilename != "") {
		simulationOptions.moduleOptionsDatabase["recFilePlayer"]["recfile"] = replayingFilename;
		simulationOptions.engineOptions.startupModules.insert("recFilePlayer");
		simulationOptions.engineOptions.startupModules.erase("testCasePlayer");  // removes the testCasePlayer module so that the overriding command-line option doesn't cause a conflict with default options.
	}

	if (recordingFilename != "") {
		simulationOptions.moduleOptionsDatabase["simulationRecorder"]["recfile"] = recordingFilename;
		simulationOptions.engineOptions.startupModules.insert("simulationRecorder");
	}

	if (frameDumpDirectory != "")
	{
		simulationOptions.moduleOptionsDatabase["frameDumpDirectory"]["dir"] = frameDumpDirectory;
		// simulationOptions.engineOptions.startupModules.insert("simulationRecorder");
	}


	//separate the modules names and possible options
	for (unsigned int i=0; i<modulesGivenOnCommandLine.size(); i++) {
		// First, split the command-line between the module name and the string of options.
		string::size_type firstComma = modulesGivenOnCommandLine[i].find(',');
		std::string moduleName = modulesGivenOnCommandLine[i].substr(0, firstComma);
		std::string moduleOptions = "";
		if (firstComma != string::npos) {
			moduleOptions = modulesGivenOnCommandLine[i].substr(firstComma+1, string::npos);
		}
		else {
			// no comma was found, so there are no options.
			moduleOptions = "";
		}

		// Next, add these options to the options database.
		simulationOptions.mergeModuleOptions(moduleName, moduleOptions);

		// add it to the list of startup modules; its a set so will only exist once, anyway.
		simulationOptions.engineOptions.startupModules.insert(moduleName);
	}




	// If user requested to store a config file, do that and exit.
	if (generateConfigFilename != "") {
		simulationOptions.generateConfigFile(generateConfigFilename);
		exit(0);
	}

}

/*
PLUGIN_API	const char * init_steersim(int argc, char ** argv)
{
	return steersuite_init(argc, argv);
}
*/
PLUGIN_API	LogData * init_steersim2(int argc, char ** argv)
{
	return init_steersuite(argc, argv);
}

