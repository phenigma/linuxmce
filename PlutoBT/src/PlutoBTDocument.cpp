#include "PlutoBTDocument.h"
#include "PlutoBTAppUi.h"

CPlutoBTDocument::CPlutoBTDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

CPlutoBTDocument::~CPlutoBTDocument()
    {
    }

void CPlutoBTDocument::ConstructL()
    {
    }

CPlutoBTDocument* CPlutoBTDocument::NewL(
        CEikApplication& aApp)    
    {
    CPlutoBTDocument* self = new (ELeave) CPlutoBTDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


    
CEikAppUi* CPlutoBTDocument::CreateAppUiL()
    {
		return new (ELeave) CPlutoBTAppUi;
    }

// End of File  
