#include "PlutoMOAppUi.h"
#include "PlutoMOContainer.h" 
#include "PlutoMOGridContainer.h"
#include <PlutoMO.rsg>
#include "PlutoMO.hrh"

#include <avkon.hrh>

#include "PlutoVMCContainer.h"
#include "PlutoVMCUtil.h"
#include "PlutoVMCView.h"
#include "Logger.h"
#include "PlutoDefs.h"

//test
#include <http\rhttpsession.h>
#include <http\mhttptransactioncallback.h>
#include <uri8.h>
#include <e32base.h>
#include <http.h>
#include <chttpformencoder.h>

#include "httpclient.h"
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::ConstructL()
{
	m_pBDCommandProcessor_Symbian_Bluetooth = NULL;
	m_bSendKeyStrokes = false;
	m_bMakeVisibleAllowed = false;
	m_pVMCView = NULL;

	BaseConstructL();
    iAppContainer = new (ELeave) CPlutoMOContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );

	SymbianLogger *pLogger = new SymbianLogger(string("PlutoMO.log"), KCPlutoLoggerId, CCoeStatic::EApp);

	m_pBDCommandProcessor_Symbian_Bluetooth = 
		new BDCommandProcessor_Symbian_Bluetooth("", this);

	m_pBDCommandProcessor = m_pBDCommandProcessor_Symbian_Bluetooth;

	m_pBDCommandProcessor_Symbian_Bluetooth->Start();
	m_pBDCommandProcessor_Symbian_Bluetooth->Listen();
	m_pBDCommandProcessor_Symbian_Bluetooth->SetupSecurityManager();
	m_pBDCommandProcessor_Symbian_Bluetooth->AdvertiseThePlutoService();

	LOG("Waiting for connections...\n");


