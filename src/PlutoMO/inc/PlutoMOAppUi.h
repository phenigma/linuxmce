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
//----------------------------------------------------------------------------------------------
//this version is must be synchronized with the one from Orbiter_Plugin.cpp
#define  VERSION "2005.04.18"
//----------------------------------------------------------------------------------------------
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
	string m_sLoggerFileName;
	string m_sVMCFolderFilter;
	string m_sAppFolder;
	string m_sPlutoVMC;
	string m_sPlutoConfig;
	string m_sPlutoEventPng;

public: 

    void ConstructL();

    ~CPlutoMOAppUi();
	void Cleanup();

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
		bool bTurnOn,
		unsigned long iSelectedItem
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
	void MakeViewerVisible(bool Value);
	void Disconnect();

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
	void CaptureSoftKeys();
	void CancelCaptureSoftKeys();

	void SetupPaths();
	char GetCurrentDrive();

    CPlutoMOContainer* iAppContainer; 
	CPlutoEventView* iPlutoEventView;

	TInt m_iCapturedKeyIdYes;
	TInt m_iCapturedKeyIdNo;
 };
//----------------------------------------------------------------------------------------------
#endif

// End of File
