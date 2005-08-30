/*
 * config.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: config.h,v 1.7 2004/11/23 14:21:24 schmitzj Exp $
 *
 */

#ifndef TVONSCREEN_CONFIG_H
#define TVONSCREEN_CONFIG_H

#include <vdr/config.h>
#include <vdr/menuitems.h>

class tvonscreenConfig
{
public:

	tvonscreenConfig(void);
	~tvonscreenConfig();
	bool SetupParse(const char *Name, const char *Value);
	bool ProcessArgs(int argc, char *argv[]);
	const char *CommandLineHelp(void);

//  char showChannels[];
	int showLogos;
	int XLfonts;
	int noInfoLine;
	int showChannels;
	int bwlogos;
	int colorworkaround;
	int usertime1;
	int usertime2;
	int usertime3;
	int thenshownextday;
	int showsearchinitiator;

	char *logos;
	char *vdradminfile;
};

extern tvonscreenConfig tvonscreenCfg;

class tvonscreenConfigPage : public cMenuSetupPage
{
private:
	tvonscreenConfig m_NewConfig;
	
protected:
	virtual void Store(void);

public:
	tvonscreenConfigPage(void);
	virtual ~tvonscreenConfigPage();
};

#endif 


