#include "PlutoMODocument.h"
#include "PlutoMOAppUi.h"

CPlutoMODocument::CPlutoMODocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

CPlutoMODocument::~CPlutoMODocument()
    {
    }

void CPlutoMODocument::ConstructL()
    {
    }

CPlutoMODocument* CPlutoMODocument::NewL(
        CEikApplication& aApp)    
    {
    CPlutoMODocument* self = new (ELeave) CPlutoMODocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


    
CEikAppUi* CPlutoMODocument::CreateAppUiL()
    {
		return new (ELeave) CPlutoMOAppUi;
    }

// End of File  
