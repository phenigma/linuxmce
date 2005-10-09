/*
 * config.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: config.c,v 1.11 2005/01/02 12:19:32 schmitzj Exp $
 *
 */

#include "config.h"
#include "magazine.h"

#include <getopt.h>
#include <unistd.h>
#include <vdr/menuitems.h>

tvonscreenConfig tvonscreenCfg;

#if VDRVERSNUM < 10307
class cMenuEditTimeItem : public cMenuEditItem {
protected:
  int *value;
  int hh, mm;
  int pos;
  virtual void Set(void);
public:
  cMenuEditTimeItem(const char *Name, int *Value);
  virtual eOSState ProcessKey(eKeys Key);
  };
#endif

tvonscreenConfig::tvonscreenConfig(void)
{
	showLogos=false;
	XLfonts=true;
	noInfoLine=false;
	showChannels=true;
#if VDRVERSNUM >= 10307
	bwlogos=false;
#else
	bwlogos=true;
#endif
	colorworkaround=true;

	usertime1=1200;
	usertime2=1800;
	usertime3=2015;
	thenshownextday=true;
	showsearchinitiator=true;
	
	logos=NULL;
	vdradminfile=NULL;
}
tvonscreenConfig::~tvonscreenConfig()
{
	if (logos)
		free(logos);
	if (vdradminfile)
		free(vdradminfile);
}

bool tvonscreenConfig::SetupParse(const char *Name, const char *Value)
{
	if      (strcmp(Name,"showLogos")==0) showLogos = atoi(Value);
	else if (strcmp(Name,"XLfonts")==0) XLfonts = atoi(Value);
	else if (strcmp(Name,"noInfoLine")==0) noInfoLine = atoi(Value);
	else if (strcmp(Name,"showChannels")==0) showChannels = atoi(Value);
	else if (strcmp(Name,"bwlogos")==0) bwlogos = atoi(Value);
	else if (strcmp(Name,"colorworkaround")==0) colorworkaround = atoi(Value);
	else if (strcmp(Name,"usertime1")==0) usertime1 = atoi(Value);
	else if (strcmp(Name,"usertime2")==0) usertime2 = atoi(Value);
	else if (strcmp(Name,"usertime3")==0) usertime3 = atoi(Value);
	else if (strcmp(Name,"thenshownextday")==0) thenshownextday = atoi(Value);
	else if (strcmp(Name,"showsearchinitiator")==0) showsearchinitiator = atoi(Value);
	else 
		return false;

#if VDRVERSNUM < 10307
	bwlogos=true;
#endif
	return true;
}
const char *tvonscreenConfig::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
	return "  -l PathToLogos\n  --logos=PathToLogos\n  -v vdradmind.at\n  --vdradminfile=vdradmind.at\n";
}

bool tvonscreenConfig::ProcessArgs(int argc, char *argv[])
{ 
	static struct option long_options[] = {
		{ "logos",       required_argument, NULL, 'l' },
		{ "vdradminfile",required_argument, NULL, 'v' },
		{ NULL }
	};

	bool retval=true;
	int c;
	while ((c = getopt_long(argc, argv, "l:v:", long_options, NULL)) != -1)
	{
		switch (c)
		{
			case 'l': 
				if (logos)
				{
					free(logos);
					logos=NULL;
				}
				logos = strdup(optarg);
				retval=true;
				break;
			case 'v':
				if (vdradminfile)
				{
					free(vdradminfile);
					vdradminfile=NULL;
				}
				vdradminfile = strdup(optarg);
				retval=true;
				break;
			default:  
				break;
		}
	}
	return retval;
}

// ----------------------------------------------------------------------

tvonscreenConfigPage::tvonscreenConfigPage(void) : cMenuSetupPage()
{
	m_NewConfig = tvonscreenCfg;

#if TL_YSTART == 48
	Add(new cMenuEditBoolItem(tr("show channel logos"),
			&m_NewConfig.showLogos));
	Add(new cMenuEditBoolItem(tr("show channel names"),
			&m_NewConfig.showChannels));
	Add(new cMenuEditBoolItem(tr("show logos in black&white"),
			&m_NewConfig.bwlogos));
	Add(new cMenuEditBoolItem(tr("enable color problem work around"),
			&m_NewConfig.colorworkaround));
#endif
	Add(new cMenuEditBoolItem(tr("use XL fonts"),
			&m_NewConfig.XLfonts));
	Add(new cMenuEditBoolItem(tr("hide info line"),
			&m_NewConfig.noInfoLine));

	Add(new cMenuEditTimeItem(tr("user point in time 1 (Key 4)"),
			&m_NewConfig.usertime1));
	Add(new cMenuEditTimeItem(tr("user point in time 2 (Key 5)"),
			&m_NewConfig.usertime2));
	Add(new cMenuEditTimeItem(tr("user point in time 3 (Key 6)"),
			&m_NewConfig.usertime3));

	Add(new cMenuEditBoolItem(tr("jump to next day point if ago"),
			&m_NewConfig.thenshownextday));

	Add(new cMenuEditBoolItem(tr("Show search item itself"),
			&m_NewConfig.showsearchinitiator));


}

tvonscreenConfigPage::~tvonscreenConfigPage()
{
}

void tvonscreenConfigPage::Store(void)
{
	SetupStore("showLogos", m_NewConfig.showLogos);
	SetupStore("XLfonts", m_NewConfig.XLfonts);
	SetupStore("noInfoLine", m_NewConfig.noInfoLine);
	SetupStore("showChannels", m_NewConfig.showChannels);
	SetupStore("bwlogos", m_NewConfig.bwlogos);
	SetupStore("colorworkaround", m_NewConfig.colorworkaround);
	SetupStore("usertime1", m_NewConfig.usertime1);
	SetupStore("usertime2", m_NewConfig.usertime2);
	SetupStore("usertime3", m_NewConfig.usertime3);
	SetupStore("thenshownextday", m_NewConfig.thenshownextday);
	SetupStore("showsearchinitiator", m_NewConfig.showsearchinitiator);

	tvonscreenCfg = m_NewConfig;
}
