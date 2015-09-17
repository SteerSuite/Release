//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_SIMULATION_OPTIONS_H__
#define __STEERLIB_SIMULATION_OPTIONS_H__

/// @file SimulationOptions.h
/// @brief Declares the SimulationOptions class and functionality.
///
/// @todo
///  - robust-ify the feature for generating the default XML config file and for reading from an XML config file.
///  - update many of the options...
///

#include <string>
#include <set>

#include "Globals.h"
#include "util/Geometry.h"
#include "util/Color.h"
#include "util/XMLParser.h"
#include "interfaces/ModuleInterface.h"


#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif


/*
//====================================
// TRAVERSAL COST OPTIONS
// todo: should these become options? the only one really used is COST_UNTRAVERSABLE, hard-coded in a few places ...
//====================================
// hard-coded traversal costs
#define COST_TRAVERSABLE_THRESHOLD           1000
#define COST_UNTRAVERSABLE                   (COST_TRAVERSABLE_THRESHOLD+1)
#define COST_NO_EXTRA_COST                   0
#define COST_EASY_TO_TRAVERSE                1
#define COST_DYNAMIC_AGENT_BLOCKING_THE_WAY  0
#define COST_HARD_TO_TRAVERSE                50
#define COST_SHOULD_AVOID                    200
*/

namespace SteerLib {

	class STEERLIB_API ModuleOptionsXMLParser : public Util::XMLParserCallbackInterface {
	public:
		void init(ModuleOptionsDatabase * moduleOptionsDatabasePtr) { _modOpts = moduleOptionsDatabasePtr; }
		void startElement( Util::XMLTag * tag, const ticpp::Element * subRoot );
		void outputFormattedXML(std::ostream &out, const std::string & indentation);
	protected:
		ModuleOptionsDatabase * _modOpts;
	};

	class STEERLIB_API StartupModulesXMLParser : public Util::XMLParserCallbackInterface {
	public:
		void init(std::set< std::string > * startupModules) { _startupModules = startupModules; }
		void startElement( Util::XMLTag * tag, const ticpp::Element * subRoot );
		void outputFormattedXML(std::ostream &out, const std::string & indentation);
	protected:
		std::set<std::string> * _startupModules;
	};


	/**
	 * @brief Class containing all options related to the SimulationEngine and <b>steersim</b>.
	 *
	 * The values are initialized to hard-coded defaults, which can then be overridden
	 * by a configuration file using loadOptionsFromConfigFile() as well as directly changing 
	 * the values here.  A config file can be generated with the current options using 
	 * generateConfigFile(), which can then be edited using any text or XML editor.
	 *
	 * If you are instantiating a SimulationEngine directly from your code, you will create an instance
	 * of this class, set the options as you like, and then give this instance to the SimulationEngine.
	 *
	 */
	class STEERLIB_API SimulationOptions {
	public:

		/// Constructor initializes all options to hard-coded defaults.
		SimulationOptions();

		/// @name Configuration file usage
		//@{
		/// Loads options from a configuration file; any options specified in the file will override hard-coded defaults.
		void loadOptionsFromConfigFile( const std::string & filename );
		/// Generates a configuration file using the current values of all options.
		void generateConfigFile( const std::string & filename );
		//@}