//	Hide();

	/*
	TUriParser8 uri;
	uri.Parse(_L8("news.yahoo.com"));

	PlutoHttpClient hc;
	hc.SendRequest(uri);*/
}
//----------------------------------------------------------------------------------------------
CPlutoMOAppUi::~CPlutoMOAppUi()
{
	if(m_iCapturedKeyId)
	{
		CEikonEnv::Static()->RootWin().CancelCaptureKeyUpAndDowns(m_iCapturedKeyId);	
		m_iCapturedKeyId = 0;
	}

	delete m_pBDCommandProcessor_Symbian_Bluetooth;
	m_pBDCommandProcessor_Symbian_Bluetooth = NULL;

	if (iAppContainer)
    {
	    RemoveFromStack( iAppContainer );
		delete iAppContainer;
    }
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }
//----------------------------------------------------------------------------------------------
TKeyResponse CPlutoMOAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
{
	m_pVMCView->iContainer->OfferKeyEvent(aKeyEvent, aType);

    return EKeyWasConsumed;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {

		case EAknSoftkeyOk:
//			iBluetoothHandler->PressedKey('o');
			break;
        case EAknSoftkeyExit:
//			iBluetoothHandler->PressedKey('q');
            break;
//        case EEikCmdExit:
//            Exit();
//            break; 


//       case EAknSoftkeyBack:
        case EEikCmdExit:
            {
			if(!m_bVMCViewerVisible)
				Exit();
            break;
            }
        // Have to change what it does currently
		// Opens the file 
		case EPlutoMOCmdAppTest1:
            {
            iAppContainer->CallLaunch();
			break;
            }
		// List view
        case EPlutoMOCmdAppTest2:
            {
			    //BaseConstructL();
				iGridContainer = new (ELeave) CPlutoMOGridContainer;
				//iGridContainer->SetMopParent(this);
				iGridContainer->ConstructL( ClientRect() );
				//AddToStackL( iGridContainer );
				break;
            }
		// Deleting a VMC file
        case EPlutoMOCmdAppTest3:
            {
            iAppContainer->CallDelete();
			iAppContainer->SetPlutoMO(EPlutoMOPictures, EPlutoMOSizeDateNoChange);
			break;
            }
		// Help Screen
        case EPlutoMOCmdAppTest4:
            {
            // Will not do anything now
            break;
            }

        default:
            break;      
        }
    }
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::CreateVMCView()
{
	if(m_pVMCView == NULL)
	{
		// Create and add views.
		m_pVMCView = new (ELeave) CPlutoVMCView();
		CleanupStack::PushL(m_pVMCView);
		m_pVMCView->ConstructL();
		CleanupStack::Pop();    // view1
		AddViewL(m_pVMCView);        // transfer ownership to CAknViewAppUi

		ActivateLocalViewL(m_pVMCView->Id());
	}
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::MakeViewerVisible(bool Value)
{
	if(Value)
	{
		m_pVMCView->iContainer->MakeVisible(true);

		if(!m_iCapturedKeyId)
			m_iCapturedKeyId = CEikonEnv::Static()->RootWin().CaptureKeyUpAndDowns(EStdKeyNo, 0, 0);
	}

	m_bVMCViewerVisible = true;
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::OpenImage(unsigned char Type, unsigned long Size, const char *Data) //fullscreen (!)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetImage(Type, Size, Data);

	m_pVMCView->Refresh();

	MakeViewerVisible(m_bMakeVisibleAllowed);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::OpenVMC(bool bParsed, TFileName& iFileName, 
								 VIPMenuCollection *pVMC)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	if(bParsed)
	{
		pVCMUtil->SetAndSwitchToMenu(pVMC);
	}
	else
		pVCMUtil->Parse(iFileName);

	m_pVMCView->Refresh();

	MakeViewerVisible(m_bMakeVisibleAllowed);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::ShowList(
	unsigned long x, 
	unsigned long y, 
	unsigned long Width, 
	unsigned long Height,
	RPointerArray<string> DatagridStringList
)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetList(x, y, Width, Height, DatagridStringList);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SetCaptureKeyboardCommand(
	bool bOnOff, 
	bool bDataGrid, 
	bool bReset, 
	bool bTypePin, 
	int  iVariable, 
	string sText
)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetCaptureKeyboardCommand(bOnOff, bDataGrid, bReset, bTypePin, iVariable, sText);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::SaveFile(
	long iFileNameSize, 
	const char *pFileName, 
	long iFileDataSize, 
	const char *pFileData
) 
{
	TFileName iFileName;
	
	RFile file;
	RFs   aFs;
	aFs.Connect();

	aFs.Delete(string(pFileName).Des());
	file.Create(aFs, string(pFileName).Des(), EFileStream | EFileWrite);

	HBufC8 *base_str = HBufC8::NewL(iFileDataSize);
	TPtr8 pStr = base_str->Des();

	for (unsigned int i = 0; i < iFileDataSize; i++)
		pStr.Append(TChar(pFileData[i]));

	file.Write(pStr, iFileDataSize);
	PLUTO_SAFE_DELETE(base_str);

	file.Close();

	aFs.Close();
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::UpdateScreen(VIPMenuCollection *pVMC)
{
	TFileName iDummy;

	OpenVMC(true, iDummy, pVMC);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::UpdateScreen(bool bParsed, const TDes8& aVmc, unsigned int uSize, 
								 VIPMenuCollection *pVMC)
{
	TFileName iFileName;
	
	if(!bParsed)
	{
		RFile file;
		RFs   aFs;
		aFs.Connect();

		HBufC8 *base_str = HBufC8::NewL(uSize);
		TPtr8 pStr = base_str->Des();

		for (unsigned int i = 0; i < uSize; i++)
			pStr.Append(TChar(aVmc[i]));

		HBufC16 *pPath = HBufC16::NewL(256);
		TPtr16 aPath = pPath->Des();

		file.Temp(aFs, aPath, iFileName, EFileStream | EFileWrite);
		file.Write(pStr, uSize);
		file.Close();

		delete base_str;
		base_str = NULL;

		delete pPath;
		pPath = NULL;

		aFs.Close();
	}

	OpenVMC(bParsed, iFileName, pVMC);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::Hide()
{
	TApaTask task(CEikonEnv::Static()->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.SendToBackground();

	m_bVMCViewerVisible = false;

	if(m_iCapturedKeyId)
	{
		CEikonEnv::Static()->RootWin().CancelCaptureKeyUpAndDowns(m_iCapturedKeyId);	
		m_iCapturedKeyId = 0;
	}
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::Show()
{
	TApaTask task(CEikonEnv::Static()->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.BringToForeground();

	m_bVMCViewerVisible = true;

	if(!m_iCapturedKeyId)
		m_iCapturedKeyId = CEikonEnv::Static()->RootWin().CaptureKeyUpAndDowns(EStdKeyNo, 0, 0);
}
//----------------------------------------------------------------------------------------------
void CPlutoMOAppUi::ResetViewer()
{
	LOG("Reseting list...\n");
	RPointerArray<string> Dummy;
	ShowList(-1, -1, -1, -1, Dummy);

	LOG("Reseting capture keyboard...\n");
	SetCaptureKeyboardCommand(false, false, true, false, 0, "");

	LOG("Reset Viewer complete!\n");
}
//----------------------------------------------------------------------------------------------
// End of File  

