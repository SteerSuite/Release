//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
// Courtesy Alan Gasperini

#include "Timer.h"

#ifdef WIN32
#pragma comment(lib, "winmm.lib")
Timer::Timer()
{
	last_time=0;
	if(QueryPerformanceFrequency((LARGE_INTEGER*) &perf_cnt))
	{
		perf_flag=true;
		time_count=DWORD(perf_cnt); //perf_cnt counts per second
		QueryPerformanceCounter((LARGE_INTEGER*) &last_time);
		time_scale=1.0f/perf_cnt;
		QPC=true;
	}
	else
	{
		perf_flag=false;
		last_time=timeGetTime();
		time_scale=0.001f;
		time_count=33;
	}
}

float Timer::GetElapsedTime()
{
	if(perf_flag)
		QueryPerformanceCounter((LARGE_INTEGER*) &cur_time);
	else
		cur_time=timeGetTime();
				
	float time_elapsed=(cur_time-last_time)*time_scale;
	//last_time=cur_time;
	return time_elapsed;
}

#else
//***********************************unix specific*********************************
Timer::Timer()
{
	gettimeofday(&cur_time,0);
}

float Timer::GetElapsedTime()
{
	float dif;
	timeval newtime;
	gettimeofday(&newtime,0);
	dif=(newtime.tv_sec-cur_time.tv_sec);
	dif+=(newtime.tv_usec-cur_time.tv_usec)/1000000.0;
	return dif;
}

#endif // unix
