#ifndef __MONSTER_SCREEN_HANLDER_H__
#define __MONSTER_SCREEN_HANLDER_H__

#include "../../Orbiter/ScreenHandler.h"
//-----------------------------------------------------------------------------------------------------
class PositionCallBackData : public CallBackData
{
public:
	int x, y, w, h;
};
//-----------------------------------------------------------------------------------------------------
class MonsterScreenHandler: public ScreenHandler
{
public:
	MonsterScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	~MonsterScreenHandler();

	bool Register() { return false; }

	void SCREEN_FileSave(long PK_Screen, string sDefaultUserValue, string sPrivate, string sPublic, string sCaption);
	bool FileSave_ObjectSelected(CallBackData *pData);
};
//-----------------------------------------------------------------------------------------------------
#endif

