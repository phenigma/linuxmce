/* Copyright (c) 2003, Nokia. All rights reserved */

#include <aknviewappui.h>
#include <aknconsts.h>
//#include <MultiViews.rsg>

#include "PlutoMoAppUi.h"
#include "PlutoEventView.h"
#include "PlutoEventContainer.h"
//#include "MultiViews.hrh"

CPlutoEventView::CPlutoEventView()
    {
    // No implementation required
    }

CPlutoEventView::~CPlutoEventView()
    {
    // No implementation required
    }

void CPlutoEventView::ConstructL()
    {
    BaseConstructL();
    }


TUid CPlutoEventView::Id() const
    {
    return KPlutoEventViewId;
    }

void CPlutoEventView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/)
    {
    ASSERT(!(iContainer));
    iContainer = CPlutoEventContainer::NewL(ClientRect());

	TRect xrect = ClientRect();
	TRect rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iContainer->SetRect(rect);
    }

void CPlutoEventView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    }

void CPlutoEventView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
		
        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }
