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
#include "GL2DEffectBlending.h"

//basic transit effect
#include "GL2DEffectTransitionNoEffect.h"

//bezier based effects
#include "gl2deffectbeziertranzit.h"
#include "gl2deffectbeziertranzit_topleft.h"
#include "gl2deffectbeziertranzit_prism.h"
#include "gl2deffectbeziertranzit_flow_slideleft.h"
#include "gl2deffectbeziertranzit_flow_slideleft_reverse.h"

//strict 2D planes effects
#include "gl2deffectslidefromleft.h"
#include "gl2deffectfadesfromtop.h"
#include "gl2deffectfadesfromunderneath.h"
#include "GL2DEffectNoEffect.h"
#include "GL2DEffectHighLightArea.h"
#include "gl2deffectselectedarea.h"
#include "DCE/Logger.h"


#include "../../../pluto_main/Define_Effect.h"

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
	if(!Effects.empty())
	{
		vector<GL2DEffect*>::iterator Effect = Effects.begin();
		if(NULL != dynamic_cast<GL2DEffectTransit*>(*Effect))
			return NULL;
	}
	GL2DEffect* Effect = NULL;
	switch (IDEffect)
	{
		case GL2D_EFFECT_TRANSIT_NO_EFFECT:
			Effect = new GL2DEffectTransitionNoEffect(this, TimeForComplete);
			break;
	
		case GL2D_EFFECT_TRANSIT:
			Effect = new GL2DEffectBlending(this, TimeForComplete);
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
		case GL2D_EFFECT_NOEFFECT:
			Effect = new GL2DEffectNoEffect(this, TimeForComplete);
			break;
		case GL2D_EFFECT_HIGHLIGHT_AREA:
			Effect = new GL2DEffectHighLightArea(this, TimeForComplete);
			break;
		case GL2D_EFFECT_SELECT_AREA:
			Effect = new GL2DEffectSelectedArea(this, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANSIT_PRISM:
			Effect = new GL2DBezierEffectTransit_Prism(this, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANSIT_TOPLEFT:
			Effect = new GL2DBezierEffectTransit_TopLeft(this, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT:
			Effect = new GL2DBezierEffectTransit_Flow_SlideLeft(this, TimeForComplete);
			break;
		case GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT_REVERSE:
			Effect = new GL2DBezierEffectTransit_Flow_SlideLeft_Reverse(this, TimeForComplete);
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
		for(Effect = Effects.begin(); HasEffects() && (Effect != Effects.end()); ++Effect)
		{
			(*Effect)->Paint(CurrentTime);
		}
	}
}

void GL2DEffectFactory::ClearEffects()
{
	if (Effects.size() == 0)
		return;

	vector<GL2DEffect*>::iterator Effect;
 
	for(Effect = Effects.begin(); Effect < Effects.end(); ++Effect)
		delete *Effect;

	Effects.clear();
}

void GL2DEffectFactory::Paint()
{
	Widgets->Paint();
}

int GL2DEffectFactory::GetEffectCode(int DBEffectCode)
{
	switch(DBEffectCode) {
		//transit
		case EFFECT_No_transit_effect_CONST:
			return GL2D_EFFECT_TRANSIT_NO_EFFECT;
		case EFFECT_Basic_transit_effect_CONST:
			return GL2D_EFFECT_TRANSIT;
		case EFFECT_Bezier_effect_CONST:
			return GL2D_EFFECT_BEZIER_TRANSIT;
		case EFFECT_Slide_from_left_CONST:
			return GL2D_EFFECT_SLIDE_FROM_LEFT;
		case EFFECT_Slide_from_top_CONST:
			return GL2D_EFFECT_TRANSIT_NO_EFFECT; //IMPLEMENT ME!
		case EFFECT_Fades_from_top_CONST:
			return GL2D_EFFECT_FADES_FROM_TOP;
		case EFFECT_Fades_from_underneath_CONST:
			return GL2D_EFFECT_FADES_FROM_UNDERNEATH;
		case EFFECT_Bezier_transit_prism_CONST:
			return GL2D_EFFECT_BEZIER_TRANSIT_PRISM;
		case EFFECT_Bezier_transit_top_left_CONST:
			return GL2D_EFFECT_BEZIER_TRANSIT_TOPLEFT;
		case EFFECT_Bezier_transit_flow_slide_left_CONST:
			return GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT;
		case EFFECT_No_select_or_hightlight_effect_CONST:
			return GL2D_EFFECT_BEZIER_TRANZIT_FLOW_SLIDELEFT_REVERSE;

		//select and highlight
		case EFFECT_Basic_highlight_effect_CONST:
			return GL2D_EFFECT_HIGHLIGHT_AREA;
		case EFFECT_Basic_select_effect_CONST:
			return GL2D_EFFECT_SELECT_AREA;
 	}

	return 0;
}
