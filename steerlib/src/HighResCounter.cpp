//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file HighResCounter.cpp
/// @brief Implements timer/counter functionality.

#include <iostream>
#include <assert.h>
#include "util/GenericException.h"
#include "util/HighResCounter.h"

using namespace Util;

#ifndef _WIN32

// initial values
unsigned long long CounterFrequencyEstimator::_freq = 0;
bool CounterFrequencyEstimator::_frequencyComputed = false;

/// Private function to compute an estimate of the counter frequency; <em>BEWARE:</em> this function is called before main, during static initialization.
void CounterFrequencyEstimator::_computeFrequencyEstimate()
{
    assert(_frequencyComputed == false);

    unsigned long long before, after;

    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 999999999;

    before = getHighResCounterValue();
    while (nanosleep(&t,NULL) != 0) {
	before = getHighResCounterValue();
    }
    after = getHighResCounterValue();

    _freq = after-before;
    _frequencyComputed = true;
}

#endif


float Util::getEstimatedCounterResolution()
{
	const unsigned int numTrials = 5000;

	unsigned long long before = 0, after = 0;
	unsigned long long totalDiff = 0;

	for (unsigned int i=0; i < numTrials; i++) {

		// tight-loop until there is at least some small difference in the tick.
		before = getHighResCounterValue();
		after = getHighResCounterValue();
		while (before==after) {
			after = getHighResCounterValue();
		}

		unsigned long long diff = after-before;
		// the while-loop above should guarantee that we don't get here unless there really is a valid difference between before and after.
		assert(diff != 0);
		totalDiff += diff;
	}


	// compute average number of "ticks" per call.
	float avgDiff = ((float)totalDiff) / ((float)(numTrials));

	// convert to seconds.  NOTE CAREFULLY: this assumes that getHighResCounterFrequency() is at least mildly accurate!!
	float rate = avgDiff / ((float)getHighResCounterFrequency());

	return rate;
}
