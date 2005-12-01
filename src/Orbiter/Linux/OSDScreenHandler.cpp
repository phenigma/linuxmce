#include "OSDScreenHandler.h"
//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) :
	ScreenHandler(pOrbiter, p_MapDesignObj)
{
}
//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::~OSDScreenHandler()
{
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_FileSave(long PK_Screen, string sPrivate, string sPublic, string sCaption)
{
	ScreenHandler::SCREEN_FileSave(PK_Screen, sPrivate, sPublic, sCaption);

	//your stuff
	RegisterCallBack(cbObjectSelected, &ScreenHandler::FileSave_ObjectSelected, new PositionCallBackData());	
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileSave_ObjectSelected(CallBackData *pData)
{
	PositionCallBackData *pMyData = dynamic_cast<PositionCallBackData *>(pData);
	g_pPlutoLogger->Write(LV_WARNING, "FileSave_ObjectSelected, with position %d, %d, %d, %d",
			pMyData->x, pMyData->y, pMyData->w, pMyData->h);
}
//-----------------------------------------------------------------------------------------------------

