#include    "PlutoMOApp.h"
#include    "PlutoMODocument.h"

TUid CPlutoMOApp::AppDllUid() const
    {
    return KUidPlutoMO;
    }

CApaDocument* CPlutoMOApp::CreateDocumentL()
    {
    return CPlutoMODocument::NewL( *this );
    }

EXPORT_C CApaApplication* NewApplication()
    {
    return new CPlutoMOApp;
    }
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }

// End of File  

