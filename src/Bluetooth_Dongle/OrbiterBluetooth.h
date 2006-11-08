#ifndef __ORBITER_BLUETOOTH_H__
#define __ORBITER_BLUETOOTH_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter/Orbiter.h"
//-----------------------------------------------------------------------------------------------------
#include "../VIPShared/MenuData.h"
#include "../VIPShared/PhoneConfig.h"
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

	PhoneConfig m_Cfg;

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

	void Initialize( GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0 );

	void Configure( PhoneConfig& cfg );
	void SelectedItem( string& sItemId );
	PhoneConfig& GetPhoneConfig( void ){ return m_Cfg; }
	void HandleMouseEvent( int iX, int iY, int EventType );
	void ShowMenu( );

protected:
	void GetMenuData( MenuData& data );
private:
	void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);
};
}
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITER_BLUETOOTH_H__ 
