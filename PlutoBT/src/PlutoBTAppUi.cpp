#include "PlutoBTAppUi.h"
#include "PlutoBTContainer.h" 
#include "PlutoBTGridContainer.h"
#include <PlutoBT.rsg>
#include "PlutoBT.hrh"

#include <avkon.hrh>

#include "PlutoVMCContainer.h"
#include "PlutoVMCUtil.h"
#include "PlutoVMCView.h"
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::ConstructL()
{
	m_pBDCommandProcessor_Symbian_Bluetooth = NULL;
	m_bSendKeyStrokes = false;
	m_bMakeVisibleAllowed = false;
	m_pVMCView = NULL;

    BaseConstructL();
    iAppContainer = new (ELeave) CPlutoBTContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );

	m_pBDCommandProcessor_Symbian_Bluetooth = 
		new BDCommandProcessor_Symbian_Bluetooth("", this);

	m_pBDCommandProcessor = m_pBDCommandProcessor_Symbian_Bluetooth;

	m_pBDCommandProcessor_Symbian_Bluetooth->Start();
	m_pBDCommandProcessor_Symbian_Bluetooth->Listen();
	m_pBDCommandProcessor_Symbian_Bluetooth->SetupSecurityManager();
	m_pBDCommandProcessor_Symbian_Bluetooth->AdvertiseThePlutoService();

	Hide();

	CEikonEnv::Static()->RootWin().CaptureKeyUpAndDowns(EStdKeyNo, 0, 0);
}
//----------------------------------------------------------------------------------------------
CPlutoBTAppUi::~CPlutoBTAppUi()
    {
	delete m_pBDCommandProcessor_Symbian_Bluetooth;
	m_pBDCommandProcessor_Symbian_Bluetooth = NULL;
    
	if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }
//----------------------------------------------------------------------------------------------
TKeyResponse CPlutoBTAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
{
	//User::Beep(500, 100000);

	m_pVMCView->iContainer->OfferKeyEvent(aKeyEvent, aType);

    return EKeyWasConsumed;
}
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::HandleCommandL(TInt aCommand)
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
		case EPlutoBTCmdAppTest1:
            {
            iAppContainer->CallLaunch();
			break;
            }
		// List view
        case EPlutoBTCmdAppTest2:
            {
			    //BaseConstructL();
				iGridContainer = new (ELeave) CPlutoBTGridContainer;
				//iGridContainer->SetMopParent(this);
				iGridContainer->ConstructL( ClientRect() );
				//AddToStackL( iGridContainer );
				break;
            }
		// Deleting a VMC file
        case EPlutoBTCmdAppTest3:
            {
            iAppContainer->CallDelete();
			iAppContainer->SetPlutoBT(EPlutoBTPictures, EPlutoBTSizeDateNoChange);
			break;
            }
		// Help Screen
        case EPlutoBTCmdAppTest4:
            {
            // Will not do anything now
            break;
            }

        default:
            break;      
        }
    }
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::CreateVMCView()
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
void CPlutoBTAppUi::MakeViewerVisible(bool Value)
{
	if(Value)
		m_pVMCView->iContainer->MakeVisible(true);

	m_bVMCViewerVisible = true;
}
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::OpenImage(unsigned char Type, unsigned long Size, const char *Data) //fullscreen (!)
{
	CreateVMCView();

	CPlutoVMCUtil *pVCMUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVCMUtil->SetImage(Type, Size, Data);

	m_pVMCView->Refresh();

	MakeViewerVisible(m_bMakeVisibleAllowed);
}
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::OpenVMC(bool bParsed, TFileName& iFileName, 
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
void CPlutoBTAppUi::ShowList(
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
void CPlutoBTAppUi::SetCaptureKeyboardCommand(
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
void CPlutoBTAppUi::UpdateScreen(VIPMenuCollection *pVMC)
{
	TFileName iDummy;

	OpenVMC(true, iDummy, pVMC);
}
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::UpdateScreen(bool bParsed, const TDes8& aVmc, unsigned int uSize, 
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
void CPlutoBTAppUi::Hide()
{
	TApaTask task(CEikonEnv::Static()->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.SendToBackground();

	m_bVMCViewerVisible = false;

	//TInt iCapturedKeyId = 1;
	//CEikonEnv::Static()->RootWin().CancelCaptureKeyUpAndDowns(iCapturedKeyId);
}
//----------------------------------------------------------------------------------------------
void CPlutoBTAppUi::Show()
{
	TApaTask task(CEikonEnv::Static()->WsSession());
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.BringToForeground();

	m_bVMCViewerVisible = true;

	CEikonEnv::Static()->RootWin().CaptureKeyUpAndDowns(EStdKeyNo, 0, 0);
}
//----------------------------------------------------------------------------------------------


// End of File  

