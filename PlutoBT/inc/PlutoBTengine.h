#ifndef PLUTOBTENGINE_H
#define PLUTOBTENGINE_H

#include <coecntrl.h>
#include <coemain.h>
#include <coeccntx.h>

#include <f32file.h>        // for CDir
#include <documenthandler.h>// for launching
#include <badesca.h>        // for CDesCArray
#include <apmstd.h>         // for TDataType

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikon.hrh>

#include <aknnotewrappers.h>

//------------------------------------------------------------------------------------------------------------------
const TInt KLengthOfStringToShowIndex = 40;
//------------------------------------------------------------------------------------------------------------------
class CPlutoBTEngine : public CCoeControl
{
public: 

    void ConstructL();

    ~CPlutoBTEngine();

public: 

    TInt StartPlutoBT();

    void GetPlutoBTItems(CDesCArray* iItems);

    void SetDirectory(TInt aDirectory);
    void SetSizeDate(TInt aSizeDate);

    void EndPlutoBT();

    TBool RemoveItems(CDesCArray* aItems);

    void LaunchCurrent(TInt aPosition);

	//Remove file
	void RemoveFile(TInt aPosition);

	// Finf Files
	void FindFiles();

private: //Data
    CDir* iDirList;

    // Fileserver session
    RFs iFsSession;

	//Information Note
	CAknInformationNote* informationNote;

private: // variables
    TInt iDirectory;    
    TInt iSizeDate;     
	TInt iCount;

};
//------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------
// End of File
