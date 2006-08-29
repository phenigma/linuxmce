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
	bool Finished;


	void CopyDependencies(vector <string> Dependencies);
public:
	vector <string> Dependencies;
	string ObjectID;
	MeshFrame *BeforeGrid, *AfterGrid;

	AnimationScrollDatagrid(string ObjectID, OpenGL3DEngine* Engine, 
		MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel, vector <string> Dependencies);
	virtual ~AnimationScrollDatagrid(void);

	bool UpdateStartTime(int StartTime);

	int GetStartTime();

	bool DatagridDependenciesSatisfied(vector<AnimationScrollDatagrid*> &AnimationDatagrids);

	void StartAnimation();
	void StopAnimation();
	/**
	 *	returns true if the animation is ended	
	 */
	bool Update(bool ModifyGeometry);

};

#endif
