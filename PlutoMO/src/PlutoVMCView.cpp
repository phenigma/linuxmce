// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 

#include <aknviewappui.h>
#include <avkon.hrh>

#include "PlutoMO.hrh"
#include "PlutoVMCView.h"
#include "PlutoVMCUtil.h"
#include "PlutoVMCContainer.h" 
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPlutoVMCView::CPlutoVMCView()
{
	bReadyToRedraw = false;

	//the one and only ...
	CPlutoVMCUtil *pVCMUtil = new CPlutoVMCUtil(KCPlutoVMCUtilId, CCoeStatic::EApp);
}

// EPOC default constructor can leave.
void CPlutoVMCView::ConstructL()
    {
    BaseConstructL();
    }

// Destructor
CPlutoVMCView::~CPlutoVMCView()
{
	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	delete pVMCUtil;
	pVMCUtil = NULL;

	
    if (iContainer)
	{
		AppUi()->RemoveFromStack(iContainer);
	    delete iContainer;
		iContainer = NULL;
    }
	
}

void CPlutoVMCView::Refresh() 
{ 
	if(bReadyToRedraw)
		iContainer->DrawDeferred(); 
}

// ---------------------------------------------------------
// TUid CAknExEditorView1::Id()
// Returns Id of the view.
// ---------------------------------------------------------
TUid CPlutoVMCView::Id() const
    {
    return KPlutoVMCViewId;
    }

// ---------------------------------------------------------
// CPlutoVMCView::HandleCommandL(TInt aCommand)
// Handles commands
// ---------------------------------------------------------
void CPlutoVMCView::HandleCommandL(TInt aCommand)
    {   
    switch (aCommand)
        {
		
        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// ---------------------------------------------------------
// CPlutoVMCView::HandleClientRectChange()
// Handles client rect change.
// ---------------------------------------------------------
void CPlutoVMCView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

// ---------------------------------------------------------
// CPlutoVMCView::DoActivateL(...)
// Creates the Container class object.
// ---------------------------------------------------------
void CPlutoVMCView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/)
{
    iContainer = new (ELeave) CPlutoVMCContainer;
    iContainer->SetMopParent(this);
    iContainer->ConstructL(ClientRect());
    AppUi()->AddToStackL(*this, iContainer);

	TRect xrect = ClientRect();
	TRect rect(0, 0, 176, 300);
    iContainer->SetRect(rect);

	bReadyToRedraw = true;

	CPlutoVMCUtil *pVMCUtil = (CPlutoVMCUtil *)CCoeEnv::Static(KCPlutoVMCUtilId);
	pVMCUtil->Setup(iContainer, this);
}


// ---------------------------------------------------------
// CPlutoVMCView::DoDeactivate()
// Deletes the Container class object.
// ---------------------------------------------------------
void CPlutoVMCView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        }
    
    delete iContainer;
    iContainer = NULL;
    }

// End of File


