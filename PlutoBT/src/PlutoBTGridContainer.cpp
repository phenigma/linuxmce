#include "PlutoBTGridContainer.h"

#include <PlutoBT.rsg> // R_SIMPLEGRID_GAMES_GRID, R_ICON_FILE_NAME

#include "PlutoBTGridEngine.h"

#define KGridPosition TPoint(0,0)

void CPlutoBTGridContainer::ConstructL(const TRect& aRect)
	{
	CreateWindowL();

	iGamesGrid = new (ELeave)CPlutoBTGridEngine;
	iGamesGrid->SetContainerWindowL(*this);
	iGamesGrid->ConstructL(R_PLUTOBT_GAMES_GRID, R_ICON_FILE_NAME);
	
	iGamesGrid->SetListBoxObserver(this);
	
	SetRect(aRect);
	ActivateL();
	}

CPlutoBTGridContainer* CPlutoBTGridContainer::NewL(const TRect& aRect)
	{
	CPlutoBTGridContainer* self = CPlutoBTGridContainer::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}
CPlutoBTGridContainer* CPlutoBTGridContainer::NewLC(const TRect& aRect)
	{
	CPlutoBTGridContainer* self = new (ELeave) CPlutoBTGridContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}
CPlutoBTGridContainer::~CPlutoBTGridContainer()
	{
	delete iGamesGrid;
	}

void CPlutoBTGridContainer::SizeChanged()
	{
	if (iGamesGrid)
		{
		iGamesGrid->SetExtent (KGridPosition,Rect().Size());
		}
	}

TInt CPlutoBTGridContainer::CountComponentControls() const
	{
	return 1; 
	}

CCoeControl* CPlutoBTGridContainer::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
		case 0:
			return iGamesGrid;
		default:
			return NULL;
		}
	}

void CPlutoBTGridContainer::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
	}

TKeyResponse CPlutoBTGridContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{

	if (aType != EEventKey)
		return EKeyWasNotConsumed;

	if (iGamesGrid)
		return iGamesGrid->OfferKeyEventL (aKeyEvent, aType);
	else
		return EKeyWasNotConsumed;
	}

void CPlutoBTGridContainer::PlaySelectedGame()
	{
		iEikonEnv->InfoMsg(_L("An Icon was clicked"));
	}

void CPlutoBTGridContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aListBoxEvent)
	{
	if ((aListBoxEvent == MEikListBoxObserver::EEventEnterKeyPressed) ||
		(aListBoxEvent == MEikListBoxObserver::EEventItemClicked))
		{
		PlaySelectedGame();
		}
	}


// End of File	
