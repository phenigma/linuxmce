/**
 *  That class stores the 'amixer' command settings
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef AMIXER_OPTIONS_H
#define AMIXER_OPTIONS_H

#include <vector>

#include "SettingsDictionary.h"
/**
 *	Singleton class which keeps the options of AMixer command
 *	based on Dictionary
 */
class AMixerOptions
{
	/**
	 *	Keept the mixer options	
	 */
	SettingsDictionary Options;

	static AMixerOptions* Instance;
public:
	AMixerOptions();

	static AMixerOptions* GetInstance();

	/**
	 *	Cleanup resources attached with singleton class	
	 */
	void CleanUp();
	
	/**
	 *	Virtual destructor	
	 */
	virtual ~AMixerOptions(void);

	void AddOption(char* OptionName);

	bool Exists(char* OptionName);

};

#endif
