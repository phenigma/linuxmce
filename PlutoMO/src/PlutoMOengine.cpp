/*
 PlutoMOengine
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 

//#include "bitmapmethods.h"
#include "palbimageviewerbasic.h"

#include "PlutoUtils/MyString.h"
#include "PlutoMOEngine.h"
#include "PlutoMO.hrh"     // enumerations

#include "PlutoMOAppUi.h"

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
void CPlutoMOEngine::ConstructL()
    {
    }
//------------------------------------------------------------------------------------------------------------------
CPlutoMOEngine::~CPlutoMOEngine()
    {
    delete iDirList;
    }
//------------------------------------------------------------------------------------------------------------------
TInt CPlutoMOEngine::StartPlutoMO()
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
        case EPlutoMOSounds:
            error = iFsSession.GetDir(KDirSounds,KEntryAttNormal,ESortByName,iDirList);
            break;
        case EPlutoMOPictures:
            error = iFsSession.GetDir(KDirPictures,KEntryAttNormal,ESortByName,iDirList);
            break;
        case EPlutoMOVideos:
            error = iFsSession.GetDir(KDirVideos,KEntryAttNormal,ESortByName,iDirList);
            break;
        default:
            error = iFsSession.GetDir(KDirSounds,KEntryAttNormal,ESortByName,iDirList);
            break;
        }

    return error;

    }
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::GetPlutoMOItems(CDesCArray* aItems)
    {
    if(!iDirList)
        return;
                       
    for (TInt i=0;i<iDirList->Count();i++)
        {
        TFileName filename = NULL;
        if(iSizeDate==EPlutoMOSize)
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
void CPlutoMOEngine::SetDirectory(TInt aDirectory)
    {
    if (aDirectory!=EPlutoMODirNoChange)
        iDirectory=aDirectory;
    }
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::LaunchCurrent(TInt aPosition)
    {

    if(!iDirList)
        return;
 
    CDocumentHandler* handler = CDocumentHandler::NewL(NULL);
    CleanupStack::PushL(handler);
    TFileName descr;

    switch (iDirectory)
        {   
        case EPlutoMOSounds:
            descr.Append(KDirSounds);
            break;
        case EPlutoMOPictures:
            descr.Append(KDirPictures);
            break;
        case EPlutoMOVideos:
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
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->OpenVMC(false, descr, NULL);


    CleanupStack::PopAndDestroy(); // handler
    };
//------------------------------------------------------------------------------------------------------------------
// Remove Files
void CPlutoMOEngine::RemoveFile(TInt aPosition)
    {

    if(!iDirList)
        return;
     //Remove current item in the list

    TFileName descr;
    
	
	switch (iDirectory)
        {   
        case EPlutoMOSounds:
            descr.Append(KDirSounds);
            break;
        case EPlutoMOPictures:
            descr.Append(KDirPictures);
            break;
        case EPlutoMOVideos:
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
void CPlutoMOEngine::FindFiles()
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
    
	informationNote->ExecuteLD(number);	
};
//------------------------------------------------------------------------------------------------------------------
TBool CPlutoMOEngine::RemoveItems(CDesCArray* aItems)
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
void CPlutoMOEngine::SetSizeDate(TInt aSizeDate)
    {
    if (aSizeDate==EPlutoMOToggle)
        if (iSizeDate==EPlutoMOSize)
            iSizeDate=EPlutoMODate;
        else
            iSizeDate=EPlutoMOSize;
    else
        if (aSizeDate!=EPlutoMOSizeDateNoChange)
            iSizeDate=aSizeDate;
    };
//------------------------------------------------------------------------------------------------------------------
void CPlutoMOEngine::EndPlutoMO()
    {
    iFsSession.Close();
    };
//------------------------------------------------------------------------------------------------------------------
