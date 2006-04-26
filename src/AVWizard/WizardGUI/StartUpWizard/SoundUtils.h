//
// C++ Interface: SoundUtils
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SoundUtils_H_
#define SoundUtils_H_

#include "AMixerOptions.h"

AMixerOptions* GetCurentMixerOptions();
bool GetControlLimits(char* ControlName, int& LowerBound, int& UpperBound, int& Value);
void SetControlLimits(char* ControlName, int Value);

#endif
