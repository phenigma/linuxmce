#ifndef PLUTOMOCONTAINER_H
#define PLUTOMOCONTAINER_H

#include <coecntrl.h>
#include <aknlists.h>
#include "eiklbx.h"         // For Listbox
#include "PlutoMOEngine.h"




class CPlutoMOContainer : public CCoeControl, MCoeControlObserver
    {
    public: 
        void ConstructL(const TRect& aRect);

        ~CPlutoMOContainer();

    public: 
        void SetPlutoMO(TInt aDirectory, TInt aSizeDate);
		
		// Handles open from option menu
		void CallLaunch();		

		// Handles file delete
		void CallDelete();

		// Find number of files
		void FindNumberFiles();
    
		// Make grid
		void MakeGridL();

	public: 

    TKeyResponse    OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);



    private: // Functions from base classes
        void SizeChanged();

        TInt CountComponentControls() const;

        CCoeControl* ComponentControl(TInt aIndex) const;

        void Draw(const TRect& aRect) const;

        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    public:
        CPlutoMOEngine* iAppEngine;
	
    private: //data
        // Listbox
        CAknDoubleNumberStyleListBox* iListBox;
		//CPlutoMOGridEngine* iGamesGrid; // the grid

    };

#endif

// End of File
