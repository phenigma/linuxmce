#ifndef PLUTOMOAPPUI_H
#define PLUTOMOAPPUI_H

#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>

#include <coecntrl.h>
#include <etel.h>
#include <e32base.h>
#include <apparc.h>
#include <apgtask.h>
#include <apgcli.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>

_LIT(KPlutoVMCFile,"c:\\PlutoMO.vmc");
_LIT(KPlutoCfgFile,"c:\\PlutoMO.cfg");

#include "BD/BDCommandProcessor_Symbian_Bluetooth.h"
//----------------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
class CPlutoVMCView;
class CPlutoMOContainer;
class CPlutoMOGridContainer;
class BDCommandProcessor_Symbian_Bluetooth;

class MBluetoothListener;
//----------------------------------------------------------------------------------------------
class CGetCallerId;
class CIncomingCallWatcher;
//----------------------------------------------------------------------------------------------
#define MAX_PHONE_TYPES 100
#define MAX_EVENT_TYPES	10
//----------------------------------------------------------------------------------------------
class CPlutoPhoneTypes
{
public:
	int			iWAP_EventType;
	TBuf<32>	iPhoneNumber;
	TBuf<128>	iMessage;
	int			iHangUp;
};
//----------------------------------------------------------------------------------------------
class CPlutoMOAppUi : public CAknViewAppUi, public MBluetoothListener
{
public: 

    void ConstructL();

    ~CPlutoMOAppUi();

	CCoeEnv* CoeEnv() { return CEikonEnv::Static();}
	void UpdateScreen(bool bParsed, const TDes8& aVmc, unsigned int uSize, 
								 VIPMenuCollection *pVMC);
	void UpdateScreen(VIPMenuCollection *pVMC);
	void Hide();
	void Show();

	void OpenVMC(bool bParsed, TFileName& iFileName, 
								 VIPMenuCollection *pVMC); 
	void OpenImage(unsigned char Type, unsigned long Size, const char *Data);
	void CloseVMC();

	void ShowList(
		unsigned long x, 
		unsigned long y, 
		unsigned long Width, 
		unsigned long Height,
		RPointerArray<string> DatagridStringList
	);

	void SetCaptureKeyboardCommand(
		bool bOnOff, 
		bool bDataGrid, 
		bool bReset, 
		bool bTypePin, 
		int  iVariable, 
		string sText
	);

	void SaveFile(
		long iFileNameSize, 
		const char *pFileName, 
		long iFileDataSize, 
		const char *pFileData
	);

	void ResetViewer();
	
	BDCommandProcessor_Symbian_Bluetooth *m_pBDCommandProcessor;
	BDCommandProcessor_Symbian_Bluetooth *m_pBDCommandProcessor_Symbian_Bluetooth;
	
	bool m_bSendKeyStrokes;
	bool m_bVMCViewerVisible;
	bool m_bMakeVisibleAllowed;

	CPlutoVMCView* m_pVMCView;


	//incoming calls and stuff related
	void NotifyIncomingCall(TDesC& aCallName);
	void NotifyIncomingNumber(const TDesC& aTellNumber);
	void ReadConfigurationFile();
	void SetupIncomingCallNotifier();
	void LaunchBrowser();

	CGetCallerId *iGetCallerId;
	CIncomingCallWatcher *iIncomingCallWatcher;

	TInt iNumPhoneTypes, iNumEventTypes, iCurType;
	CPlutoPhoneTypes iPhoneTypes[MAX_PHONE_TYPES];
	TBuf<128> iPlutoEventTypes[MAX_EVENT_TYPES];
	TBuf<256> iURL;

	RCall			iCall;
	RLine			iLine;
	RPhone			iPhone;
	RTelServer		iTelServer;

	CIdle *iIdle;
	static TInt DoIdleStatic(TAny *aApp);
	TInt DoIdle();

private:
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    void HandleCommandL(TInt aCommand);

    virtual TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,TEventCode aType);

	void CreateVMCView();
	void MakeViewerVisible(bool Value);

private:
    CPlutoMOContainer* iAppContainer; 
	CPlutoMOGridContainer* iGridContainer; 

	TInt m_iCapturedKeyId;
 };
//----------------------------------------------------------------------------------------------
#endif

// End of File
