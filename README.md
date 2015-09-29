SteerSuite
==========

Welcome!  SteerSuite is a set of test cases, tools, and a C++ library
for developing and evaluating agent steering AI behaviors.

The SteerSuite web page is
[steersuite.eecs.yorku.ca](http://steersuite.eecs.yorku.ca/)

On the web page you can find a description of SteerSuite, download the
latest version of SteerSuite, read the latest documentation, and join
the discussion group.

Please send us any comments and feedback about how to make SteerSuite
easier to use.  Your input is greatly appreciated.

Enjoy!

Documentation
-------------

If you want to build your own documentation from this package, read
the file `documentation/readme.txt` for more information.  Otherwise,
the latest documentation can be found at the
[SteerSuite web page](http://steersuite.eecs.yorku.ca/).

The documentation includes:

  - The User Guide explains how to use most features of SteerSuite.
  - The Reference Manual contains a comprehensive listing of
    SteerSuite components.
  - The Code Reference is doxygen-generated documentation of the C++
    code.

Directory structure
-------------------

The directory structure of this package is as follows:

    build           - Unix scripts and Visual Studio 2012 solution file
                      to compile all components of SteerSuite.

    documentation   - raw unprocessed documentation and instructions for
                      building the documentation.

    external        - external dependencies that are (legally) included
                      for convenience, but NOT part of SteerSuite.

    kdtree          - Spatial Database module for a kdtree type spatial
                      databse.

    navmeshBuilder  - Module to build navigation meshes during runtime.
                      Uses Recast.

    pprAI           - source directory for the PPR steering module, a
                      demo steering algorithm for SteerSim.

    rvo2AI          - source directory for the ORCA steering module,
                      based on the RVO2 steering algorithm library.

    reactiveAI      - source directory for the reactive steering module,
                      a demo steering algorithm for SteerSim (just the
                      reactive appraoch of PPR).

    socialForcesAI  - source directory for the social foces steering
                      module, an implementation of the social forces
                      steering algorithm.

    simpleAI        - source directory for the simpleAI module, a basic
                      demo plugin for SteerSim.

    steerbench      - source directory for SteerBench, a tool used to
                      score and analyze steering AI.

    steerlib        - source directory for SteerLib, a shared library
                      containing most of SteerSuite's functionality.

    steersim        - source directory for SteerSim, a modular
                      simulation tool.

    steertool       - source directory for SteerTool, a utility for
                      useful miscellaneous tasks.

    testcases       - XML test cases and the XML schema describing the
                      test case format.

Compiling SteerSuite
--------------------

Below are quick instructions for compiling with default options. For
more complete instructions, refer to the
[SteerSuite User Guide](http://steersuite.eecs.yorku.ca/UserGuide/).

As with any graphics library you will need to make sure you already
have the OpenGL libraries on your computer. For example on
Ubuntu 14.04 you will want to install the following:

    freeglut3-dev build-essential libx11-dev libxmu-dev libxi-dev libgl1-mesa-glx libglu1-mesa libglu1-mesa-dev libglew1.6-dev mesa-utils libglew-dev premake4

This will install OpenGL and GLEW.

On Windows you will need to download glut32.lib and the opengl header files and put them in your Visual Studio library path.

Note: The build system has been updated and now uses
[premake4](http://premake.github.io/). You are going to need to have
this installed to be able to build the software.

### Windows 7/8 with Visual Studio 2012

Run `premake4 --platform=x64 vs2012` and choose Debug/Release and
32/64 bit mode. Most components should compile successfully, and all
compiled components will be in the `build/bin` folder.

### Linux/Unix

First run the following:

    cd build
    premake4 gmake
    cd gmake

Then run `make config=[debug|release]`, depending on your preference
for the type of build.

All components are copied into the `build/bin` and `build/lib`
directories.

### Mac OS X

For now, the process is the same as Linux/Unix.  With OS X version
10.4 or earlier, you may need to use an `LD_LIBRARY_PATH` environment
variable for the executable to properly link with shared and dynamic
libraries.

Contact information
-------------------

Contact Information:

- Glen Berseth      `glenpb@cse.yorku.ca`
- Mubbasir Kapadia  `mubbasir@cs.ucla.edu`
- Petros Faloutsos  `pfal@cse.yorku.ca`
- Glenn Reinman     `reinman@cs.ucla.edu`

SteerSuite web page:
[steersuite.eecs.yorku.ca](http://steersuite.eecs.yorku.ca/)

Forum (Google Group):
[groups.google.com/forum/#!forum/steersuite](https://groups.google.com/forum/#!forum/steersuite)

Please report bugs by opening an issue on
[github](https://github.com/SteerSuite/Release/issues). For any other
queries please use the forum.

Copyright and license
---------------------

SteerSuite, SteerBench, SteerBug, SteerSim, and SteerLib are Copyright
(c) 2008-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros
Faloutos, and Glenn Reinman.

See `license.txt` for the complete license.

NOTE: The contents of the `external/` directory are NOT part of
SteerSuite.  Each component in `external/` has its own authors,
copyright, and license, and those souces are only included for
convenience.

Credits
-------

Refer to the [SteerSuite web page](http://steersuite.eecs.yorku.ca/)
for credits and acknowledgements.
