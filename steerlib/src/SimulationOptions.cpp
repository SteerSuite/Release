//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file SimulationOptions.cpp
/// @brief Implements SimulationOptions functionality, and defines defaults for options that are not specified by the user.
///
/// @todo
///  - update documentation in this file.
///  - remove dependency on the glfw.h file
///


//====================================
//
// * STOP! *
//
// * READ THIS! *
//
/// @file SimulationOptions.cpp
///
/// <b>If you want to change defaults for yourself, generate and edit an xml config file, don't change this file.</b>
///
/// If you really must change the hard-coded defaults, this is the file to edit.
///
/// To add an option, do the following:
///  -# Add a default-value macro in this .cpp file, which will be the hard-coded default value.
///  -# Add a variable inside the SimulationOptions class, probably inside one of the existing structs.
///  -# Initialize the hard-coded value in the constructor
///  -# Add code in _setupXMLStructure() that defines the option for XML parsing/generating.
///
//====================================

#include <fstream>
#include <algorithm>
#include "simulation/SimulationOptions.h"
#include "util/Misc.h"
#include "util/GenericException.h"


#include "glfw/include/GL/glfw.h"

using namespace SteerLib;
using namespace Util;


//====================================
// KEYBOARD-ACTION MAPPINGS
//====================================

// GUI ACTIONS
#define DEFAULT_KEY_QUIT                  GLFW_KEY_ESC
#define DEFAULT_KEY_PRINT_CAMERA_INFO     'P'
#define DEFAULT_KEY_TOGGLE_ANTIALIASING   'A'
#define DEFAULT_KEY_TAKE_SCREENSHOT       GLFW_KEY_F12
#define DEFAULT_START_DUMPING_FRAMES      GLFW_KEY_HOME
#define DEFAULT_STOP_DUMPING_FRAMES       GLFW_KEY_END
// ENGINE ACTIONS
#define DEFAULT_KEY_PAUSE                 GLFW_KEY_SPACE
#define DEFAULT_KEY_STEP_ONE_FRAME        GLFW_KEY_RIGHT
// REC FILE PLAYER MODULE ACTIONS
#define DEFAULT_KEY_SPEEDUP_PLAYBACK      '='
#define DEFAULT_KEY_SLOWDOWN_PLAYBACK     '-'
#define DEFAULT_KEY_RESET_PLAYBACK_SPEED  GLFW_KEY_BACKSPACE
#define DEFAULT_KEY_STEP_BACKWARD         GLFW_KEY_LEFT
#define DEFAULT_KEY_RESTART_PLAYBACK      'R'
#define DEFAULT_KEY_DUMP_TESTCASE      	  'T'

//====================================
// MOUSE-ACTION MAPPINGS
//====================================
#define DEFAULT_MOUSE_BUTTON_TO_SELECT_AGENT   GLFW_MOUSE_BUTTON_LEFT
#define DEFAULT_MOUSE_BUTTON_TO_MOVE_CAMERA    GLFW_MOUSE_BUTTON_MIDDLE
#define DEFAULT_MOUSE_BUTTON_TO_ROTATE_CAMERA  GLFW_MOUSE_BUTTON_LEFT
#define DEFAULT_MOUSE_BUTTON_TO_ZOOM_CAMERA    GLFW_MOUSE_BUTTON_RIGHT

//====================================
// GUI DEFAULTS
//====================================
#define DEFAULT_ANTIALIASING true
#define DEFAULT_VSYNC false
#define DEFAULT_CAN_USE_MOUSE_SELECTION true
#define DEFAULT_USE_MOUSE_WHEEL_ZOOM true
#define DEFAULT_MOUSE_ROTATE_FACTOR .007f
#define DEFAULT_MOUSE_ZOOM_FACTOR .0075f
#define DEFAULT_MOUSE_MOVE_FACTOR .001f
#define DEFAULT_BACKGROUND_COLOR  Color(0.5f, 0.5f, 0.28f) // seems useless right now
#define DEFAULT_LINE_WIDTH 3.0f
#define DEFAULT_CAMERA_POSITION   Point(0.0f, 37.0f, 40)
#define DEFAULT_CAMERA_LOOKAT     Point(0.0f, 0.0f,  -5)
#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
#define DEFAULT_CAMERA_FOVY       45

