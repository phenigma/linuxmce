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

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
#define  VERSION "<=version=>"
const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/


_LIT(KPlutoVMCFile,"C:\\system\\apps\\PlutoMO\\PlutoMO.vmc");

#include "BD/BDCommandProcessor_Symbian_Bluetooth.h"
//----------------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
class CPlutoVMCView;
class CPlutoMOContainer;
class CPlutoEventView;
class BDCommandProcessor_Symbian_Bluetooth;

class MBluetoothListener;
//----------------------------------------------------------------------------------------------
class CGetCallerId;
class CIncomingCallWatcher;
//----------------------------------------------------------------------------------------------
#define MAX_PHONE_TYPES 100
#define MAX_EVENT_TYPES	10
//----------------------------------------------------------------------------------------------
#define SCREEN_WIDTH  176
#define SCREEN_HEIGHT 208 
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
	void UpdateScreen(	
		bool bStore, 
		unsigned long iVMCSize, 
		const char* pVMC,
		unsigned long iVMCFileNameSize, 
		const char* pVMCFileName
	);

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
		RPointerArray<string> DatagridStringList,
		bool bSendSelectedOnMove,
		bool bTurnOn
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

	void SimulateEvent(long eventType, long key);

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

	RCall			iCall;
	RLine			iLine;
	RPhone			iPhone;
	RTelServer		iTelServer;

	CIdle *iIdle;
	static TInt DoIdleStatic(TAny *aApp);
	TInt DoIdle();

	TInt iNumPhoneTypes, iNumEventTypes, iCurType;
	CPlutoPhoneTypes iPhoneTypes[MAX_PHONE_TYPES];
	TBuf<128> iPlutoEventTypes[MAX_EVENT_TYPES];
	TBuf<256> iURL;
	TBuf<256> iURLClone;

	bool m_bPlutoEventVisible;

private:
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    void HandleCommandL(TInt aCommand);

    virtual TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,TEventCode aType);

	void CreateVMCView();
	void MakeViewerVisible(bool Value);

    CPlutoMOContainer* iAppContainer; 
	CPlutoEventView* iPlutoEventView;

	TInt m_iCapturedKeyId;
 };
//----------------------------------------------------------------------------------------------
#endif

// End of File
