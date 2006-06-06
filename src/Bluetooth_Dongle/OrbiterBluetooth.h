#ifndef __ORBITER_BLUETOOTH_H__
#define __ORBITER_BLUETOOTH_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter/Orbiter.h"
//-----------------------------------------------------------------------------------------------------
class BDCommandProcessor;
//-----------------------------------------------------------------------------------------------------
namespace DCE
{

class OrbiterBluetooth : public Orbiter
{
	friend class DataGridRenderer_Bluetooth;

	bool  m_bShowListSent;
    int   m_ImageQuality;

	BDCommandProcessor *m_pBDCommandProcessor;

public:
	OrbiterBluetooth(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
        int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
        int nImageWidth, int nImageHeight, pluto_pthread_mutex_t *pExternalScreenMutex);
	~OrbiterBluetooth();

	void SimulateMouseClick(int x, int y);
	void SimulateKeyPress(long key);

    void OnReload();
    bool GetConfig();
	int ImageQuality() { return m_ImageQuality; }

    void SetImageQuality(unsigned long nImageQuality);
	void ImageGenerated(const string& csImageFileName);

private:
	void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);
};
}
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITER_BLUETOOTH_H__ 
