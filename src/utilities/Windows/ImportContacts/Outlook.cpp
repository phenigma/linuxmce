#include "StdAfx.h"
#include "Outlook.h"

// Define this according to the Outlook Object
// Model version you are compiling under
#define OUTL11// Outlook 2003
//#define OUTL10// Outlook 2002
//#define OUTL9// Outlook 2000

#pragma warning(disable:4146)

#if defined(OUTL11) // Outlook 2003
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE11\mso.dll" \
no_namespace
#import "C:\Program Files\Microsoft Office\OFFICE11\msoutl.olb" \
rename_namespace("Outlook")
#elif defined(OUTL10) // Outlook 2002
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE10\mso.dll" \
no_namespace, rename("DocumentProperties", "DocProps")
#import "C:\Program Files\Microsoft Office\OFFICE10\msoutl.olb" \
rename_namespace("Outlook")
#elif defined(OUTL9) // Outlook 2000
#import "C:\Program Files\Microsoft Office\Office\mso9.dll" \
no_namespace, rename("DocumentProperties", "DocProps")
#import "C:\Program Files\Microsoft Office\Office\msoutl9.olb" \
rename_namespace("Outlook")
#endif // OUTL11

#pragma warning(default:4146)

using namespace Outlook;

int OutlookWraper::readContacts(ContactsList &list)
{
	_ApplicationPtr pApp("Outlook.Application");
	_NameSpacePtr pMAPI;
	Contact *pContactInfo = NULL;

	MAPIFolderPtr pFolder;
	_ItemsPtr pItems;
	_ContactItemPtr pContact;;
	int nRead = 0, nCount = 0;

	try
	{
		pMAPI= pApp->GetNamespace("MAPI");
		pMAPI->Logon("","",false,false);
		pFolder = pMAPI->GetDefaultFolder(olFolderContacts);
		pItems = pFolder->GetItems();
		pContact = pItems->GetFirst();
		nCount = pItems->GetCount();

		for(int i=0;i<nCount;i++)
		{

			pContactInfo = new Contact;

			if( (char *) pContact->GetFirstName() != NULL )
				pContactInfo->firstName = (char *) pContact->GetFirstName();
			if( (char *) pContact->GetMiddleName() != NULL )
				pContactInfo->middleName = (char *) pContact->GetMiddleName();
			if( (char *) pContact->GetLastName() != NULL )
				pContactInfo->lastName = (char *) pContact->GetLastName();

			if( (char *) pContact->GetCompanyName() != NULL )
				pContactInfo->companyName = (char *) pContact->GetCompanyName();
			if( (char *) !pContact->GetJobTitle() != NULL)
				pContactInfo->title = (char *) pContact->GetJobTitle();

			if( (char *) pContact->GetHomeTelephoneNumber() != NULL )
				pContactInfo->phoneHome = (char *) pContact->GetHomeTelephoneNumber();
			if( (char *) pContact->GetBusinessTelephoneNumber() != NULL )
				pContactInfo->homeBusiness = (char *) pContact->GetBusinessTelephoneNumber();
			if( (char*) pContact->GetMobileTelephoneNumber() != NULL )
				pContactInfo->phoneMobile = (char *) pContact->GetMobileTelephoneNumber();

			if( (char *) pContact->GetHomeAddress() != NULL )
				pContactInfo->homeAddress = (char *) pContact->GetHomeAddress();
			if( (char *) pContact->GetBusinessAddress() != NULL )
				pContactInfo->businessAddress = (char *) pContact->GetBusinessAddress();

			if( (char *) pContact->GetEmail1Address() != NULL )
				pContactInfo->email = (char *) pContact->GetEmail1Address();


			list.addContact( pContactInfo );
			nRead++;
			pContact = pItems->GetNext();
		}
	}
	catch(_com_error& ce)
    {
		//errDesc = (char *) ce.Description();
		return nRead;
	}
	return nRead;
}