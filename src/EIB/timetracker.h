//
// C++ Interface: timetracker
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSTIMETRACKER_H
#define EIBBUSTIMETRACKER_H

#include <time.h>

namespace EIBBUS {

/**
@author 
*/
class TimeTracker
{
public:
    TimeTracker(bool start = false);
    ~TimeTracker();

public:
	void Start();
	void Stop();

	double getCurrentPeriodInMiliSecs(); // diff between current time and stop time
	double getCurrentPeriodInSecs(); // diff between current time and stop time
	double getTotalPeriodInSecs(); // diff between stop time and start time

private:
	struct timespec starttm_;
	struct timespec stoptm_;
};

};

#endif
