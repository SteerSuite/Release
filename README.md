SteerSuite
==========

-----------
 CONTENTS
-----------
- Introduction
- Documentation
- Directory Structure
- Compiling SteerSuite
- Contact Information
- Copyright and License
- Credits

---------------
 INTRODUCTION
---------------

Welcome!  SteerSuite is a set of test cases, tools, and a C++ library
for developing and evaluating agent steering AI behaviors.

The SteerSuite web page is:

  http://steersuite.eecs.yorku.ca/

On the web page you can find a description of SteerSuite, download the
latest version of SteerSuite, read the latest documentation, and join
the discussion group.

Please send us any comments and feedback about how to make SteerSuite
easier to use.  Your input is greatly appreciated.

Enjoy!


----------------
 DOCUMENTATION
----------------

If you want to build your own documentation from this package, read
the file documentation/readme.txt for more information.  Otherwise,
the latest documentation can be found at the SteerSuite web page at:

  http://steersuite.eecs.yorku.ca/

The documentation includes:

  - The User Guide explains how to use most features of SteerSuite
  - The Reference Manual contains a comprehensive listing of
    SteerSuite components
  - The Code Reference is doxygen-generated documentation of the C++
    code.



----------------------
 DIRECTORY STRUCTURE
----------------------

The directory structure of this package is as follows:

	- build/          Unix scripts and Visual Studio 2012 solution file 
                  to compile all components of SteerSuite.
	- documentation/  raw unprocessed documentation and instructions for
                  building the documentation.
	- external/       external dependencies that are (legally) included
                  for convenience, but NOT part of SteerSuite.
	- pprAI/          source directory for the PPR steering module, a
                  demo steering algorithm for SteerSim.
	- rvo2AI/         source directory for the ORCA steering module, based
                  on the RVO2 steering algorithm library.
	- socialForcesAI/ source directory for the social foces steering module, a
                  implementation of the social forces steering algorithm.
	- simpleAI/       source directory for the simpleAI module, a basic
                  demo plugin for SteerSim.
	- steerbench/     source directory for SteerBench, a tool used to
                  score and analyze steering AI.
	- steerlib/       - source directory for SteerLib, a shared library
                  containing most of SteerSuite's functionality.
	- steersim/       source directory for SteerSim, a modular simulation
                  tool.
	- steertool/      source directory for SteerTool, a utility for useful
                  miscellaneous tasks
	- testcases/      XML test cases and the XML schema describing the
                  test case format.



-----------------------
 COMPILING STEERSUITE
-----------------------

Below are quick instructions for compiling with default options. For
more complete instructions, refer to the SteerSuite User Guide.  

As with any graphics library you will need to make sure you already have the
opengl libraries on you computer. For example on Ubuntu 14.04 you will want
to install
```
freeglut3-dev build-essential libx11-dev libxmu-dev libxi-dev libgl1-mesa-glx libglu1-mesa libglu1-mesa-dev libglew1.6-dev mesa-utils
```
This will install opengl and glew.  

Windows XP/Vista/7/8 with Visual Studio 2012:
  1. Open the Visual Studio 2012 solution file, located in 
     build/win32/steersuite.sln
  2. Choose Debug or Release mode,
  3. All components should compile successfully.
  4. All compiled components will be in the build/win32/Debug/ or
     build/win32/Release/ folder.

Mac OS X:
  For now, the process is the same as Linux/Unix.  With OS X version
  10.4 or earlier, you may need to use an LD_LIBRARY_PATH environment
  variable for the executable to properly link with shared and dynamic
  libraries.

Linux/Unix:
  1. From a command line, go to the build/ directory
  2. Run ./buildall <platform>
      - make sure you are in the build/ directory.
      - running ./buildall with no args will list the possible platforms.
  3. All of the given components should compile succefuly. A few librarys
   that are not included may fail.
  4. All components are copied into the build/bin/, build/lib/, and
     build/modules/ directories.


----------------------
 CONTACT INFORMATION
----------------------

To report bugs or give general feedback, email Shawn or Glen or Mubbasir.

Contact Information:
  Glen Berseth      gberseth@cs.ubc.ca
  Mubbasir Kapadia  mubbasir@cs.ucla.edu
  Petros Faloutsos  pfal@cse.yorku.ca
  Glenn Reinman     reinman@cs.ucla.edu

SteerSuite web page:
  http://steersuite.eecs.yorku.ca/

------------------------
 COPYRIGHT AND LICENSE
------------------------

SteerSuite, SteerBench, SteerBug, SteerSim, and SteerLib,
Copyright (c) 2008-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros
Faloutos, Glenn Reinman.

See license.txt for complete license.

NOTE:
The contents of the external/ directory are NOT part of SteerSuite.
Each component in external/ has its own authors, copyright, and
license, and those sources are only included for convenience.

----------
 CREDITS
----------

Refer to the SteerSuite web page for credits and acknowledgements.


