#ifndef PLUTOBTAPPUI_H
#define PLUTOBTAPPUI_H

#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>

#include "BTCommandProcessor_Symbian_Bluetooth.h"

#include "PlutoVMCView.h"
//----------------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
class CPlutoBTContainer;
class CPlutoBTGridContainer;
//----------------------------------------------------------------------------------------------
class CPlutoBTAppUi : public CAknViewAppUi, public MBluetoothListener
{
public: 

    void ConstructL();

    ~CPlutoBTAppUi();

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
	
	BTCommandProcessor_Symbian_Bluetooth *m_pBTCommandProcessor;
	BTCommandProcessor_Symbian_Bluetooth *m_pBTCommandProcessor_Symbian_Bluetooth;
	
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
    CPlutoBTContainer* iAppContainer; 
	CPlutoBTGridContainer* iGridContainer; 

	TInt m_iCapturedKeyId;
 };
//----------------------------------------------------------------------------------------------
#endif

// End of File
