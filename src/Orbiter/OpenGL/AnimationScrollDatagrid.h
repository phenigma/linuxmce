#ifndef AnimationScrollDatagrid_H_
#define AnimationScrollDatagrid_H_

#include "../../SerializeClass/ShapesColors.h"

class MeshFrame;
class OpenGL3DEngine;

class AnimationScrollDatagrid
{
	int MilisecondTime;
	int Direction;
	int StartTime;
	OpenGL3DEngine* Engine;
	float MaxAlpha;

public:
	MeshFrame *BeforeGrid, *AfterGrid;
	AnimationScrollDatagrid(OpenGL3DEngine* Engine, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel);
	virtual ~AnimationScrollDatagrid(void);

	void StartAnimation();
	/**
	 *	returns true if the animation is ended	
	 */
	bool Update();

};

#endif
