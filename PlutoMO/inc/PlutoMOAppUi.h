#ifndef PLUTOMOAPPUI_H
#define PLUTOMOAPPUI_H

#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>

#include "BD/BDCommandProcessor_Symbian_Bluetooth.h"

#include "PlutoVMCView.h"
//----------------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
class CPlutoMOContainer;
class CPlutoMOGridContainer;
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
	void CloseVMC() 
	{ 
		m_pVMCView->iContainer->MakeVisible(false);
		m_bVMCViewerVisible = false;
		
		if(m_iCapturedKeyId)
			CEikonEnv::Static()->RootWin().CancelCaptureKeyUpAndDowns(m_iCapturedKeyId);
	}

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

	void ResetViewer();
	
	BDCommandProcessor_Symbian_Bluetooth *m_pBDCommandProcessor;
	BDCommandProcessor_Symbian_Bluetooth *m_pBDCommandProcessor_Symbian_Bluetooth;
	
	bool m_bSendKeyStrokes;
	bool m_bVMCViewerVisible;
	bool m_bMakeVisibleAllowed;

	CPlutoVMCView* m_pVMCView;

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