//====================================
// MISC. CAMERA VIEWS (copy-paste and 
// uncomment only one of these to the 
// GUI defaults)
//====================================
// initial generic camera view
//#define DEFAULT_CAMERA_POSITION   Point(0.0f, 37.0f, -40)
//#define DEFAULT_CAMERA_LOOKAT     Point(0.0f, 0.0f,  -5)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// initial zoom-in for curves scenario
//#define DEFAULT_CAMERA_POSITION   Point(3.35f, 10.5f, -0.11)
//#define DEFAULT_CAMERA_LOOKAT     Point(3.35f, 0.0f,  -0.10)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// initial zoom-in for cross scenario
//#define DEFAULT_CAMERA_POSITION   Point(1.2f, 15.5f, 3.43)
//#define DEFAULT_CAMERA_LOOKAT     Point(1.2f, 0.0f,  3.44)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// initial zoom-in for 4-way-obstacle
//#define DEFAULT_CAMERA_POSITION   Point(-0.45f, 26.0f, -0.15f)
//#define DEFAULT_CAMERA_LOOKAT     Point(-0.45f, 0f,    -0.13f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// new zoom-in for 4-way obstacle
//#define DEFAULT_CAMERA_POSITION   Point(9.21589f, 8.08322f, -1.93683f)
//#define DEFAULT_CAMERA_LOOKAT     Point(-0.45f, 0.0f, -0.13f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// for frogger zoomed in
//#define DEFAULT_CAMERA_POSITION   Point(-0.900609f, 7.41979f, -10.5049f)
//#define DEFAULT_CAMERA_LOOKAT     Point(-0.900609f, 0.0f, 0.940365f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// for wall-squeeze
//#define DEFAULT_CAMERA_POSITION   Point(6.64612f, 9.94769f, 3.77235f)
//#define DEFAULT_CAMERA_LOOKAT     Point(0.573495f, 0.0f, -1.51921f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)

//====================================
// ENGINE DEFAULTS
//====================================
#ifdef _WIN32
#define DEFAULT_MODULE_SEARCH_PATH "./"
#define DEFAULT_TEST_CASE_SEARCH_PATH "../../../testcases/"
#else
#define DEFAULT_MODULE_SEARCH_PATH "../lib/"
#define DEFAULT_TEST_CASE_SEARCH_PATH "../../testcases/"
#endif

#define DEFAULT_ENGINE_DRIVER "glfw"
#define DEFAULT_COUT_REDIRECTION_FILENAME ""
#define DEFAULT_CERR_REDIRECTION_FILENAME ""
#define DEFAULT_CLOG_REDIRECTION_FILENAME ""
#define DEFAULT_DATA_FILE ""
#define DEFAULT_NUM_THREADS 1
#define DEFAULT_NUM_FRAMES_TO_SIMULATE 0
#define DEFAULT_FIXED_FPS 20.0f
#define DEFAULT_MIN_VARIABLE_DT 0.001f
#define DEFAULT_MAX_VARIABLE_DT 0.2f
#define DEFAULT_CLOCK_MODE "fixed-fast"

//====================================
// SPATIAL DATABASE DEFAULTS
//====================================
#define DEFAULT_USE_DATABASE "gridDatabase"

//====================================
// GRID DATABASE DEFAULTS
//====================================
#define DEFAULT_MAX_ITEMS_PER_GRID_CELL 7
#define DEFAULT_GRID_SIZE_X 200.0f
#define DEFAULT_GRID_SIZE_Z 200.0f
#define DEFAULT_NUM_GRID_CELLS_X 200
#define DEFAULT_NUM_GRID_CELLS_Z 200
#define DEFAULT_DRAW_GRID true

//====================================
// Planning Domain DEFAULTS
//====================================
#define DEFAULT_USE_PLANNER "gridDomain"
#define DEFAULT_MAX_NODES_TO_EXPAND 50000


//====================================
// GLFW ENGINE DRIVER DEFAULTS
//====================================
#define DEFAULT_CLOCK_PAUSED_ON_START false
#define DEFAULT_WINDOW_SIZE_X 1024
#define DEFAULT_WINDOW_SIZE_Y 768
#define DEFAULT_WINDOW_POSITION_X 0
#define DEFAULT_WINDOW_POSITION_Y 0
#define DEFAULT_WINDOW_TITLE "SteerSim"
#define DEFAULT_FULLSCREEN false
#define DEFAULT_STEREO_MODE "off"

//====================================
// BUILT-IN MODULES DEFAULTS
//====================================
#define DEFAULT_TEST_CASE_PLAYER_FILENAME "simple-1.xml"
#define DEFAULT_TEST_CASE_PLAYER_AI "dummyAI"





//====================================


