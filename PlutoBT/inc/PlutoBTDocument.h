#ifndef PLUTOBTDOCUMENT_H
#define PLUTOBTDOCUMENT_H

#include <akndoc.h>

   
class  CEikAppUi;

class CPlutoBTDocument : public CAknDocument
    {
    public: 
        static CPlutoBTDocument* NewL(CEikApplication& aApp);

        virtual ~CPlutoBTDocument();

    private:

        CPlutoBTDocument(CEikApplication& aApp);
        void ConstructL();

    private: 
        CEikAppUi* CreateAppUiL();

    };

#endif

// End of File

