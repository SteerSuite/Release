//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
// Courtesy Alan Gasperini

#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef WIN32
#include <windows.h>

class Timer
{
public:
	Timer();

	//in seconds
	float GetElapsedTime();
	void Reset();

private:
	LONGLONG cur_time;

	DWORD time_count;
	LONGLONG perf_cnt;
	bool perf_flag;
	LONGLONG last_time;
	float time_scale;

	bool QPC;
};

inline void Timer::Reset()
{
	last_time = cur_time;
}


#else
//*****************************unix stuff****************************
#include <sys/time.h>

class Timer
{
public:
	Timer();

	void Reset();
	float GetElapsedTime();

private:
	timeval cur_time;

};

inline void Timer::Reset()
{
	gettimeofday(&cur_time,0);
}



#endif //unix

#endif  // _TIMER_H_
