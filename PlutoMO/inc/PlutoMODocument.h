#ifndef PLUTOMODOCUMENT_H
#define PLUTOMODOCUMENT_H

#include <akndoc.h>

   
class  CEikAppUi;

class CPlutoMODocument : public CAknDocument
    {
    public: 
        static CPlutoMODocument* NewL(CEikApplication& aApp);

        virtual ~CPlutoMODocument();

    private:

        CPlutoMODocument(CEikApplication& aApp);
        void ConstructL();

    private: 
        CEikAppUi* CreateAppUiL();

    };

#endif

// End of File