SimulationOptions::SimulationOptions()
{
	//
	// initialize ALL hard-coded default values, these values will be
	// overridden if they are specified in the config file.
	//

	// keyboard config
	keyboardBindings.quit = DEFAULT_KEY_QUIT;
	keyboardBindings.printCameraInfo = DEFAULT_KEY_PRINT_CAMERA_INFO;
	keyboardBindings.toggleAntialiasing = DEFAULT_KEY_TOGGLE_ANTIALIASING;
	keyboardBindings.takeScreenshot = DEFAULT_KEY_TAKE_SCREENSHOT;
	keyboardBindings.startDumpingFrames = DEFAULT_START_DUMPING_FRAMES;
	keyboardBindings.stopDumpingFrames = DEFAULT_STOP_DUMPING_FRAMES;
	keyboardBindings.pause = DEFAULT_KEY_PAUSE;
	keyboardBindings.stepForward = DEFAULT_KEY_STEP_ONE_FRAME;
	keyboardBindings.stepBackward = DEFAULT_KEY_STEP_BACKWARD;
	keyboardBindings.speedupPlayback = DEFAULT_KEY_SPEEDUP_PLAYBACK;
	keyboardBindings.slowdownPlayback = DEFAULT_KEY_SLOWDOWN_PLAYBACK;
	keyboardBindings.resetPlaybackSpeed = DEFAULT_KEY_RESET_PLAYBACK_SPEED;
	keyboardBindings.restartPlayback = DEFAULT_KEY_RESTART_PLAYBACK;
	keyboardBindings.dumpTestCase = DEFAULT_KEY_DUMP_TESTCASE;

	// mouse config
	mouseBindings.selectAgent = DEFAULT_MOUSE_BUTTON_TO_SELECT_AGENT;
	mouseBindings.moveCamera = DEFAULT_MOUSE_BUTTON_TO_MOVE_CAMERA;
	mouseBindings.rotateCamera = DEFAULT_MOUSE_BUTTON_TO_ROTATE_CAMERA;
	mouseBindings.zoomCamera = DEFAULT_MOUSE_BUTTON_TO_ZOOM_CAMERA;

	// global options
	globalOptions.engineDriver = DEFAULT_ENGINE_DRIVER;
	globalOptions.coutRedirectionFilename = DEFAULT_COUT_REDIRECTION_FILENAME;
	globalOptions.cerrRedirectionFilename = DEFAULT_CERR_REDIRECTION_FILENAME;
	globalOptions.clogRedirectionFilename = DEFAULT_CLOG_REDIRECTION_FILENAME;
	globalOptions.blendingDemo = false;
	globalOptions.parameterDemo = false;
	globalOptions.noTweakBar = false;
	globalOptions.dataFileName = DEFAULT_DATA_FILE;

	// engine options
	engineOptions.moduleSearchPath = DEFAULT_MODULE_SEARCH_PATH;
	engineOptions.testCaseSearchPath = DEFAULT_TEST_CASE_SEARCH_PATH;
	engineOptions.startupModules.clear();
	engineOptions.numThreads = DEFAULT_NUM_THREADS;
	engineOptions.numFramesToSimulate = DEFAULT_NUM_FRAMES_TO_SIMULATE;
	engineOptions.fixedFPS = DEFAULT_FIXED_FPS;
	engineOptions.minVariableDt = DEFAULT_MIN_VARIABLE_DT;
	engineOptions.maxVariableDt = DEFAULT_MAX_VARIABLE_DT;
	engineOptions.clockMode = DEFAULT_CLOCK_MODE;

	spatialDatabaseOptions.name = DEFAULT_USE_DATABASE;

	// grid database options
	gridDatabaseOptions.maxItemsPerGridCell = DEFAULT_MAX_ITEMS_PER_GRID_CELL;
	gridDatabaseOptions.gridSizeX = DEFAULT_GRID_SIZE_X;
	gridDatabaseOptions.gridSizeZ = DEFAULT_GRID_SIZE_Z;
	gridDatabaseOptions.numGridCellsX = DEFAULT_NUM_GRID_CELLS_X;
	gridDatabaseOptions.numGridCellsZ = DEFAULT_NUM_GRID_CELLS_Z;
	gridDatabaseOptions.drawGrid = DEFAULT_DRAW_GRID;

	// Planning Domain options
	planningDomainOptions.name = DEFAULT_USE_PLANNER;
	planningDomainOptions.maxNodesToExpand = DEFAULT_MAX_NODES_TO_EXPAND;

	// GUI options
	guiOptions.useAntialiasing = DEFAULT_ANTIALIASING;
	guiOptions.useVsync = DEFAULT_VSYNC;
	guiOptions.mouseRotationFactor = DEFAULT_MOUSE_ROTATE_FACTOR;
	guiOptions.mouseZoomFactor = DEFAULT_MOUSE_ZOOM_FACTOR;
	guiOptions.mouseMovementFactor = DEFAULT_MOUSE_MOVE_FACTOR;
	guiOptions.canUseMouseSelection = DEFAULT_CAN_USE_MOUSE_SELECTION;
	guiOptions.canUseMouseWheelZoom = DEFAULT_USE_MOUSE_WHEEL_ZOOM;
	guiOptions.cameraPosition = DEFAULT_CAMERA_POSITION;
	guiOptions.cameraLookAt = DEFAULT_CAMERA_LOOKAT;
	guiOptions.cameraUp = DEFAULT_CAMERA_UP;
	guiOptions.cameraFovy = DEFAULT_CAMERA_FOVY;
	guiOptions.backgroundColor = DEFAULT_BACKGROUND_COLOR;
	guiOptions.lineWidth = DEFAULT_LINE_WIDTH;

	// glfw engine driver options
	glfwEngineDriverOptions.pausedOnStart = DEFAULT_CLOCK_PAUSED_ON_START;
	glfwEngineDriverOptions.windowSizeX = DEFAULT_WINDOW_SIZE_X;
	glfwEngineDriverOptions.windowSizeY = DEFAULT_WINDOW_SIZE_Y;
	glfwEngineDriverOptions.windowPositionX = DEFAULT_WINDOW_POSITION_X;
	glfwEngineDriverOptions.windowPositionY = DEFAULT_WINDOW_POSITION_Y;
	glfwEngineDriverOptions.windowTitle = DEFAULT_WINDOW_TITLE;
	glfwEngineDriverOptions.fullscreen = DEFAULT_FULLSCREEN;
	glfwEngineDriverOptions.stereoMode = DEFAULT_STEREO_MODE;

	//
	// module options
	// for each module, initialize its module options, and insert that into the module options database.
	// NOTE CAREFULLY: module names are CASE-SENSITIVE!!!
	//
	OptionDictionary modOpts;
	moduleOptionsDatabase.clear();

	modOpts.clear();
	modOpts["testcase"] = DEFAULT_TEST_CASE_PLAYER_FILENAME;
	modOpts["ai"] = DEFAULT_TEST_CASE_PLAYER_AI;
	moduleOptionsDatabase["testCasePlayer"] = modOpts;
	/*
	modOpts.clear();
	moduleOptionsDatabase["recFilePlayer"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["simpleAI"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["dummyAI"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["simulationRecorder"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["metricsCollector"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["steerBench"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["steerBug"] = modOpts;
	*/

	_moduleOptionsXMLParser.init(&moduleOptionsDatabase);
	_startupModulesXMLParser.init(&engineOptions.startupModules);


	/*
	std::cout << "HARD-CODED DEFAULTS:\n";
	for (ModuleOptionsDatabase::iterator iter = moduleOptionsDatabase.begin(); iter != moduleOptionsDatabase.end(); ++iter) {
		std::cout << "module \"" << (*iter).first << "\"\n";
		for (OptionDictionary::iterator k = (*iter).second.begin(); k != (*iter).second.end(); ++k) {
			std::cout << "    " << (*k).first << " --> " << (*k).second << "\n";
		}
		std::cout << "\n";
	}
	*/

}

