//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef FrameSaver_h 
#define FrameSaver_h

#ifdef _WIN32
// see steerlib/util/DrawLib.h for explanation
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/DrawLib.h"
#include <errno.h>
#include "SteerLib.h"

namespace Util {
	class  UTIL_API FrameSaver
	{
	public:
		FrameSaver() ;

		unsigned char *m_pixels;
		int m_record ;
		int m_frameCount ;
		int DumpPPM(int w, int h) ;
		void DumpPPM(FILE *fp,int w, int h) ;
		void StartRecord(int width) ;
		void Toggle(int width) ;

		std::string baseName;
		std::string filePath;
	} ;

}

#ifdef _WIN32
// see steerlib/util/DrawLib.h for explanation
#pragma warning( pop )
#endif

#endif
