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
    TimeTracker();
    ~TimeTracker();

public:
	void Start();
	void Stop();

	double getPeriodInSecs();

private:
	struct timespec starttm_;
	struct timespec stoptm_;
};

};

#endif