//
// loadOptionsFromConfigFile() - any values specified in the config file will override the original hard-coded defaults.
//
void SimulationOptions::loadOptionsFromConfigFile( const std::string & filename )
{
	XMLParser xmlOpts;
	_setupXMLStructure(xmlOpts);

	if (fileCanBeOpened(filename)) {
		xmlOpts.parseXMLFile(filename, true);
	}
	else {
		throw GenericException("Cannot find the specified config file \"" + filename + "\".");
	}

	/*
	// debugging output, keep it here for now
	std::cout << "CONFIG-FILE DEFAULTS:\n";
	for (ModuleOptionsDatabase::iterator iter = moduleOptionsDatabase.begin(); iter != moduleOptionsDatabase.end(); ++iter) {
		std::cout << "module \"" << (*iter).first << "\"\n";
		for (OptionDictionary::iterator k = (*iter).second.begin(); k != (*iter).second.end(); ++k) {
			std::cout << "    " << (*k).first << " --> " << (*k).second << "\n";
		}
		std::cout << "\n";
	}
	*/


	//
	// do error checking and option conversions next, try to be user-friendly where possible.
	//

	/// @todo add some path-cleanup and error checking here in SimulationOptions.cpp.

	// transform clockMode option to lower case
	engineOptions.clockMode = Util::toLower(engineOptions.clockMode);

	if ((engineOptions.clockMode != "fixed-fast") && (engineOptions.clockMode != "fixed-real-time") && (engineOptions.clockMode != "variable-real-time")) {
		std::cerr << "WARNING: Bad option value for clockMode in configuration file.\n         Valid options are: \"fixed-fast\", \"fixed-real-time\" or \"variable-real-time\".\n         For now, setting default to \"fixed-fast\", which may be overridden by command-line or GUI.";
		engineOptions.clockMode = "fixed-fast";
	}

	glfwEngineDriverOptions.stereoMode = Util::toLower(glfwEngineDriverOptions.stereoMode);
	if ((glfwEngineDriverOptions.stereoMode != "off") &&
		(glfwEngineDriverOptions.stereoMode != "side-by-side") &&
		(glfwEngineDriverOptions.stereoMode != "top-and-bottom") &&
		(glfwEngineDriverOptions.stereoMode != "quadbuffer"))
	{
		std::cerr << "WARNING: Bad option value for stereoMode in configuration file.\n         Valid options are: \"off\", \"side-by-side\", \"top-and-bottom\", or \"quadbuffer\".\n         For now, setting default to \"" << DEFAULT_STEREO_MODE <<  "\", which may be overridden by command-line or GUI.";

		glfwEngineDriverOptions.stereoMode = DEFAULT_STEREO_MODE;
	}
}


