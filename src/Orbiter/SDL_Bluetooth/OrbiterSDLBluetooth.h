#ifndef __OrbiterSDLBluetooth_H__
#define __OrbiterSDLBluetooth_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 

#include <SDL.h>
//-----------------------------------------------------------------------------------------------------
#include "../SDL/OrbiterSDL.h"
//-----------------------------------------------------------------------------------------------------

class BDCommandProcessor;

//-----------------------------------------------------------------------------------------------------
namespace DCE
{

class OrbiterSDLBluetooth : public OrbiterSDL
{
	bool  m_bShowListSent;

	BDCommandProcessor *m_pBDCommandProcessor;

public:
	OrbiterSDLBluetooth(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, int nImageWidth, int nImageHeight);
	virtual ~OrbiterSDLBluetooth();

	// Public virtual methods
	virtual void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);
	virtual void RenderDataGrid(DesignObj_DataGrid *pObj);

	virtual void BeginPaint();
	virtual void EndPaint();

	virtual void SimulateMouseClick(int x, int y);
	virtual void SimulateKeyPress(long key);


private:
	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);
};
}
//-----------------------------------------------------------------------------------------------------
#endif //__OrbiterSDLBluetooth_H__ 
