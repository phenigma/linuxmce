/*
 * search.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: search.h,v 1.1 2004/11/23 14:21:24 schmitzj Exp $
 *
 */

#ifndef SEARCH_H
#define SEARCH_H

#include <vdr/plugin.h>
#include <time.h>
#include "gfxtools.h"
#include "config.h"

class EventItem : public cOsdItem
{
#if VDRVERSNUM >= 10300
	const class cEvent *myev;
#else
	const class cEventInfo *myev;
#endif

public:
#if VDRVERSNUM >= 10300
	EventItem(const class cEvent *ev);
#else
	EventItem(const class cEventInfo *ev);
#endif

#if VDRVERSNUM >= 10300
	const class cEvent *Event(void)
#else
	const class cEventInfo *Event(void)
#endif
	{
		return myev;
	};
};

// -----------------------------------------------------------------------------

class cSearchMenu : public cOsdMenu
{
#if VDRVERSNUM >= 10300
	cSchedulesLock _schedulesLock;
#else
	cMutexLock _mutexLock;
#endif

#if VDRVERSNUM >= 10300
	const class cEvent *myev;
#else
	const class cEventInfo *myev;
#endif

#if VDRVERSNUM >= 10300
	int search(const cSchedule *s,const class cEvent *ev);
#else
	int search(const cSchedule *s,const class cEventInfo *ev);
#endif

public:
#if VDRVERSNUM >= 10300
	cSearchMenu(const class cEvent *EventInfo);
#else
	cSearchMenu(const class cEventInfo *EventInfo);
#endif
#if VDRVERSNUM >= 10300
	const class cEvent *currentSelected(void);
#else
	const class cEventInfo *currentSelected(void);
#endif

	void searchIn(const cSchedule** schedArray,int schedArrayNum);
};

#endif