void SimulationOptions::generateConfigFile( const std::string & filename )
{
	XMLParser xmlOpts;
	_setupXMLStructure(xmlOpts);

	if (fileCanBeOpened(filename)) {
		throw GenericException("Cannot generate config file \"" + filename + "\", file already exists.");
	}

	xmlOpts.writeXMLFile(filename);

	std::cout << "Default configuration written to " << filename << ".\n";
}

/**
 *
 * Creating these tags defines the expected structure of the XML document
 * Works really fast and easy.
 */
void SimulationOptions::_setupXMLStructure( Util::XMLParser & xmlOpts )
{
	XMLTag * root = xmlOpts.createRootTag("SteerSimOptions", "This file contains options for SteerSim.  Edit this file to your preference, and\nthen use the '-config' command line option to load these options in SteerSim.\nOptions specified by the command line will override options in this configuration file.");

	// primary option groups
	root->createChildTag("keyboard", "Maps various actions to keyboard input (config for keybaord not implemented yet!)");
	root->createChildTag("mouse", "Maps various actions to mouse input (config for mouse not implemented yet!)");
	XMLTag * guiTag = root->createChildTag("gui", "Options related to the openGL visualization and interaction.  Also, make sure to look at the engine driver options for more interface-related options.");
	XMLTag * globalTag = root->createChildTag("global", "Options related to the main execution of the steersim");
	XMLTag * engineTag = root->createChildTag("engine", "Options related to the simulation engine");
	XMLTag * spatialDatabaseTag = root->createChildTag("spatialDatabase", "Options related to the spatial database");
	XMLTag * planningDomainTag = root->createChildTag("planningDomain", "Options related to the planning domain");
	XMLTag * engineDriversTag = root->createChildTag("engineDrivers", "Options related to engine drivers");
	root->createChildTag("modules", "Module-specific options.  Any options specified on the command-line will override the options specified here.  Modules specified here will not necessarily be loaded when started; for that use the startupModules option for the engine.", XML_DATA_TYPE_CONTAINER, NULL, &_moduleOptionsXMLParser );

	// option sub-groups
	engineDriversTag->createChildTag("commandLine", "Options for the command-line engine driver (currently there are no options for the command-line)");
	XMLTag * glfwEngineDriverTag = engineDriversTag->createChildTag("glfw", "Options for the GLFW engine driver");
	engineDriversTag->createChildTag("qt", "Options for the Qt engine driver (config for qt not implemented yet!)");

	// GUI options
	guiTag->createChildTag("useAntialiasing", "Set to \"true\" to remove jaggies, for smoother-looking visuals, but lower performance", XML_DATA_TYPE_BOOLEAN, &guiOptions.useAntialiasing);
	guiTag->createChildTag("useShadows", "DO NOT USE THIS VALUE.  It is only kept here for backwards compatibility.", XML_DATA_TYPE_BOOLEAN, &_dummyUseShadowsFlag);
	guiTag->createChildTag("useVsync", "Set to \"false\" for higher performance that is not synchronizeded with the display's refresh rate", XML_DATA_TYPE_BOOLEAN, &guiOptions.useVsync);
	guiTag->createChildTag("mouseRotationFactor", "Scaling factor for sensitivity of camera rotation when using mouse.", XML_DATA_TYPE_FLOAT, &guiOptions.mouseRotationFactor);
	guiTag->createChildTag("mouseZoomFactor", "Scaling factor for sensitivity of camera zoom when using mouse", XML_DATA_TYPE_FLOAT, &guiOptions.mouseZoomFactor);
	guiTag->createChildTag("mouseMovementFactor", "Scaling factor for sensitivity of camera movement when using mouse", XML_DATA_TYPE_FLOAT, &guiOptions.mouseMovementFactor);
	guiTag->createChildTag("canUseMouseSelection", "Set to \"true\" to be able to select agents with the mouse, \"false\" is recommended when using many many agents, because selection algorithm is brute-force and slow", XML_DATA_TYPE_BOOLEAN, &guiOptions.canUseMouseSelection);
	guiTag->createChildTag("canUseMouseWheelZoom", "Set to \"true\" to be able to zoom with the mouse wheel; this does not disable other possible ways to zoom the camera.", XML_DATA_TYPE_BOOLEAN, &guiOptions.canUseMouseWheelZoom);
	guiTag->createChildTag("cameraPosition", "Camera's physical position in the 3-D scene", XML_DATA_TYPE_XYZ, &guiOptions.cameraPosition);
	guiTag->createChildTag("cameraLookAt", "The 3-D point the camera will look at", XML_DATA_TYPE_XYZ, &guiOptions.cameraLookAt);
	guiTag->createChildTag("cameraUp", "The vector that represnts the upright orientation for the camera", XML_DATA_TYPE_XYZ, &guiOptions.cameraUp);
	guiTag->createChildTag("cameraVerticalFieldOfView", "The vertical field of view of the camera, in degrees", XML_DATA_TYPE_FLOAT, &guiOptions.cameraFovy);
	guiTag->createChildTag("backgroundColor", "The background color of the openGL visualization", XML_DATA_TYPE_RGB, &guiOptions.backgroundColor);
	guiTag->createChildTag("lineWidth", "width of lines drawn in the GUI", XML_DATA_TYPE_FLOAT, &guiOptions.lineWidth);

	// global options
	globalTag->createChildTag("engineDriver", "The name of the engine driver to use, if not specified from command line", XML_DATA_TYPE_STRING, &globalOptions.engineDriver);
	globalTag->createChildTag("redirectCoutToFile", "If a filename is specified, std::cout will be redirected to that filename.  NOTE: Only std::cout will be redirected; low-level and C-style output will not be redirected.", XML_DATA_TYPE_STRING, &globalOptions.coutRedirectionFilename);
	globalTag->createChildTag("redirectCerrToFile", "If a filename is specified, std::cerr will be redirected to that filename.  NOTE: Only std::cerr will be redirected; low-level and C-style output will not be redirected.  Exceptions will be caught and redirected to both the new and the original std::cerr output.", XML_DATA_TYPE_STRING, &globalOptions.cerrRedirectionFilename);
	globalTag->createChildTag("redirectClogToFile", "If a filename is specified, std::clog will be redirected to that filename.  NOTE: Only std::clog will be redirected; low-level and C-style output will not be redirected.", XML_DATA_TYPE_STRING, &globalOptions.clogRedirectionFilename);

	// engine options
	engineTag->createChildTag("moduleSearchPath","The default directory to search for dynamic plug-in modules at runtime.", XML_DATA_TYPE_STRING, &engineOptions.moduleSearchPath);
	engineTag->createChildTag("testCaseSearchPath","The default directory to search for test cases at runtime.", XML_DATA_TYPE_STRING, &engineOptions.testCaseSearchPath);
	engineTag->createChildTag("startupModules", "The list of modules to use on startup.  Modules specified by the command line will be merged with this list.", XML_DATA_TYPE_CONTAINER, NULL, &_startupModulesXMLParser);
	engineTag->createChildTag("numThreads", "The default number of threads to run on the simulation", XML_DATA_TYPE_UNSIGNED_INT, &engineOptions.numThreads);
	engineTag->createChildTag("numFrames", "The default number of frames to simulate - 0 means run the entire simulation until all agents are disabled.", XML_DATA_TYPE_UNSIGNED_INT, &engineOptions.numFramesToSimulate);
	engineTag->createChildTag("fixedFPS", "The fixed frames-per-second for the simulation clock.  This value is used when simulationClockMode is \"fixed-fast\" or \"fixed-real-time\".", XML_DATA_TYPE_FLOAT, &engineOptions.fixedFPS);
	engineTag->createChildTag("minVariableDt", "The minimum time-step allowed when the clock is in \"variable-real-time\" mode.  If the proposed time-step is smaller, this value will be used instead, effectively limiting the max frame rate.", XML_DATA_TYPE_FLOAT, &engineOptions.minVariableDt);
	engineTag->createChildTag("maxVariableDt", "The maximum time-step allowed when the clock is in \"variable-real-time\" mode.  If the proposed time-step is larger, this value will be used instead, at the expense of breaking synchronization between simulation time and real-time.", XML_DATA_TYPE_FLOAT, &engineOptions.maxVariableDt);
	engineTag->createChildTag("clockMode", "can be either \"fixed-fast\" (fixed simulation frame rate, running as fast as possible), \"fixed-real-time\" (fixed simulation frame rate, running in real-time), or \"variable-real-time\" (variable simulation frame rate in real-time).", XML_DATA_TYPE_STRING, &engineOptions.clockMode);

	// spatial database stuff
	spatialDatabaseTag->createChildTag("useDatabase", "Option to select the database type to use , ", XML_DATA_TYPE_STRING, &spatialDatabaseOptions.name);
	XMLTag * gridDatabaseTag = spatialDatabaseTag->createChildTag("gridDatabase", "Options related to the grid database");
	spatialDatabaseTag->createChildTag("navmeshDatabase", "Options related to the navmesh database");

	// planning domain stuff
	planningDomainTag->createChildTag("planner", "Options selects which planning tool to use during simulation", XML_DATA_TYPE_STRING, &planningDomainOptions.name);
	XMLTag * planningDomainSettingsTag = planningDomainTag->createChildTag("domainSettings", "Options related to the grid database");
	planningDomainSettingsTag->createChildTag("maxNodesToExpand", "Options informs planner to the max number of nodes to expand in search", XML_DATA_TYPE_UNSIGNED_INT, &planningDomainOptions.maxNodesToExpand);

	// grid database options
	gridDatabaseTag->createChildTag("maxItemsPerGridCell", "Max number of items a grid cell can contain", XML_DATA_TYPE_UNSIGNED_INT, &gridDatabaseOptions.maxItemsPerGridCell);
	gridDatabaseTag->createChildTag("sizeX", "Total size of the grid along the X axis", XML_DATA_TYPE_FLOAT, &gridDatabaseOptions.gridSizeX);
	gridDatabaseTag->createChildTag("sizeZ", "Total size of the grid along the Z axis", XML_DATA_TYPE_FLOAT, &gridDatabaseOptions.gridSizeZ);
	gridDatabaseTag->createChildTag("numCellsX", "Number of cells in the grid along the X axis", XML_DATA_TYPE_UNSIGNED_INT, &gridDatabaseOptions.numGridCellsX);
	gridDatabaseTag->createChildTag("numCellsZ", "Number of cells in the grid along the Z axis", XML_DATA_TYPE_UNSIGNED_INT, &gridDatabaseOptions.numGridCellsZ);
	gridDatabaseTag->createChildTag("draw", "Draws the grid if \"true\".", XML_DATA_TYPE_BOOLEAN, &gridDatabaseOptions.drawGrid);

	// GLFW engine driver options
	glfwEngineDriverTag->createChildTag("startWithClockPaused", "Starts the clock paused if \"true\".", XML_DATA_TYPE_BOOLEAN, &glfwEngineDriverOptions.pausedOnStart);
	glfwEngineDriverTag->createChildTag("windowSizeX", "Width of the openGL window in pixels", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowSizeX);
	glfwEngineDriverTag->createChildTag("windowSizeY", "Height of the openGL window in pixels", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowSizeY);
	glfwEngineDriverTag->createChildTag("windowPositionX", "Position of the openGL window in x", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowPositionX);
	glfwEngineDriverTag->createChildTag("windowPositionY", "Position of the openGL window in y", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowPositionY);
	glfwEngineDriverTag->createChildTag("windowTitle", "Title of the openGL window", XML_DATA_TYPE_STRING, &glfwEngineDriverOptions.windowTitle);
	glfwEngineDriverTag->createChildTag("fullscreen", "Uses fullscreen (rather than windowed) mode if \"true\".", XML_DATA_TYPE_BOOLEAN, &glfwEngineDriverOptions.fullscreen);
	glfwEngineDriverTag->createChildTag("stereoMode", "The stereoscopic mode. Can be one of \"off\", \"side-by-side\", \"top-and-bottom\", or \"quadbuffer\".", XML_DATA_TYPE_STRING, &glfwEngineDriverOptions.stereoMode);
}


