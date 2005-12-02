#include "MonsterScreenHandler.h"
//-----------------------------------------------------------------------------------------------------
MonsterScreenHandler::MonsterScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) :
	ScreenHandler(pOrbiter, p_MapDesignObj)
{
}
//-----------------------------------------------------------------------------------------------------
MonsterScreenHandler::~MonsterScreenHandler()
{
}
//-----------------------------------------------------------------------------------------------------
void MonsterScreenHandler::SCREEN_FileSave(long PK_Screen, string sDefaultUserValue, string sPrivate, 
										   string sPublic, string sCaption)
{
	ScreenHandler::SCREEN_FileSave(PK_Screen, sDefaultUserValue, sPrivate, sPublic, sCaption);

	//your stuff
	RegisterCallBack(cbObjectSelected, &ScreenHandler::FileSave_ObjectSelected, new PositionCallBackData());	
}
//-----------------------------------------------------------------------------------------------------
bool MonsterScreenHandler::FileSave_ObjectSelected(CallBackData *pData)
{
	PositionCallBackData *pMyData = (PositionCallBackData *)pData;
	g_pPlutoLogger->Write(LV_WARNING, "FileSave_ObjectSelected, with position %d, %d, %d, %d",
			pMyData->x, pMyData->y, pMyData->w, pMyData->h);

	return false;
}
//-----------------------------------------------------------------------------------------------------

