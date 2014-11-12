//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
#include "util/FrameSaver.h"

using namespace Util;

// #define _DEBUG 1
#define FILE_NAME_LENGTH 256

FrameSaver::FrameSaver()
{
	m_record = 0 ;
	m_pixels = NULL ;
	m_frameCount = 0 ;
}
//
//
//void Util::FrameSaver::Toggle(int w)
//{
//	if( m_record == 0 ) 
//		StartRecord(w) ;
//	else 
//		m_record = 0 ;
//}
//
void Util::FrameSaver::StartRecord(int width)
{
	m_record = 1 ;
	m_frameCount = 0 ;
}

//
int Util::FrameSaver::DumpPPM(int width, int height)
{
	if( m_pixels ) delete [] m_pixels ;
	m_pixels = new unsigned char [3*width];
    if(	m_pixels == NULL )
    {
		fprintf(stderr,"Cannot allocate	enough memory\n") ;
		return  -1;
    }

	char fname[FILE_NAME_LENGTH] ;
	if( m_record == 0 ) // one time save
		strcpy(fname, "scene.ppm") ;
	else				// animation recording
	{
		sprintf(fname,"%sframe%d.ppm", filePath.c_str(), m_frameCount) ;
		m_frameCount++ ;
	}
	FILE *fp = fopen(fname,"wb") ;
	if( fp == NULL ) 
	{
		fprintf(stderr, "Cannot open file %s\n", fname) ;
		return -1 ;
	}
	DumpPPM(fp,width,height) ;
	fclose(fp) ;
	return 1 ;
}


//
// dumps a PPM raw (P6) file on an already allocated memory array
void Util::FrameSaver::DumpPPM(FILE *fp, int width, int height)
{
#ifdef ENABLE_GLFW
    const int maxVal=255;
    register int y;
    int r = 0;
    int sum = 0;
    int b_width = 3*width;
    //printf("width = %d height = %d\n",width, height) ;
#ifdef _DEBUG
    std::cout << "Writing Frame Dump\n";
    std::cout << "width = " << width << ", height = " << height << "\n";
#endif
    fprintf(fp,	"P6 ");
    fprintf(fp,	"%d %d ", width, height);
    fprintf(fp,	"%d\n",	maxVal);

	glReadBuffer(GL_FRONT) ;

	for	( y = height-1;	y>=0; y-- ) 
	{
		// bzero(m_pixels, 3*width);
		glReadPixels(0,y,width,1,GL_RGB,GL_UNSIGNED_BYTE, 
			(GLvoid *) m_pixels);
#ifdef _DEBUG
		sum = 0;
		for (r=0; r < b_width; r++)
		{
			sum = sum + m_pixels[r];
		}
		std::cout << "Sum of bytes being written is: " << sum << "\n";
#endif
		fwrite(m_pixels, 3, width, fp);
	}
#endif
}
