#ifndef GL2DEffect_H
#define GL2DEffect_H

#include "../math3dutils.h"

/**
 * That class is the base class for 2D effects using OpenGL
 */
namespace GLEffect2D
{
class EffectFactory;

class Effect
{
protected:
	EffectFactory* Effects;
	int Start, Length;

public:
	bool Configured;
	bool Reversed;

	Effect(EffectFactory* EffectsEngine, int StartAfter, int TimeForCompleteEffect);
	virtual ~Effect();
	
	/**
	 * Return the stage of the effect, from 0.0 to 1.0
	 * 0 = StartTime
	 * 1.0 = StartTime+Length;
	 */
	float Stage(float Time);
  
	virtual void Paint(int Time) = 0;

	virtual void Configure(PlutoRectangle* EffectSourceSize) = 0;
	
	//virtual void Configure(PlutoRectangle* EffectSourceSize) = 0;
};

}

#endif
