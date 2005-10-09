/*
 * timer.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: timer.c,v 1.2 2004/11/23 14:21:24 schmitzj Exp $
 *
 */

#include "magazine.h"

tvOcMenuEvent::
#if VDRVERSNUM >= 10300
  tvOcMenuEvent(const class cEvent *EventInfo)
#else
  tvOcMenuEvent(const class cEventInfo *EventInfo)
#endif
 : cMenuEvent(EventInfo,false)
{
	helpLine();
}

void tvOcMenuEvent::printMsg(const char *s)
{
	SetStatus(s);
}
void tvOcMenuEvent::helpLine(bool full)
{
	SetHelp(tr("Record"), tr("Search"), full?tr("AutoTimer"):NULL, NULL);
}
