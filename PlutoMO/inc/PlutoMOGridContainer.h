#ifndef PLUTOMOGRIDCONTAINER_H
#define PLUTOMOGRIDCONTAINER_H

#include <coecntrl.h> // CCoeControl
#include <eiklbo.h> // MEikListBoxObserver

class CPlutoMOGridEngine;

class CPlutoMOGridContainer : public CCoeControl, MEikListBoxObserver
	{
public: 

	static CPlutoMOGridContainer* NewL(const TRect& aRect);
	static CPlutoMOGridContainer* NewLC(const TRect& aRect);
	~CPlutoMOGridContainer();

public: 
	
	void PlaySelectedGame();

public: 

	void ConstructL(const TRect& aRect);
	
private: 

	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aListBoxEvent);

private: 

	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void Draw(const TRect& aRect) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

private: 

	CPlutoMOGridEngine* iGamesGrid; // the grid
	};

#endif	

// End of File