//========================================
const SteerLib::OptionDictionary & SimulationOptions::getModuleOptions(const std::string & moduleName)
{
	ModuleOptionsDatabase::iterator optionsIter = moduleOptionsDatabase.find(moduleName);

	if ( optionsIter != moduleOptionsDatabase.end() ) {
		return moduleOptionsDatabase[moduleName];
	}
	else {
		throw Util::GenericException("Cannot find module options, unknown module \"" + moduleName + "\".");
	}

}


void SimulationOptions::mergeModuleOptions( const std::string & moduleName, const std::string & options)
{

	// split the options
	std::string tempOptions = options;

	std::vector< char * > optionList;
	_tokenizeModuleOptions( tempOptions, optionList);
	for (unsigned int i=0; i<optionList.size(); i++) {
		std::string optionValue = std::string(optionList[i]);
		std::string option = optionValue.substr(0, optionValue.find('='));
		std::string value = optionValue.substr(optionValue.find('=')+1, std::string::npos);
		if (option=="") {
			throw GenericException("Invalid empty option found in the list of options \"" + options + "\".");
		}
		// the following overrides existing values or creates a new 
		// map entry if the "option" key did not already exist.

		moduleOptionsDatabase[moduleName][option] = value;
	}
}

void SimulationOptions::_tokenizeModuleOptions( std::string & options,  std::vector<char*> & argv)
{
	argv.clear();
	if (options.length() > 0) {
		size_t numchars = options.length();
		std::replace_if( options.begin(), options.end(), _isDelimeter, '\0');
		char * cstr = const_cast<char*>(options.c_str());
		argv.push_back( &(cstr[0]) );
		for (unsigned int i = 1; i < numchars; i++) {
			if (cstr[i-1] == '\0')
				argv.push_back( &(cstr[i]) );
		}
	}
}