		/// @name Module option management
		//@{
		/// Returns an STL map of options for a specific module; note that moduleName is case-sensitive.
		const SteerLib::OptionDictionary & getModuleOptions(const std::string & moduleName);
		/// @brief Adds module-specific options; please refer to detailed documentation of this function.
		///
		/// The options string can contain any number of options, where each option is of the form
		/// "optionName=value", and options are separated by a comma or a space.
		///
		/// for example, the following is a valid option string for the recFilePlayer module:
		/// \code
		/// "testcase=curves.xml,ai=simpleAI"
		/// \endcode
		///
		/// If the option did not exist, this function will add the option to the module options database.
		/// If the option already existed, this function will override the existing option with the new value.
		/// Finally, note that both parameters moduleName and options are case-sensitive.
		void mergeModuleOptions( const std::string & moduleName, const std::string & options);
		//@}



/*
		/// @name Keyboard map accessors
		//@{
		inline int quitKey() const { return _keyboardBindingDefaults.quit; }
		inline int printCameraInfoKey() const { return _keyboardBindingDefaults.printCameraInfo; }
		inline int toggleAntialiasingKey() const { return _keyboardBindingDefaults.toggleAntialiasing; }
		inline int takeScreenshotKey() const { return _keyboardBindingDefaults.takeScreenshot; }
		inline int startDumpingFramesKey() const { return _keyboardBindingDefaults.startDumpingFrames; }
		inline int stopDumpingFramesKey() const { return _keyboardBindingDefaults.stopDumpingFrames; }
		inline int pauseKey() const { return _keyboardBindingDefaults.pause; }
		inline int stepForwardKey() const { return _keyboardBindingDefaults.stepForward; }
		inline int stepBackwardKey() const { return _keyboardBindingDefaults.stepBackward; }
		inline int speedupPlaybackKey() const { return _keyboardBindingDefaults.speedupPlayback; }
		inline int slowdownPlaybackKey() const { return _keyboardBindingDefaults.slowdownPlayback; }
		inline int resetPlaybackSpeedKey() const { return _keyboardBindingDefaults.resetPlaybackSpeed; }
		inline int restartPlaybackKey() const { return _keyboardBindingDefaults.restartPlayback; }
		//@}

		/// @name Mouse map accessors
		//@{
		inline int selectAgentClick() const { return _mouseBindingDefaults.selectAgent; }
		inline int moveCameraClick() const { return _mouseBindingDefaults.moveCamera; }
		inline int rotateCameraClick() const { return _mouseBindingDefaults.rotateCamera; }
		inline int zoomCameraClick() const { return _mouseBindingDefaults.zoomCamera; }
		//@}

		/// @name Global options accessors
		//@{
		inline std::string defaultEngineDriver() const { return _globalDefaults.engineDriver; }
		inline std::string defaultCoutRedirectionFilename() const { return _globalDefaults.coutRedirectionFilename; }
		inline std::string defaultCerrRedirectionFilename() const { return _globalDefaults.cerrRedirectionFilename; }
		inline std::string defaultClogRedirectionFilename() const { return _globalDefaults.clogRedirectionFilename; }
		//@}


		/// @name Engine options accessors
		//@{
		inline std::string defaultModuleSearchPath() const { return _engineDefaults.moduleSearchPath; }
		inline std::string defaultTestCaseSearchPath() const { return _engineDefaults.testCaseSearchPath; }
		inline std::set<std::string> defaultStartupModules() const { return _engineDefaults.startupModules; }
		inline unsigned int defaultNumThreads() const { return _engineDefaults.numThreads; }
		inline unsigned int defaultNumFramesToSimulate() const { return _engineDefaults.numFramesToSimulate; }
		inline float defaultFixedFPS() const { return _engineDefaults.fixedFPS; }
		inline float defaultMinVariableDt() const { return _engineDefaults.minVariableDt; }
		inline float defaultMaxVariableDt() const { return _engineDefaults.maxVariableDt; }
		inline std::string defaultClockMode() const { return _engineDefaults.clockMode; }
		//@}

		/// @name Grid database options accessors
		//@{
		inline unsigned int defaultMaxItemsPerGridCell() const { return _gridDatabaseDefaults.maxItemsPerGridCell; }
		inline float defaultGridSizeX() const { return _gridDatabaseDefaults.gridSizeX; }
		inline float defaultGridSizeZ() const { return _gridDatabaseDefaults.gridSizeZ; }
		inline unsigned int defaultNumGridCellsX() const { return _gridDatabaseDefaults.numGridCellsX; }
		inline unsigned int defaultNumGridCellsZ() const { return _gridDatabaseDefaults.numGridCellsZ; }
		//@}

		/// @name GUI options accessors
		//@{
		inline bool defaultUseAntialiasing() const { return _guiDefaults.useAntialiasing; }
		inline bool defaultUseVsync() const { return _guiDefaults.useVsync; }
		inline float defaultMouseRotationFactor() const { return _guiDefaults.mouseRotationFactor; }
		inline float defaultMouseZoomFactor() const { return _guiDefaults.mouseZoomFactor; }
		inline float defaultMouseMovementFactor() const { return _guiDefaults.mouseMovementFactor; }
		inline bool defaultCanUseMouseSelection() const { return _guiDefaults.canUseMouseSelection; }
		inline bool defaultCanUseMouseWheelZoom() const { return _guiDefaults.canUseMouseWheelZoom; }
		inline Util::Point defaultCameraPosition() const { return _guiDefaults.cameraPosition; }
		inline Util::Point defaultCameraLookAt() const { return _guiDefaults.cameraLookAt; }
		inline Util::Vector defaultCameraUp() const { return _guiDefaults.cameraUp; }
		inline float defaultCameraFovy() const { return _guiDefaults.cameraFovy; }
		inline Util::Color defaultBackgroundColor() const { return _guiDefaults.backgroundColor; }
		inline float defaultLineWidth() const { return _guiDefaults.lineWidth; }
		//@}

		// Command line engine driver options accessors
		// no options for the command line at the moment.

		/// @name GLFW engine driver options accessors
		//@{
		inline bool defaultPausedOnStart() const { return _glfwEngineDriverDefaults.pausedOnStart; }
		inline unsigned int defaultWindowSizeX() const { return _glfwEngineDriverDefaults.windowSizeX; }
		inline unsigned int defaultWindowSizeY() const { return _glfwEngineDriverDefaults.windowSizeY; }
		inline unsigned int defaultWindowPositionX() const { return _glfwEngineDriverDefaults.windowPositionX; }
		inline unsigned int defaultWindowPositionY() const { return _glfwEngineDriverDefaults.windowPositionY; }
		inline std::string defaultWindowTitle() const { return _glfwEngineDriverDefaults.windowTitle; }
		//@}

		// Qt engine driver options accessors
		// options todo:
		//  - default placement of console
		//  - default placement of steersim module windows
		//  - default openGL widget size
		//  - default GUI size
		//  - lock dock widgets in place
		//  - wrap console text
*/

