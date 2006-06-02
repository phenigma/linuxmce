
#include "gl2deffect.h"

GL2DEffect::GL2DEffect(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: Effects(EffectsEngine),
	  Start(0),
	  Length(TimeForCompleteEffect),
	  Configured(false) {
}

GL2DEffect::~GL2DEffect() {
}

