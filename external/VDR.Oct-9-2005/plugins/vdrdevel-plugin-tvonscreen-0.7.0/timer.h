/*
 * timer.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: timer.h,v 1.3 2004/11/24 17:05:27 schmitzj Exp $
 *
 */

#ifndef TIMER_H
#define TIMER_H

#include <vdr/plugin.h>
#include <time.h>
#include "gfxtools.h"
#include "config.h"

#if VDRVERSNUM >= 10300
class cMenuEvent : public cOsdMenu {
private:
  const cEvent *event;
public:
  cMenuEvent(const cEvent *Event, bool CanSwitch = false);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
};
#else
class cMenuEvent : public cOsdMenu {
private:
  const class cEventInfo *eventInfo;
public:
  cMenuEvent(const class cEventInfo *EventInfo, bool CanSwitch = false);
  cMenuEvent(bool Now);
  virtual eOSState ProcessKey(eKeys Key);
};
#endif

class cMenuEditTimer : public cOsdMenu {
private:
  cTimer *timer;
  cTimer data;
  int channel;
  bool addIfConfirmed;
  class cMenuEditDateItem *firstday;
  void SetFirstDayItem(void);
public:
  cMenuEditTimer(cTimer *Timer, bool New = false);
  virtual ~cMenuEditTimer();
  virtual eOSState ProcessKey(eKeys Key);
  };

class tvOcMenuEvent : public cMenuEvent {
public:
#if VDRVERSNUM >= 10300
	tvOcMenuEvent(const class cEvent *EventInfo);
#else
	tvOcMenuEvent(const class cEventInfo *EventInfo);
#endif
	void printMsg(const char *);
	void helpLine(bool full=true);
};


#endif
