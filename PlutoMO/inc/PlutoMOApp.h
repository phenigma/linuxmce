#ifndef PLUTOMOAPP_H
#define PLUTOMOAPP_H

#include <aknapp.h>
#include <documenthandler.h>

const TUid KUidPlutoMO = { 0x01ef0015 };
class CPlutoMOApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        CApaDocument* CreateDocumentL();
        
        TUid AppDllUid() const;
        CDocumentHandler* iDocHandler;
    };

#endif

// End of File