void ModuleOptionsXMLParser::startElement( Util::XMLTag * tag, const ticpp::Element * subRoot )
{
	// DO NOT CLEAR the module options here; instead the purpose is to MERGE options with the existing hard-coded 
	// defaults.  This is easy  do since the STL map's [] operator will allow us to overwrite existing values
	// as well as create new vaules if they did not already exist.

	// iterate over the children tags in the xml DOM
	ticpp::Iterator<ticpp::Element> moduleIter;
	for (moduleIter = moduleIter.begin(subRoot); moduleIter != moduleIter.end(); ++moduleIter ) {
		ticpp::Element * moduleRoot;
		moduleRoot = &(*moduleIter);

		std::string moduleName = moduleRoot->Value();

		ticpp::Iterator<ticpp::Element> optionIter;
		for (optionIter = optionIter.begin(moduleRoot); optionIter != optionIter.end(); ++optionIter ) {
			if (optionIter->FirstChildElement(false)) {
				std::cerr << "Error while parsing XML config file: possible invalid extra tag inside of a module option at line "+Util::toString(optionIter->FirstChildElement(false)->Row()) << std::endl;
				// throw GenericException("Error while parsing XML config file: invalid extra tag inside of a module option at line "+Util::toString(optionIter->FirstChildElement(false)->Row()));
			}
			std::string option = (*optionIter).Value();
			std::string value = (*optionIter).GetText(false);
			(*_modOpts)[moduleName][option] = value;
		}
	}
}

