#ifndef __DATAGRID_ANIMATION_MANAGER_H__
#define __DATAGRID_ANIMATION_MANAGER_H__

#include <vector>
using namespace std;

class MeshFrame;
class OpenGL3DEngine;
class AnimationScrollDatagrid;

class DatagridAnimationManager
{
	//private data
	vector<AnimationScrollDatagrid *> m_vectCurrentAnimations;
	vector<AnimationScrollDatagrid *> m_vectPendingAnimations; 
	OpenGL3DEngine *m_pEngine;

	//internal events
	void OnStartAnimation();
	void OnPrepareForAnimation();
	void OnStopAnimation();

	//helper methods
	void Cleanup();
	bool AnimationsPrepared();
	

public:

	DatagridAnimationManager(OpenGL3DEngine *Engine) : m_pEngine(Engine) {}
	~DatagridAnimationManager() { /* todo : cleanup */}

	void PrepareForAnimation(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel, vector<string> Dependencies);

	void StopAnimations();

	void ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame);

	bool Update();
};

#endif //__DATAGRID_ANIMATION_MANAGER_H__
