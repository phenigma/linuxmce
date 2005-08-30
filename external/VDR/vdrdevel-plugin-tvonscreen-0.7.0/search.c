/*
 * search.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: search.c,v 1.3 2004/11/23 22:13:37 schmitzj Exp $
 *
 */

#include "search.h"
#include "magazine.h"

#if VDRVERSNUM >= 10300
EventItem::EventItem(const class cEvent *ev)
#else
EventItem::EventItem(const class cEventInfo *ev)
#endif
{
	char buf[200];
	const char *txt,*time1,*time2,*date,*chan;
	cChannel *channel;
		
	myev=ev;
#if VDRVERSNUM >= 10300
	channel = Channels.GetByChannelID(ev->ChannelID(), true);
#else
	channel = Channels.GetByChannelID(ev->GetChannelID(), true);
#endif
	time1=ev->GetTimeString();
	time2=ev->GetEndTimeString();
#if VDRVERSNUM >= 10308
	date=ev->GetDateString();
#else
	date=ev->GetDate();
#endif
#if VDRVERSNUM >= 10315
	chan=channel->ShortName(true);
#else
	chan=channel->Name();
#endif
#if VDRVERSNUM >= 10300
	txt=ev->Title();
#else
	txt=ev->GetTitle();
#endif

	snprintf(buf,sizeof(buf)-1,"%.6s %s - %s %s/%s",date,time1,time2,chan,txt);
	SetText(buf);
}

// -----------------------------------------------------------------------------

cSearchMenu::
#if VDRVERSNUM >= 10300
  cSearchMenu(const class cEvent *ev)
#else
  cSearchMenu(const class cEventInfo *ev)
#endif
 : cOsdMenu(tr("Search"))
{
	myev=ev;

	const char *title;
#if VDRVERSNUM >= 10300
	title=ev->Title();
#else
	title=ev->GetTitle();
#endif

	char txt[200];
	snprintf(txt,sizeof(txt),"%s '%s'...",tr("Search for"),title);
	SetTitle(txt);

	SetStatus(tr("Searching..."));
	Display();
#if VDRVERSNUM >= 10307
        Skins.Message(mtInfo, tr("Searching..."));
#else
	Interface->Status(tr("Searching..."));
	Interface->Flush();
#endif
#if VDRVERSNUM >= 10300
	const cSchedules* Schedules = cSchedules::Schedules(_schedulesLock);
#else
	const cSchedules* Schedules = cSIProcessor::Schedules(_mutexLock);
#endif

	const cSchedule *s;
#if VDRVERSNUM >= 10300
	s=Schedules->GetSchedule(ev->ChannelID());
#else
	s=Schedules->GetSchedule(ev->GetChannelID());
#endif

	int cc=search(s,ev);

	snprintf(txt,sizeof(txt),"%s '%s':",tr("Search for"),title);
	SetTitle(txt);
	if (cc>0)
	{
		SetStatus("");
		SetHelp(tr("Record"), tr("Details"), tr("in all"), tr("Back"));
	}
	else
	{
		SetStatus(tr("Nothing found!"));
		SetHelp(NULL, NULL, tr("in all"), tr("Back"));
	}
	Display();
}
#if VDRVERSNUM >= 10300
	const cEvent *cev=NULL;
#else
	const cEventInfo *cev=NULL;
#endif

int cSearchMenu::
#if VDRVERSNUM >= 10300
  search(const cSchedule *s,const class cEvent *ev)
#else
  search(const cSchedule *s,const class cEventInfo *ev)
#endif
{
	const char *title;
#if VDRVERSNUM >= 10300
	title=ev->Title();
#else
	title=ev->GetTitle();
#endif

	int cc=0;
	cev=s->GetPresentEvent();
	while(cev)
	{
		if (cev!=ev || tvonscreenCfg.showsearchinitiator)
		{
			const char *ctitle;
#if VDRVERSNUM >= 10300
			ctitle=cev->Title();
#else
			ctitle=cev->GetTitle();
#endif
			if (ctitle && title && strstr(ctitle,title)) // || strstr(title,ctitle))
			{
				Add(new EventItem(cev));
				cc++;
			}
		}
		cev=magazine::getNext(s,cev);
	}
	return cc;
}
void cSearchMenu::searchIn(const cSchedule** schedArray,int schedArrayNum)
{
	cChannel* channel;
	const char *title;
#if VDRVERSNUM >= 10300
	title=myev->Title();
#else
	title=myev->GetTitle();
#endif

	char txt[200];
	snprintf(txt,sizeof(txt),"%s '%s'...",tr("Search for"),title);
	SetTitle(txt);
	Clear();

	SetStatus(tr("Searching..."));
	Display();
#if VDRVERSNUM >= 10307
	Skins.Message(mtInfo, tr("Searching..."));
#else
	Interface->Status(tr("Searching..."));
	Interface->Flush();
#endif
	int cc=0;

	for(int i=0;i<schedArrayNum;i++)
	{
#if VDRVERSNUM >= 10300
		channel = Channels.GetByChannelID(schedArray[i]->ChannelID(), true);
#else
		channel = Channels.GetByChannelID(schedArray[i]->GetChannelID(), true);
#endif
#if VDRVERSNUM >= 10315
		snprintf(txt,sizeof(txt),"%s /%-20.20s",tr("Searching..."),channel->ShortName(true));
#else
		snprintf(txt,sizeof(txt),"%s /%-20.20s",tr("Searching..."),channel->Name());
#endif
#if VDRVERSNUM >= 10307
	        Skins.Message(mtInfo, txt);
#else
		Interface->Status(txt);
		Interface->Flush();
#endif
		cc+=search(schedArray[i],myev);
	}

	snprintf(txt,sizeof(txt),"%s '%s':",tr("Search for"),title);
	SetTitle(txt);
	if (cc>0)
	{
		SetStatus("");
		SetHelp(tr("Record"), tr("Details"), NULL, tr("Back"));
	}
	else
	{
		SetStatus(tr("Nothing found!"));
		SetHelp(NULL, NULL, NULL, tr("Back"));
	}
	Display();
}


#if VDRVERSNUM >= 10300
const class cEvent *cSearchMenu::currentSelected(void)
#else
const class cEventInfo *cSearchMenu::currentSelected(void)
#endif
{
	EventItem *item = (EventItem *)Get(Current());
	return item ? item->Event() : NULL;
}
