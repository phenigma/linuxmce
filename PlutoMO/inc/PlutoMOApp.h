#ifndef PLUTOBTAPP_H
#define PLUTOBTAPP_H

#include <aknapp.h>
#include <documenthandler.h>

const TUid KUidPlutoBT = { 0x01ef0015 };
class CPlutoBTApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        CApaDocument* CreateDocumentL();
        
        TUid AppDllUid() const;
        CDocumentHandler* iDocHandler;
    };

#endif

// End of File

