//
// C++ Implementation: timetracker
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "timetracker.h"

namespace EIBBUS {

TimeTracker::TimeTracker()
{
	starttm_.tv_sec = starttm_.tv_nsec = 0;
	stoptm_.tv_sec = stoptm_.tv_nsec = 0;
}

TimeTracker::~TimeTracker(){
}

void 
TimeTracker::Start() {
	clock_gettime(CLOCK_REALTIME, &starttm_);
}

void 
TimeTracker::Stop() {
	clock_gettime(CLOCK_REALTIME, &stoptm_);
}

double 
TimeTracker::getPeriodInSecs() {
	return stoptm_.tv_sec - starttm_.tv_sec + 1.0 * (stoptm_.tv_nsec - starttm_.tv_nsec) / 1000000000;
}

};
