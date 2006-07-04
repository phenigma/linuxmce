/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef SoundUtils_H_
#define SoundUtils_H_

#include "AMixerOptions.h"

AMixerOptions* GetCurentMixerOptions();
bool GetControlLimits(char* ControlName, int& LowerBound, int& UpperBound, int& Value);
void SetControlLimits(char* ControlName, int Value);

#endif
