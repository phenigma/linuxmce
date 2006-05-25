#ifndef __ORBITERLINUX_H__
#define __ORBITERLINUX_H__

#include "../SDL/StartOrbiterSDL.h"
#include "../SDL/OrbiterSDL.h"
#include "OrbiterRendererSDLLinux.h"

#include <string>
using namespace std;

class OrbiterLinux : public Orbiter/*, public RatpoisonHandler<OrbiterLinux>*/
{
public:
	OrbiterLinux(int DeviceID,int PK_DeviceTemplate,
                 string ServerAddress, string sLocalDirectory,
                 bool bLocalMode, int nImageWidth, int nImageHeight);

    virtual ~OrbiterLinux();


	// overridden to handle turning on and off the mouse pointer
	virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Activate_Window(string sWindowName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage);

    virtual void CMD_Surrender_to_OS(string sOnOff, bool bFully_release_keyboard, string &sCMD_Result, Message *pMessage);



	virtual class ScreenHandler *CreateScreenHandler();

        OrbiterRendererSDLLinux * OrbiterLinux::ptrRendererSDL();

};

#endif // __ORBITERLINUX_H__
