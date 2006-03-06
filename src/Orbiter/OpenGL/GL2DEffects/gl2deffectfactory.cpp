//
// C++ Implementation: gl2deffectfactory
//
// Description: 
//
//
// Author: ciplogic <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gl2deffectfactory.h"

/**
 * Implemented effects	
 */
#include "gl2deffecttransit.h"
#include "gl2deffectbeziertranzit.h"
#include "gl2deffectslidefromleft.h"
#include "gl2deffectfadesfromtop.h"
#include "gl2deffectfadesfromunderneath.h"

namespace DCE {

GL2DEffectFactory::GL2DEffectFactory (DrawingWidgetsEngine* WidgetsEngine)
	: SnapshotTex(0),
	Widgets(WidgetsEngine) {
	gettimeofday(&StartTime, NULL);
}

int GL2DEffectFactory::MilisecondTimmer()
{
	timespec TimeNow;  
	gettimeofday(&TimeNow,NULL);
	timespec m_tInterval = TimeNow - StartTime;
	long tMilisecondsPassed = m_tInterval.tv_sec * 1000 + 
		m_tInterval.tv_nsec / 1000000;
	
	return tMilisecondsPassed;
}

GL2DEffect* GL2DEffectFactory::CreateEffect(int IDEffect, int TimeForComplete)
{
	GL2DEffect* Effect = NULL;
	switch (IDEffect)
	{
		case GL2D_EFFECT_TRANSIT:
			Effect = new GL2DEffectTransit(this, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANSIT:
			Effect = new GL2DBezierEffectTransit (this, TimeForComplete);
			break;
		case GL2D_EFFECT_SLIDE_FROM_LEFT:
			Effect = new GL2DEffectSlideFromLeft (this, TimeForComplete);
			break;
		case GL2D_EFFECT_FADES_FROM_TOP:
			Effect = new GL2DEffectFadesFromTop(this, TimeForComplete);
			break;
		case GL2D_EFFECT_FADES_FROM_UNDERNEATH:
			Effect = new GL2DEffectFadesFromUnderneath (this, TimeForComplete);
			break;
	}
	if(IDEffect >= GL2D_UNIQUE_EFFECT)
		ClearEffects();

	if (Effect != NULL)
		Effects.push_back(Effect);
	return Effect;
}

void GL2DEffectFactory::UpdateEffects()
{
	int CurrentTime = MilisecondTimmer();
	fflush(stdout);
	if(Effects.empty())
		return;
	
	vector<GL2DEffect*>::iterator Effect;
	
	//std::cout<<"void GL2DEffectFactory::UpdateEffects()"<<std::endl;
	
	for(Effect = Effects.begin(); Effect < Effects.end(); )
	{
		if(((*Effect)->Configured) &&((*Effect)->Stage(float(CurrentTime))>1) )
		{
			delete *Effect;
			Effect = Effects.erase(Effect);
		}
		else
			++Effect;
	}
	//std::cout<<"end of void GL2DEffectFactory::UpdateEffects()"<<std::endl;
	if (HasEffects())
	{
		vector<GL2DEffect*>::iterator Effect;	
		int CurrentTime = MilisecondTimmer();
		for(Effect = Effects.begin(); Effect != Effects.end(); ++Effect)
			(*Effect)->Paint(CurrentTime);
	}
}

void GL2DEffectFactory::ClearEffects()
{
	vector<GL2DEffect*>::iterator Effect;

	//std::cout<<"void GL2DEffectFactory::UpdateEffects()"<<std::endl;

	for(Effect = Effects.begin(); Effect < Effects.end(); )
			delete *Effect;

		Effects.clear();
}

void GL2DEffectFactory::Paint()
{
	//cout<<"/*GL2DEffectFactory::Paint()"<<endl;
	Widgets->Paint();
}

} // end of namespace DCE
