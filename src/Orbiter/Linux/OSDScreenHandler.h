#ifndef __OSD_SCREEN_HANLDER_H__
#define __OSD_SCREEN_HANLDER_H__

#include "../ScreenHandler.h"

class PositionCallBackData : public CallBackData
{
public:
	int x, y, w, h;
};

class OSDScreenHandler: public ScreenHandler
{
public:
	OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	~OSDScreenHandler();

	void SCREEN_FileSave(long PK_Screen, string sPrivate, string sPublic, string sCaption);
	bool FileSave_ObjectSelected(CallBackData *pData);
};

#endif

