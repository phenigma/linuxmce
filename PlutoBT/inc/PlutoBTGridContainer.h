#ifndef PLUTOBTGRIDCONTAINER_H
#define PLUTOBTGRIDCONTAINER_H

#include <coecntrl.h> // CCoeControl
#include <eiklbo.h> // MEikListBoxObserver

class CPlutoBTGridEngine;

class CPlutoBTGridContainer : public CCoeControl, MEikListBoxObserver
	{
public: 

	static CPlutoBTGridContainer* NewL(const TRect& aRect);
	static CPlutoBTGridContainer* NewLC(const TRect& aRect);
	~CPlutoBTGridContainer();

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

	CPlutoBTGridEngine* iGamesGrid; // the grid
	};

#endif	

// End of File