		struct KeyboardBindings {
			int quit;
			int printCameraInfo;
			int toggleAntialiasing;
			int takeScreenshot;
			int startDumpingFrames;
			int stopDumpingFrames;
			int pause;
			int stepForward;
			int stepBackward;
			int speedupPlayback;
			int slowdownPlayback;
			int resetPlaybackSpeed;
			int restartPlayback;
			int dumpTestCase;
		};

		struct MouseBindings {
			int selectAgent;
			int moveCamera;
			int rotateCamera;
			int zoomCamera;
		};

		struct GlobalOptions {
			std::string engineDriver;
			std::string coutRedirectionFilename;
			std::string cerrRedirectionFilename;
			std::string clogRedirectionFilename;
			bool blendingDemo;
			bool parameterDemo;
			bool noTweakBar;
			std::string dataFileName;
		};

		struct EngineOptions {
			std::string moduleSearchPath;
			std::string testCaseSearchPath;
			std::string frameDumpDirectory;
			std::set<std::string> startupModules;
			unsigned int numThreads;
			unsigned int numFramesToSimulate;
			float fixedFPS;
			float minVariableDt;
			float maxVariableDt;
			std::string clockMode;
		};

		struct GridDatabaseOptions {
			unsigned int maxItemsPerGridCell;
			float gridSizeX;
			float gridSizeZ;
			unsigned int numGridCellsX;
			unsigned int numGridCellsZ;
			bool drawGrid;
		};

		struct SpatialDatabaseOptions {
			std::string name;
		};

		struct PlanningDomainOptions {
			std::string name;
			unsigned int maxNodesToExpand;
		};

		struct GUIOptions {
			bool useAntialiasing;
			bool useVsync;
			float mouseRotationFactor;
			float mouseZoomFactor;
			float mouseMovementFactor;
			bool canUseMouseSelection;
			bool canUseMouseWheelZoom;
			Util::Point cameraPosition;
			Util::Point cameraLookAt;
			Util::Vector cameraUp;
			float cameraFovy;
			Util::Color backgroundColor;
			float lineWidth;
		};

		struct CommandLineEngineDriverOptions {
		};

		struct GLFWEngineDriverOptions {
			bool pausedOnStart;
			unsigned int windowSizeX;
			unsigned int windowSizeY;
			unsigned int windowPositionX;
			unsigned int windowPositionY;
			std::string windowTitle;
			bool fullscreen;
			std::string stereoMode;
		};

		struct QtEngineDriverOptions {
		};

		/// @name Options data
		/// @brief The actual options are stored in these public data structures.
		///
		/// Users may want to programmatically set up these options for their own purposes,
		/// and there is no compelling reason to make an unnecessary layer of get() and 
		/// set() accessor functions.  Therefore, these are publically accessible.
		///
		/// It is recommended to use the mergeModuleOptions() function to modify the
		/// moduleOptionsDatabase, but it is also kept public so that users can directly 
		/// delete options from that database if desired.
		///
		//@{
		KeyboardBindings   keyboardBindings;
		MouseBindings   mouseBindings;
		GlobalOptions   globalOptions;
		EngineOptions   engineOptions;
		SpatialDatabaseOptions spatialDatabaseOptions;
		PlanningDomainOptions planningDomainOptions;
		GridDatabaseOptions   gridDatabaseOptions;
		GUIOptions   guiOptions;
		CommandLineEngineDriverOptions   commandLineEngineDriverOptions;
		GLFWEngineDriverOptions   glfwEngineDriverOptions;
		QtEngineDriverOptions   qtEngineDriverOptions;
		SteerLib::ModuleOptionsDatabase   moduleOptionsDatabase;
		//@}


	protected:

		/// Helper function to tokenize options when merging module options.
		void _tokenizeModuleOptions( std::string & options,  std::vector<char*> & argv);
		/// Helper function for tokenizing options.
		static bool _isDelimeter(char c) { return ((c==',')||(c==' ')); }


		/// A helper function to set up the XML parsing structure, for both parsing and serializing (generating) XML files.
		void _setupXMLStructure( Util::XMLParser & xmlOpts );

		/// A helper class to parse module options in the XML config file.
		ModuleOptionsXMLParser _moduleOptionsXMLParser;
		/// A helper class to parse the list of startup modules in the XML config file
		StartupModulesXMLParser _startupModulesXMLParser;
		/// @name Backwards-compatibility data
		/// @brief Do not use these options, they are only kept for backwads compatibility of old config files.
		//@{
		bool _dummyUseShadowsFlag;
		//@}
	};

} // end namespace SteerLib


#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
