#include "PlutoBTAppUi.h"
#include "PlutoBTContainer.h"
#include "PlutoBTEngine.h"
#include "PlutoBT.hrh"

// For Grid

#include <PlutoBT.rsg> // R_PlutoBT_GAMES_GRID, R_ICON_FILE_NAME


#include "PlutoBTGridEngine.h"//CPlutoBTGridEngine

void CPlutoBTContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
  
    iListBox = new (ELeave) CAknDoubleNumberStyleListBox;

	iListBox->SetContainerWindowL( *this );
	iListBox->ConstructL( this, EAknListBoxMarkableList);


	// Create the scroll indicator
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->ActivateL();

    // Create the PlutoBTEngine
    iAppEngine = new (ELeave) CPlutoBTEngine;  

    SetPlutoBT(EPlutoBTPictures, EPlutoBTDate);
	
	SetRect(aRect);
    ActivateL();
    }

CPlutoBTContainer::~CPlutoBTContainer()
    {
    delete iAppEngine;
    delete iListBox;
    }

void CPlutoBTContainer::SetPlutoBT(TInt aDirectory, TInt aSizeDate)
    {

    CDesCArray* items = static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray());

    if (iAppEngine->RemoveItems(items)) 
        {
        iListBox->HandleItemRemovalL();  
        }

    iAppEngine->SetDirectory(aDirectory);  
    iAppEngine->SetSizeDate(aSizeDate);     
    if(iAppEngine->StartPlutoBT() == KErrNone)
        {        
        iAppEngine->GetPlutoBTItems(items);       
        }
    iAppEngine->EndPlutoBT();                  

	iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex(0);
    iListBox->DrawNow();
    }

void CPlutoBTContainer::SizeChanged()
    {
    iListBox->SetExtent(TPoint(0,0),TSize(176,160));
    }


TKeyResponse CPlutoBTContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
	if(((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->m_bVMCViewerVisible)
		return EKeyWasNotConsumed;

    TInt code = aKeyEvent.iCode;
    switch(code)
        {
    	case EKeyOK:
            iAppEngine->LaunchCurrent(iListBox->CurrentItemIndex());
            return (EKeyWasConsumed);
            break;

        default:
            return iListBox->OfferKeyEventL(aKeyEvent, aType);
            break;
        }
    }

TInt CPlutoBTContainer::CountComponentControls() const
    {
    return 1; 
    }

CCoeControl* CPlutoBTContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

void CPlutoBTContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }

void CPlutoBTContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
		int a = 0;
    }

void CPlutoBTContainer::CallLaunch() 
{
	iAppEngine->LaunchCurrent(iListBox->CurrentItemIndex());	
}

void CPlutoBTContainer::CallDelete() 
{
	iAppEngine->RemoveFile(iListBox->CurrentItemIndex());	
}

void CPlutoBTContainer::FindNumberFiles() 
{
	iAppEngine->FindFiles();	
}

void CPlutoBTContainer::MakeGridL()
{
/*
	iGamesGrid = new (ELeave)CPlutoBTGridEngine;

  CreateWindowL();

	
	SetRect(aRect);
	ActivateL();
*/
}

// End of File  
