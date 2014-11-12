//
// Copyright (c) 2009 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file Documentation.cpp
/// @brief Contains additional doxygen documentation.
///
/// This file contains the doxygen code reference main page, and any Doxygen documentation that does not have a
/// better place.
///

/// @page todo Todo List
///
/// @todo
///  - test compilation on osx
///  - typedef all the messy complicated STL containers; good for readability and portability of code.
///  - implement circle class, box2D class ??
///  - check for redundant code in multiple constructors (all should call a single init function) particularly in RecFileReader
///  - document several places in test case IO and rec file IO, explaining the index parameters of many of the functions.
///  - fix the correspondence between "world bounds" of the recfile/testcase and the spatial database being allocated.
///  - comb through all interfaces and propagate const correctness everywhere, will help greatly for compiler optimization.
///  - gradually migrate interfaces to use return-by-reference instead of pointers when possible
///

/// @mainpage SteerSuite Code Reference
///
/// This set of pages is doxygen-generated documentation of the SteerSuite code.  This includes %SteerLib, the main library of SteerSuite,
/// as well as the code from all other tools.
///
/// In addition to the code reference, there is also a User Guide and Reference Manual.  The User Guide walks through the 
/// main features of SteerSuite and how to use them.  The Reference Manual describes all test cases, metrics, benchmark 
/// techniques, modules, command line tools in SteerSuite.
///
/// <h2>Useful Starting Points for Programming with %SteerLib</h2>
///
///  - The <a href="http://www.magix.ucla.edu/steersuite/UserGuide">User Guide</a> has an entire chapter on using %SteerLib, including
///    simple steps to include and link %SteerLib into your own code.
///
/// <b>Simulations and modules:</b>
///
///  - To create a plugin module for simulations, you will need to include SimulationPlugin.h,
///    implement #createModule and #destroyModule, and inherit and implement SteerLib::ModuleInterface. 
///    Optionally, your plugin can also use functionality provided by the
///    SteerLib::EngineInterface, and implement your own agents by inheriting SteerLib::AgentInterface.
///
///  - You can use the simulation engine in your own programs directly, too.  To do this, inherit and implement
///    SteerLib::EngineControllerInterface.   This class will essentially be your wrapper to the
///    SteerLib::SimulationEngine, which you will instantiate and use inside of your engine controller.
///
/// <b>Developing your own AI:</b>
///
///  - The SteerLib::BestFirstSearchPlanner is a template class that implements a best-first search.  You define the
///    state space and action space for the planning task by implementing the functionality in SteerLib::StateSpaceBase.
///    By varying the heuristic, you can make the search behave like A* or any other best-first search technique.
///
///  - The SteerLib::GridDatabase2D is a very useful spatial database that allows you to perform nearest-neighbor lookups,
///    ray tracing queries, path-finding queries, and more.  This class is essential for agents to be able to "see" things
///    in the environment in various ways.
///
///  - The Util::StateMachine may be helpful for keeping track of valid and invalid transitions between states.
///
/// <b>Reading test cases and recordings:</b>
///
///  - The SteerLib::TestCaseReader class is an easy way to read XML test cases.  It automatically parses the XML and 
///    sets up the initial conditions of the test case, and all you have to do is query the initial conditions
///    to initialize your own code.
///
///  - The SteerLib::RecFileReader class is used to read an existing recording.
///
///  - The SteerLib::RecFileWriter class is used to create a recording of a steering simulation.  You only need to be able to
///    specify the position and orientation of each agent in each frame.
///
/// <b>More:</b>
///
///  - The SteerLib namespace includes more functionality, including metrics computations, benchmarking
///    techniques, a spatial database, etc.
///
///  - The Util namespace contains very useful general functionality, mostly platform-independent.  This 
///    includes functionality for loading and using dynamic libraries, read-only memory mapping, high-resolution
///    counters and performance profiling, task multithreading, and more.
/// 
///
/// <h2>Error Handling in %SteerSuite</h2>
/// External errors (for example, an input file has bad syntax, accidental misuse of a class) 
/// are handled by throwing a Util::GenericException containing a useful error message.  Internal 
/// errors use assert statements.
///
/// <b>We strongly suggest that you catch exceptions and look at the useful message
/// returned by Util::GenericException::what().</b>  If your code does not handle exceptions, 
/// then %SteerLib may appear to crash abruptly for no reason, but most likely the error 
/// is a simple fix outside of %SteerLib, based on the error message contained in the 
/// exception.  Reading these messages can save you hours of debugging.
///


/// @namespace SteerLib 
/// @brief The namespace for the main functionality provided in %SteerLib.
///
/// This namespace contains steering-specific functionality.
///



/// @namespace Util  
/// @brief The namespace for utility functions.
///
/// This namespace contains useful generic "utility" functionality.
///



/// @namespace SteerSimQt
/// @brief The namespace for SteerSim Qt GUI functionality.
///
/// This namespace contains the classes and structures related to the SteerSim Qt GUI.
///

