#!/bin/bash

#
# Copyright (c) Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman, Glen Berseth
# See license.txt for complete license.
#

BUILD_MODULE=$1

if [ "`which gmake 2> /dev/null`" == "" ]; then
    MAKE=make
else
    MAKE=gmake
fi


# remove Visual Studio 2008 files that may exist, only used if the
# user specified "cleanall win32"

# This brute-force careful approach makes sure we don't delete
# potentially precious files that the user forgot inside directory.
echo "Cleaning bin/"
if [ -d bin/ ]; then
		if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "steerbench" ]]; then
    	rm -f bin/steerbench
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "steersim" ]]; then
    	rm -f bin/steersim
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "steerdemo" ]]; then
    	rm -f bin/steerdemo
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "steertool" ]]; then
    	rm -f bin/steertool
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "navmesh" ]]; then
    	rm -f bin/navmeshBuilder
    fi
    rmdir bin/
fi

# This brute-force careful approach makes sure we don't delete
# potentially precious files that the user forgot inside directory.
echo "Cleaning lib/"
if [ -d lib/ ]; then
		
	 	if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "steertool" ]]; then
    	rm -f lib/libsteer.so
    	rm -f lib/libsteer.dylib
    fi
	 	if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "util" ]]; then
    	rm -f lib/libutil.so
		rm -f lib/libutil.dylib
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "steersim" ]]; then
    	rm -f lib/libsteersim.so
		rm -f lib/libsteersim.dylib
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "glfw" ]]; then
    	rm -f lib/libglfw.so
    	rm -f lib/libglfw.dylib
    fi
		if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "anttweakbar" ]]; then
    	rm -f lib/libAntTweakBar.so
    	rm -f lib/libAntTweakBar.dylib
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "qhull" ]]; then
    	rm -f lib/libqhull.so
    	rm -f lib/libqhull_p.so
    	rm -f lib/libqhull.dylib
    	rm -f lib/libqhull_p.dylib
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "navmesh" ]]; then
    	rm -f lib/libDebugUtils.so
    	rm -f lib/libDetour.so
    	rm -f lib/libDetourCrowd.so
    	rm -f lib/libRecast.so
    	rm -f lib/libDetourTileCache.so
    	rm -f lib/libnavmeshBuilder.so
    	rm -f lib/libDebugUtils.dylib
    	rm -f lib/libDetour.dylib
    	rm -f lib/libDetourCrowd.dylib
    	rm -f lib/libRecast.dylib
    	rm -f lib/libDetourTileCache.dylib
    	rm -f lib/libnavmeshBuilder.dylib
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "kdtree" ]]; then
		rm -f lib/libkdtree.so
		rm -f lib/libkdtree.dylib
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "pprAI" ]]; then
    	rm -f lib/libpprAI.so
    	rm -f lib/libpprAI.dylib
    	rm -f lib/pprAI.o
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "rvo2dAI" ]]; then
    	rm -f lib/librvo2dAI.so
    	rm -f lib/librvo2dAI.dylib
    	rm -f lib/rvo2dAI.o
    fi
		if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "footstepAI" ]]; then
    	rm -f lib/libfootstepAI.so
    	rm -f lib/libfootstepAI.dylib
    	rm -f lib/footstepAI.o
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "ccAI" ]]; then
    	rm -f lib/libccAI.so
    	rm -f lib/ccAI.o
    fi
	if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "hidacAI" ]]; then
    	rm -f lib/libhidacAI.so
		rm -f lib/hidacAI.o
    fi
    if [[ $BUILD_MODULE == "all" || $BUILD_MODULE == "sfAI" ]]; then
    	rm -f lib/libsfAI.so
			rm -f lib/sfAI.o
    fi
    
    rm -f lib/libDebugUtils.a  
    rm -f lib/libDetour.a  
    rm -f lib/libDetourCrowd.a
    rm -f lib/libnavmesh.so
    rm -f lib/libRecast.a
    rm -f lib/libsimpleAI.so
    rm -f lib/libsteerlib.so  
    rm -f lib/libsteersimlib.so  
    rm -f lib/libtinyxml.so
    rm -f lib/libacclmesh.so
    rm -f lib/libegocentricAI.so  
    rm -f lib/libhybridAI.so  
    rm -f lib/libmeshdatabase.so  
    rm -f lib/librvo3dAI.so  
    rm -f lib/libscenario.so

    rm -f lib/libDebugUtils.dylib  
    rm -f lib/libDetour.dylib  
    rm -f lib/libDetourCrowd.dylib
    rm -f lib/libnavmesh.dylib
    rm -f lib/libRecast.dylib
    rm -f lib/libsimpleAI.dylib
    rm -f lib/libsteerlib.dylib  
    rm -f lib/libsteersimlib.dylib  
    rm -f lib/libtinyxml.dylib
    rm -f lib/libacclmesh.dylib
    rm -f lib/libegocentricAI.dylib  
    rm -f lib/libhybridAI.dylib  
    rm -f lib/libmeshdatabase.dylib  
    rm -f lib/librvo3dAI.dylib  
    rm -f lib/libscenario.dylib

    rmdir lib/
fi


# This brute-force careful approach makes sure we don't delete
# potentially precious files that the user forgot inside directory.
echo "Cleaning gmake/"
if [ -d gmake/ ]; then
	rm -rf gmake/
fi

