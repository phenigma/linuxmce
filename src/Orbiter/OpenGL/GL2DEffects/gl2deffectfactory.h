#ifndef GL2DEffectFactory_H
#define GL2DEffectFactory_H

#include "PlutoUtils/MultiThreadIncludes.h"

/**
 * Base class definition
 */
#include "gl2deffect.h"

#include "../GL2DWidgets/DrawingWidgetsEngine.h"
#include "../GL2DWidgets/basicwindow.h"

/**
 * Effects IDs
 */
#define GL2D_BASE_EFFECT	1000

#define GL2D_UNIQUE_EFFECT	2000

#define GL2D_EFFECT_TRANSIT 	(GL2D_UNIQUE_EFFECT+1)
#define GL2D_EFFECT_BEZIER_TRANSIT 	(GL2D_UNIQUE_EFFECT+2)

namespace DCE {

/**
 * Manager for user effects
 * Management is splited in 4 categories
 * - Creation
 * - Deletion
 * - Update (the effect status)
 * - Paint
 * If one effect is named Unique (and has ID>GL2D_UNIQUE_EFFECT), the operations are next:
 * It saves "a snapshot" of the actual image and store it as a SnapshotTex
 * 
 */
class GL2DEffectFactory {
	/**
	 * Internal time, used to know when the 2D effects start, used for sincronisation
	 */
	timespec StartTime;
	/**
	 * list of current effects
	 * if the number of effects = 0 then 
	 * will be displayed the desktop with no change
	 */
	vector <GL2DEffect*> Effects;
	
	/**
	 * Screenshot of the actual display
	 */
	OpenGLTexture SnapshotTex;
public:
	GL2DEffectFactory (DrawingWidgetsEngine* Widgets);
	
	/**
	 * It gives the time from staring the effects GL2D engine in milisecond
	 */
	int MilisecondTimmer();
	/**
	 * The widgets define the drawing operations of the GUI
	 */
	DrawingWidgetsEngine* Widgets;
	/**
	 * Creation of one effect
	 */
	GL2DEffect* CreateEffect(int IDEffect, int TimeForComplete); 
	/**
	 * It tests if effects are enable, and if doesn't they are deleted from effect list
	 */
	void UpdateEffects();
	/**
	 * Deletion of one all effects, used for fast disabling effects
	 */
	void ClearEffects();
	/**
	 * Paint the desktop using current effects
	 */
	void Paint();
	/**
	 * It may be used for testing if the drawing may idle
	 */
	bool HasEffects() {
		return Effects.size()!= 0;
	} 
	
	TBasicWindow * Screen3D;
	
};

}

#endif
