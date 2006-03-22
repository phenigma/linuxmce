#include "StdAfx.h"
#include "Outlook.h"
#include "Log.h"

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
		pMAPI = pApp->GetNamespace("MAPI");
		Log::m_pLog->writeLine( "After GetNamespace");
		pMAPI->Logon("","",false,false);
		Log::m_pLog->writeLine( "After Logon");
		pFolder = pMAPI->GetDefaultFolder(olFolderContacts);
		Log::m_pLog->writeLine( "After GetFolder");
		pItems = pFolder->GetItems();
		Log::m_pLog->writeLine( "After GetItems");
		pContact = pItems->GetFirst();
		Log::m_pLog->writeLine( "After GetFirst" );
		nCount = pItems->GetCount();
		Log::m_pLog->writeLine( "Number of contacts" );

		for(int i=0;i<nCount;i++)
		{

			pContactInfo = new Contact;

			if( (LPCSTR) pContact->GetFirstName() != NULL )
				pContactInfo->firstName = (LPCSTR) pContact->GetFirstName();
			if( (LPCSTR) pContact->GetMiddleName() != NULL )
				pContactInfo->middleName = (LPCSTR) pContact->GetMiddleName();
			if( (LPCSTR) pContact->GetLastName() != NULL )
				pContactInfo->lastName = (LPCSTR) pContact->GetLastName();

			if( (LPCSTR) pContact->GetCompanyName() != NULL )
				pContactInfo->companyName = (LPCSTR) pContact->GetCompanyName();
			if( (LPCSTR) !pContact->GetJobTitle() != NULL)
				pContactInfo->title = (LPCSTR) pContact->GetJobTitle();

			if( (LPCSTR) pContact->GetHomeTelephoneNumber() != NULL )
				pContactInfo->phoneHome = (LPCSTR) pContact->GetHomeTelephoneNumber();
			if( (LPCSTR) pContact->GetBusinessTelephoneNumber() != NULL )
				pContactInfo->homeBusiness = (LPCSTR) pContact->GetBusinessTelephoneNumber();
			if( (LPCSTR) pContact->GetMobileTelephoneNumber() != NULL )
				pContactInfo->phoneMobile = (LPCSTR) pContact->GetMobileTelephoneNumber();

			if( (LPCSTR) pContact->GetHomeAddress() != NULL )
				pContactInfo->homeAddress = (LPCSTR) pContact->GetHomeAddress();
			if( (LPCSTR) pContact->GetBusinessAddress() != NULL )
				pContactInfo->businessAddress = (LPCSTR) pContact->GetBusinessAddress();

			if( (LPCSTR) pContact->GetEmail1Address() != NULL )
				pContactInfo->email = (LPCSTR) pContact->GetEmail1Address();

			Log::m_pLog->m_file << *pContactInfo;

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