#include    "PlutoBTApp.h"
#include    "PlutoBTDocument.h"

TUid CPlutoBTApp::AppDllUid() const
    {
    return KUidPlutoBT;
    }

CApaDocument* CPlutoBTApp::CreateDocumentL()
    {
    return CPlutoBTDocument::NewL( *this );
    }

EXPORT_C CApaApplication* NewApplication()
    {
    return new CPlutoBTApp;
    }
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }

// End of File  

