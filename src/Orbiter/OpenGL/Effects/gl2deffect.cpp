
#include "gl2deffect.h"

namespace GLEffect2D 
{

Effect::Effect(
			   EffectFactory * EffectsEngine, 
			   int StartAfter, int TimeForCompleteEffect)
	: Effects(EffectsEngine),
	  Start(StartAfter),
	  Length(TimeForCompleteEffect),
	  Configured(false),
	  Reversed(false)
{
}

Effect::~Effect() {
}

float Effect::Stage(float Time) {
	if (Length<= 0) 
		return 1.1f;
	if(Time < Start)
		return -0.0f;
	if(Time > Start + Length)
		return 1.05f;
	float Result = ((float)Time-(float)Start) / (float)Length;
	if(Reversed)
		Result = 1.0f - Result;
	return Result;
} 


}