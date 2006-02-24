#ifndef GL2DEffect_H
#define GL2DEffect_H

namespace DCE {

class GL2DEffectFactory;
/**
 * That class is the base class for 2D effects using OpenGL
 */
class GL2DEffect
{
protected:
	GL2DEffectFactory* Effects;
	int Start, Length;

public:
	bool Configured;
	GL2DEffect(GL2DEffectFactory* EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffect();
	
	/**
	 * Return the stage of the effect, from 0.0 to 1.0
	 * 0 = StartTime
	 * 1.0 = StartTime+Length;
	 */
	float Stage(float Time) {
		return ((float)Time-(float)Start) / (float)Length;
	} 
  
	virtual void Paint(int Time) = 0;
};

}

#endif
