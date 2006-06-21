
#include "gl2deffect.h"

namespace GLEffect2D 
{

Effect::Effect(EffectFactory * EffectsEngine, int StartAfter, int TimeForCompleteEffect)
	: Effects(EffectsEngine),
	  Start(StartAfter),
	  Length(TimeForCompleteEffect),
	  Configured(false) {
}

Effect::~Effect() {
}

}