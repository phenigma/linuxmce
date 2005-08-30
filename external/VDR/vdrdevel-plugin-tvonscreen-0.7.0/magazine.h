/*
 * magazine.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: magazine.h,v 1.15 2004/11/30 19:31:48 schmitzj Exp $
 *
 */

#ifndef MAGAZINE_H
#define MAGAZINE_H

#include <vdr/plugin.h>
#include <time.h>
#include "gfxtools.h"
#include "config.h"
#include "timer.h"
#include "search.h"

void mzlog(int level, const char *fmt, ...);
#define TL_YSTART 	48
// else
//#define TL_YSTART	24

#if VDRVERSNUM >= 10307

// #define MULTINAMES

typedef enum {
#ifdef MULTINAMES
	NAME1_AREA = 0,
	NAME2_AREA,
	NAME3_AREA,
#else
	NAMES_AREA = 0,
#endif
	TIMELINE_AREA,
	SCHED1_AREA,
	SCHED2_AREA,
	SCHED3_AREA,
	CONTROL_AREA,
	NUMBER_OF_AREAS,
} tMagazineArea;
#endif

class magazine : public cOsdObject
{
	cPlugin *parent;
#if VDRVERSNUM >= 10307
	cOsd *osd;
#else
	cOsdBase *osd;
#endif

	class tvOcMenuEvent *me;
	class cMenuEditTimer *met;
	class cSearchMenu *mes;
	
	anyFont *f1,*f2,*f3,*f4;

#if VDRVERSNUM < 10307
	tWindowHandle timeline;
	tWindowHandle names;
	tWindowHandle sched1,sched2,sched3;
	tWindowHandle control;
#endif

#if VDRVERSNUM >= 10300
	cSchedulesLock _schedulesLock;
#else
	cMutexLock _mutexLock;
#endif
	const cSchedule** schedArray;
	int schedArrayNum;
	int currentFirst;
	time_t currentFirstTime;

	int evnum;
#if VDRVERSNUM >= 10300
	class cEvent **ev1;
	class cEvent **ev2;
	class cEvent **ev3;
#else
	class cEventInfo **ev1;
	class cEventInfo **ev2;
	class cEventInfo **ev3;
#endif
	int *fullHours;
	int *fullHours_tmp1;
	int *fullHours_tmp2;
	int *fullHours_tmp3;

	enum modes {SHOW,EDIT};
	enum modes curmode;

	unsigned short EDIT_curEvent;
	int EDIT_curChannel;
	int EDIT_curEVI;
	
	bool timeline_tested;
	bool timeline_found_conflict;
	
#if VDRVERSNUM >= 10300
public:
	static const class cEvent *getNext(const cSchedule *s,const cEvent *e);
	static const class cEvent *getPrev(const cSchedule *s,const cEvent *e);
private:
	cEvent **ev4ch(int);
#else
public:
	static const class cEventInfo *getNext(const cSchedule *s,const cEventInfo *e);
	static const class cEventInfo *getPrev(const cSchedule *s,const cEventInfo *e);
private:
	cEventInfo **ev4ch(int);
#endif
	void searchcEvt();

	void printLogo(const cSchedule *s,int p);
	void printHead(const cSchedule *s,int p);
	void showKeys(void);
	void showTimeline(void);
	void showHeads(bool onlyBG=false);
	void showScheds(void);
#if VDRVERSNUM >= 10307
	void showSched(const cSchedule *s,cEvent **ev, tMagazineArea area);
	void calcSched(const cSchedule *s,cEvent **ev);
#elif VDRVERSNUM >= 10300
	void showSched(const cSchedule *s,cEvent **ev,tWindowHandle sched);
	void calcSched(const cSchedule *s,cEvent **ev);
#else
	void showSched(const cSchedule *s,cEventInfo **ev,tWindowHandle sched);
	void calcSched(const cSchedule *s,cEventInfo **ev);
#endif
	void calcScheds(void);

	void output(void);
	void outputLR(void);

	void gotoUsertime(int u);
	void showHelp(void);

#if VDRVERSNUM >= 10300
	void autoTimer(const class cEvent *EventInfo);
#else
	void autoTimer(const class cEventInfo *EventInfo);
#endif

public:
	magazine(class cPlugin *);
	virtual ~magazine();
	virtual void Show(void);
	virtual eOSState ProcessKey(eKeys Key);
};

#endif
