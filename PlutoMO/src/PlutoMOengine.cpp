#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 

#include "bitmapmethods.h"
#include "palbimageviewerbasic.h"

#include "MyString.h"
#include "PlutoBTEngine.h"
#include "PlutoBT.hrh"     // enumerations

#include "PlutoBTAppUi.h"

//------------------------------------------------------------------------------------------------------------------
// Number, name and file size
_LIT(KStringSize,"%d\t%S\t%d bytes");
// Number, name and date modified
_LIT(KStringDate,"%d\t%S\t%S"); 
// Directory for Sounds
_LIT(KDirSounds,"c:\\Nokia\\Sounds\\Digital\\");
// Directory for Pictures
_LIT(KDirPictures,"c:\\Nokia\\Images\\");
// Directory for Videos
_LIT(KDirVideos,"c:\\Nokia\\Videos\\");
//------------------------------------------------------------------------------------------------------------------
void CPlutoBTEngine::ConstructL()
    {
    }
//------------------------------------------------------------------------------------------------------------------
CPlutoBTEngine::~CPlutoBTEngine()
    {
    delete iDirList;
    }
//------------------------------------------------------------------------------------------------------------------
TInt CPlutoBTEngine::StartPlutoBT()
    {
    if (iDirList)
        {        
        delete iDirList;
        iDirList = 0;
        }    
        
    TInt error = KErrNone;

    User::LeaveIfError(iFsSession.Connect()); 

    switch (iDirectory)
        {
        case EPlutoBTSounds:
            error = iFsSession.GetDir(KDirSounds,KEntryAttNormal,ESortByName,iDirList);
            break;
        case EPlutoBTPictures:
            error = iFsSession.GetDir(KDirPictures,KEntryAttNormal,ESortByName,iDirList);
            break;
        case EPlutoBTVideos:
            error = iFsSession.GetDir(KDirVideos,KEntryAttNormal,ESortByName,iDirList);
            break;
        default:
            error = iFsSession.GetDir(KDirSounds,KEntryAttNormal,ESortByName,iDirList);
            break;
        }

    return error;

    }
//------------------------------------------------------------------------------------------------------------------
void CPlutoBTEngine::GetPlutoBTItems(CDesCArray* aItems)
    {
    if(!iDirList)
        return;
                       
    for (TInt i=0;i<iDirList->Count();i++)
        {
        TFileName filename = NULL;
        if(iSizeDate==EPlutoBTSize)
            {
            filename.Format(KStringSize,i+1,&(*iDirList)[i].iName,(*iDirList)[i].iSize);
            }
        else
            {
            TBuf<30> dateString; 
            _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
            (*iDirList)[i].iModified.FormatL(dateString,KDateString); 
            filename.Format(KStringDate,i+1,&(*iDirList)[i].iName,&dateString);
            }
        aItems->AppendL(filename);
        }
        
    }
//------------------------------------------------------------------------------------------------------------------
void CPlutoBTEngine::SetDirectory(TInt aDirectory)
    {
    if (aDirectory!=EPlutoBTDirNoChange)
        iDirectory=aDirectory;
    }
//------------------------------------------------------------------------------------------------------------------
void CPlutoBTEngine::LaunchCurrent(TInt aPosition)
    {

    if(!iDirList)
        return;

 
    CDocumentHandler* handler = CDocumentHandler::NewL(NULL);
    CleanupStack::PushL(handler);
    TFileName descr;

    switch (iDirectory)
        {   
        case EPlutoBTSounds:
            descr.Append(KDirSounds);
            break;
        case EPlutoBTPictures:
            descr.Append(KDirPictures);
            break;
        case EPlutoBTVideos:
            descr.Append(KDirVideos);
            break;
        default:
            descr.Append(KDirSounds);
            break;
        }

    descr.Append((*iDirList)[aPosition].iName);
    TDataType nullType;

	TParse p;
	p.Set(descr, NULL, NULL);

	if(p.Ext() != ".vmc" && p.Ext() != ".VMC")
		handler->OpenFileL(descr, nullType);
	else
		//g_pAppUi->OpenVMC(descr);
		((CPlutoBTAppUi *)CCoeEnv::Static()->AppUi())->OpenVMC(false, descr, NULL);


    CleanupStack::PopAndDestroy(); // handler
    };
//------------------------------------------------------------------------------------------------------------------
// Remove Files
void CPlutoBTEngine::RemoveFile(TInt aPosition)
    {

    if(!iDirList)
        return;
     //Remove current item in the list

    TFileName descr;
    
	
	switch (iDirectory)
        {   
        case EPlutoBTSounds:
            descr.Append(KDirSounds);
            break;
        case EPlutoBTPictures:
            descr.Append(KDirPictures);
            break;
        case EPlutoBTVideos:
            descr.Append(KDirVideos);
            break;
        default:
            descr.Append(KDirPictures);
            break;
        }
    // Add filename to be deleted
    descr.Append((*iDirList)[aPosition].iName);
	
	
	User::LeaveIfError(iFsSession.Connect()); 

	User::LeaveIfError(iFsSession.Delete(descr));
    
	iFsSession.Close();
    

	// Display an information note
	informationNote = new (ELeave) CAknInformationNote;
    
	informationNote->ExecuteLD(_L("File Deleted"));
};
//------------------------------------------------------------------------------------------------------------------
// Find number of files in current folder
void CPlutoBTEngine::FindFiles()
{
	if(!iDirList)
        return;
	
	_LIT(KString, "Number of files: %d");

	TBuf< KLengthOfStringToShowIndex > number;
	TBuf< KLengthOfStringToShowIndex > format;
	
	format.Append(KString);	

	iCount = iDirList->Count();

	number.Format( format, iCount );

	informationNote = new (ELeave) CAknInformationNote;
    
	//informationNote->ExecuteLD(_L("TEST"));	
	
	informationNote->ExecuteLD(number);	

};
//------------------------------------------------------------------------------------------------------------------
TBool CPlutoBTEngine::RemoveItems(CDesCArray* aItems)
    {
    if(iDirList)
        {        
        if (iDirList->Count())
            {
            aItems->Delete(0,(iDirList->Count()));
            return ETrue;
            }
        }    
    return EFalse;
    };
//------------------------------------------------------------------------------------------------------------------
void CPlutoBTEngine::SetSizeDate(TInt aSizeDate)
    {
    if (aSizeDate==EPlutoBTToggle)
        if (iSizeDate==EPlutoBTSize)
            iSizeDate=EPlutoBTDate;
        else
            iSizeDate=EPlutoBTSize;
    else
        if (aSizeDate!=EPlutoBTSizeDateNoChange)
            iSizeDate=aSizeDate;
    };
//------------------------------------------------------------------------------------------------------------------
void CPlutoBTEngine::EndPlutoBT()
    {
    iFsSession.Close();
    };
//------------------------------------------------------------------------------------------------------------------
