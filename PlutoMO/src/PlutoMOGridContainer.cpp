#include "PlutoMOGridContainer.h"

#include <PlutoMO.rsg> // R_SIMPLEGRID_GAMES_GRID, R_ICON_FILE_NAME

#include "PlutoMOGridEngine.h"

#define KGridPosition TPoint(0,0)

void CPlutoMOGridContainer::ConstructL(const TRect& aRect)
	{
	CreateWindowL();

	iGamesGrid = new (ELeave)CPlutoMOGridEngine;
	iGamesGrid->SetContainerWindowL(*this);
	iGamesGrid->ConstructL(R_PLUTOMO_GAMES_GRID, R_ICON_FILE_NAME);
	
	iGamesGrid->SetListBoxObserver(this);
	
	SetRect(aRect);
	ActivateL();
	}

CPlutoMOGridContainer* CPlutoMOGridContainer::NewL(const TRect& aRect)
	{
	CPlutoMOGridContainer* self = CPlutoMOGridContainer::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}
CPlutoMOGridContainer* CPlutoMOGridContainer::NewLC(const TRect& aRect)
	{
	CPlutoMOGridContainer* self = new (ELeave) CPlutoMOGridContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}
CPlutoMOGridContainer::~CPlutoMOGridContainer()
	{
	delete iGamesGrid;
	}

void CPlutoMOGridContainer::SizeChanged()
	{
	if (iGamesGrid)
		{
		iGamesGrid->SetExtent (KGridPosition,Rect().Size());
		}
	}

TInt CPlutoMOGridContainer::CountComponentControls() const
	{
	return 1; 
	}

CCoeControl* CPlutoMOGridContainer::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
		case 0:
			return iGamesGrid;
		default:
			return NULL;
		}
	}

void CPlutoMOGridContainer::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
	}

TKeyResponse CPlutoMOGridContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{

	if (aType != EEventKey)
		return EKeyWasNotConsumed;

	if (iGamesGrid)
		return iGamesGrid->OfferKeyEventL (aKeyEvent, aType);
	else
		return EKeyWasNotConsumed;
	}

void CPlutoMOGridContainer::PlaySelectedGame()
	{
		iEikonEnv->InfoMsg(_L("An Icon was clicked"));
	}

void CPlutoMOGridContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aListBoxEvent)
	{
	if ((aListBoxEvent == MEikListBoxObserver::EEventEnterKeyPressed) ||
		(aListBoxEvent == MEikListBoxObserver::EEventItemClicked))
		{
		PlaySelectedGame();
		}
	}


// End of File	