void ModuleOptionsXMLParser::outputFormattedXML(std::ostream &out, const std::string & indentation)
{
	std::map<std::string, std::map<std::string, std::string> >::iterator moduleIter;
	std::map<std::string, std::string>::iterator optionIter;

	for (moduleIter = (*_modOpts).begin(); moduleIter != (*_modOpts).end(); ++moduleIter) {
		std::string moduleName;
		moduleName = (*moduleIter).first;
		out << indentation << "<" << moduleName << ">\n";

		for (optionIter = (*_modOpts)[moduleName].begin(); optionIter != (*_modOpts)[moduleName].end(); ++optionIter) {
			out << indentation << "    <" << (*optionIter).first << ">" << (*optionIter).second << "</" << (*optionIter).first << ">\n";
		}
		out << indentation << "</" << moduleName << ">\n";
	}
}

void StartupModulesXMLParser::startElement( Util::XMLTag * tag, const ticpp::Element * subRoot )
{
	ticpp::Iterator<ticpp::Element> moduleIter;
	for (moduleIter = moduleIter.begin(subRoot); moduleIter != moduleIter.end(); ++moduleIter ) {
		// I don't think it is nessasary to invalidate this
		if (moduleIter->FirstChildElement(false)) {
			std::cerr << "Error while parsing XML config file: possible invalid tag at line "+Util::toString(moduleIter->FirstChildElement(false)->Row()) << std::endl;
			// throw GenericException("Error while parsing XML config file: invalid extra tag inside of a module option at line "+Util::toString(moduleIter->FirstChildElement(false)->Row()));
		}
		if ((*moduleIter).Value() != "module") {
			throw GenericException("Error while parsing XML config file: invalid tag at line "+Util::toString(moduleIter->FirstChildElement(false)->Row()) + ", only <module> is allowed.");
		}
		std::string value = (*moduleIter).GetText(false);

		(*_startupModules).insert(value);
	}
}

void StartupModulesXMLParser::outputFormattedXML(std::ostream &out, const std::string & indentation)
{
	std::set<std::string>::iterator moduleIter;
	
	if ((*_startupModules).size() != 0) {
		// if there already are modules loaded and specified, then use those
		for (moduleIter=(*_startupModules).begin(); moduleIter != (*_startupModules).end(); ++moduleIter) {
			out << indentation << "<module>" << (*moduleIter) << "</module>\n";
		}
	}
	else {
		// otherwise, output a simple default, so that the config file has an example of the syntax and usage.
		out << indentation << "<module>testCasePlayer</module>\n";
	}
}



