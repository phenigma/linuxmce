#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "VR_IdentifyPhone.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/MySQLHelper.h"
#include "VIPShared/VIPMenu.h"
#include "VR_ShowMenu.h"
#include "VA_ForwardRequestToPhone.h"

#include <iostream>
#include <sstream>

#define	PLUTOID_USERNAME	1
#define	PLUTOID_PASSWORD	2
#define	USERS_NICKNAME	3
#define	PLUTOID_EMAIL	4
#define	USERS_GENDER	5
#define	USERS_BIRTHDATE	6
#define	USERS_ALLOWCONTACTOTHERVIP	7
#define	USERS_FIRSTNAME	8
#define	USERS_LASTNAME	9
#define	PLUTOID_PIN	10
#define	USERS_FKID_PHONEMODEL	11
#define	ESTABLISHMENT_FKID_ESTABLISHMENTCATEGORY	13
#define	ESTABLISHMENT_FKID_C_STATUS	14
#define	ESTABLISHMENT_NAME	17
#define	ESTABLISHMENT_WEBSITE	19
#define	PLUTOID_PWDISTEMP	24
#define	PLUTOID_MODIFIEDDATE	25
#define	PLUTOID_LOGINDATE	26
#define	PLUTOID_SIGNUPDATE	27
#define	PKID_ADDRESS	28
#define	PKID_PHONENUMBER	29
#define	_FP_PICTURE	30

VR_IdentifyPhone::VR_IdentifyPhone(unsigned long EstablishmentID,
		string sBluetooth,u_int64_t MacAddress,unsigned long CachedRecordVersion,unsigned long CachedPlutoId,
		unsigned long IdentifiedPlutoId,string IdentiedPlutoIdPin,class Customer *pCustomer) 
	: RA_Request()
{
	// Request is of the form:
	// AAAAIIIIdescription+null+CashierName+null
	m_iEstablishmentID=EstablishmentID;
	m_sBluetoothID=sBluetooth;
	m_iMacAddress=MacAddress;
	m_iCachedRecordVersion=CachedRecordVersion;
	m_iCachedPlutoId=CachedPlutoId;
	m_iIdentifiedPlutoId=IdentifiedPlutoId;
	m_sIdentifiedPlutoIdPin=IdentiedPlutoIdPin;
	m_pCustomer=pCustomer;

	m_iUseCache=0;
	m_iRecordVersion=0;
	m_iPlutoId=0;
}

bool VR_IdentifyPhone::ProcessRequest(class RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER
	m_pdbImage.m_dwSize = 0;
	m_pdbImage.m_pBlock = NULL;

	cout << "Processing request to ID Mac " << m_iMacAddress << " ID " << m_iIdentifiedPlutoId << "\n";

	if( !m_iMacAddress )
	{
		cout << "Request from " << m_iEstablishmentID << " had empty MAC"; 
		m_cProcessOutcome=INVALID_REQUEST_DATA;
		return true;
	}

	m_cProcessOutcome=SUCCESSFULLY_PROCESSED;

	unsigned long PKID_C_PhoneStatus=0,RecordVersion=0;
	string FileName; // File to send to the phone

	PlutoSqlResult rsMacAddress,rsPlutoId,rsUsers,rsAddress,rsExtra,rsShare,rsPhone;
	MYSQL_ROW MacAddressRow=NULL;
	MYSQL_ROW PlutoIdRow=NULL;
	MYSQL_ROW UsersRow=NULL,AddressRow=NULL,ExtraDataRow=NULL,ShareRow=NULL,PhoneRow=NULL;

	std::ostringstream s;
	s << "SELECT FKID_PlutoID,FKID_C_PhoneStatus,FKID_BinaryVersion,BinaryRevision,NoBinary,Revision,BinaryFilename FROM MacAddress LEFT JOIN PhoneModel ON FKID_PhoneModel=PKID_PhoneModel LEFT JOIN BinaryVersion ON FKID_BinaryVersion=PKID_BinaryVersion WHERE PKID_MacAddress=" << m_iMacAddress;

	cout << "Executing: " << s.str() << "\n";
	if( (rsMacAddress.r = g_pPlutoConfig->mysql_query_result(s.str())) && (MacAddressRow=mysql_fetch_row(rsMacAddress.r)) )
	{
		// See if: This phone does take a binary, and NoBinary is not set, and there is revision
		if( MacAddressRow[2] && (!MacAddressRow[4] || MacAddressRow[4][0]=='0') && MacAddressRow[5] )
		{
			if( !MacAddressRow[3] || atoi(MacAddressRow[3])<atoi(MacAddressRow[5]) )
				FileName = MacAddressRow[6]; // This is going to the phone
		}
		cout << "Found Mac " << m_iMacAddress << " associated with user: " << MacAddressRow[0] << "\n";
		m_iPlutoId = atoi(MacAddressRow[0]);
		PKID_C_PhoneStatus = atoi(MacAddressRow[1]);

		cout << "Request from " << m_iEstablishmentID << " mac: " << m_iMacAddress << 
			" PlutoID: " << m_iPlutoId << " Status: " << PKID_C_PhoneStatus; 

		s.str("");
		s << "SELECT RecordVersion,Email FROM PlutoId WHERE PKID_PlutoId=" << m_iPlutoId;
		if( (rsPlutoId.r = g_pPlutoConfig->mysql_query_result(s.str())) && (PlutoIdRow=mysql_fetch_row(rsPlutoId.r)) )
		{
			m_iRecordVersion = atoi(PlutoIdRow[0]);
			cout << "Request from " << m_iEstablishmentID << 
				" PlutoID: " << m_iPlutoId << " has version " << m_iCachedRecordVersion << "/" << m_iRecordVersion; 
			if( m_iCachedRecordVersion == m_iRecordVersion )
			{
				cout << "using cached version\n";
				cout << "Request from " << m_iEstablishmentID << 
					" PlutoID: " << m_iPlutoId << " using cache"; 
			    m_iUseCache=1;
				goto CheckForVMC;
			}
		}
	}

	// The Establishment doensn't have a current cached version.  First let's see if this is the first time
	// the phone has registered.  This would mean we didn't find the MacAddress in the database, but the 
	// Establishment did pass in a PlutoId that it derived from the Bluetooth ID.
	if( !m_iPlutoId && m_iIdentifiedPlutoId )
	{
		cout << "Request from " << m_iEstablishmentID << 
			" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin; 
		s.str("");
		s << "SELECT RecordVersion,Email,FKID_PhoneModel,NoBinary,BinaryFilename FROM PlutoId "
			<< " LEFT JOIN Users on FKID_PlutoId=PKID_PlutoId "
			<< " LEFT JOIN PhoneModel ON FKID_PhoneModel=PKID_PhoneModel "
			<< " LEFT JOIN BinaryVersion ON FKID_BinaryVersion=PKID_BinaryVersion "
			<< " WHERE PKID_PlutoId=" << m_iIdentifiedPlutoId << " AND PIN='"
			<< m_sIdentifiedPlutoIdPin << "'";

		if( (rsPlutoId.r = g_pPlutoConfig->mysql_query_result(s.str())) && (PlutoIdRow=mysql_fetch_row(rsPlutoId.r)) )
		{
			cout << "Adding to database Mac " << m_iMacAddress << " ID " << m_iIdentifiedPlutoId << "\n";
			cout << "Request from " << m_iEstablishmentID << 
				" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin << " adding to database"; 
			// We found the record
			m_iPlutoId = m_iIdentifiedPlutoId;
			// Add it to the MacAddress table
//			sprintf(sql,"INSERT INTO MacAddress(PKID_MacAddress,FKID_PlutoId,FKID_C_PhoneStatus) "
//				"VALUES(%I64d,%d,%d)",m_iMacAddress,(int) m_iPlutoId,(int) C_PHONESTATUS_NEW_PHONE_CONST);


			// See if there is a model specified, and the the nobinary flag is not set, and there is a binary version
			if( (!PlutoIdRow[3] || PlutoIdRow[3][0]=='0') && PlutoIdRow[2] && PlutoIdRow[4] )
				FileName = PlutoIdRow[4];

			s.str("");
			s << "INSERT INTO MacAddress(PKID_MacAddress,FKID_PlutoId,FKID_C_PhoneStatus,NoBinary,FKID_PhoneModel) " <<
				"VALUES(" << m_iMacAddress << "," << m_iPlutoId << "," << C_PHONESTATUS_NEW_PHONE_CONST << 
				"," << (PlutoIdRow[3] ? PlutoIdRow[3] : "0") << // No Binary
				"," << (PlutoIdRow[2] ? PlutoIdRow[2] : "NULL") << // Phone Model
				")";

			cout << "Executing: " << s << "\n";
			g_pPlutoConfig->threaded_mysql_query(s.str());
		}
	}

	if( !m_iPlutoId )
	{
		cout << "No records found\n";
		cout << "Request from " << m_iEstablishmentID << 
			" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin << " not one of ours"; 
		m_cProcessOutcome=RECORD_NOT_FOUND;
		return true;
	}

	s.str("");
	s << "SELECT Nickname,Gender,BirthDate,FirstName,LastName FROM Users where FKID_PlutoID=" << m_iPlutoId;
	if( (rsUsers.r = g_pPlutoConfig->mysql_query_result(s.str())) )
		UsersRow=mysql_fetch_row(rsUsers.r);

	s.str("");
	s << "SELECT Address,ZipCode,City.Name as City,C_State.Name As State FROM Address LEFT JOIN City on FKID_City=PKID_City LEFT JOIN C_State ON FKID_C_State=PKID_C_State where FKID_PlutoID=" << m_iPlutoId;
	if( (rsAddress.r = g_pPlutoConfig->mysql_query_result(s.str())) )
		AddressRow=mysql_fetch_row(rsAddress.r);

	s.str("");
	s << "SELECT AreaCode,Number,Extension,FKID_PhoneNumberCategory FROM PhoneNumber where FKID_PlutoID=" << m_iPlutoId;
	rsPhone.r = g_pPlutoConfig->mysql_query_result(s.str());

	s.str("");
	s << "SELECT ShareText FROM Users_EstablishmentCategory INNER JOIN Establishment ON Establishment.FKID_EstablishmentCategory=Users_EstablishmentCategory.FKID_EstablishmentCategory WHERE FKID_PlutoId_User="
		<< m_iEstablishmentID << " AND FKID_PlutoID=" << m_iPlutoId;

	if( (rsExtra.r = g_pPlutoConfig->mysql_query_result(s.str())) )
	{
		while( ExtraDataRow=mysql_fetch_row(rsExtra.r) )
		{
			MYSTL_ADDTO_LIST(m_listAttributes,new MiscVIPAttribute("extra",ExtraDataRow[0]));
		}
	}

	s.str("");
	s << "SELECT FKID_C_Field FROM ShareWithEstablishment WHERE FKID_PlutoId=" << m_iPlutoId;

	if( (rsShare.r = g_pPlutoConfig->mysql_query_result(s.str())) )
	{
		while( ShareRow=mysql_fetch_row(rsShare.r) )
		{
			cout << "Sharing field " << ShareRow[0] << "\n";
			switch( atoi(ShareRow[0]) )
			{
			case	USERS_NICKNAME:
				if( UsersRow && UsersRow[0] )
					m_sNickname=UsersRow[0];
				break;
			case	PLUTOID_EMAIL:
				if( PlutoIdRow && PlutoIdRow[1] )
				m_sEmail = PlutoIdRow[1];
				break;
			case	USERS_GENDER:
				if( UsersRow && UsersRow[1] )
				m_sGender = UsersRow[1];
				break;
			case	USERS_BIRTHDATE:
				if( UsersRow && UsersRow[2] )
				m_sBirthdate = UsersRow[2];
				break;
			case	USERS_FIRSTNAME:
				if( UsersRow && UsersRow[3] )
				m_sFirstName = UsersRow[3];
				break;
			case	USERS_LASTNAME:
				if( UsersRow && UsersRow[4] )
				m_sLastName = UsersRow[4];
				break;
			case	PKID_ADDRESS:
				if( AddressRow && AddressRow[0] )
				m_sAddress = AddressRow[0];
				if( AddressRow && AddressRow[2])
				m_sCity = AddressRow[2];
				if( AddressRow && AddressRow[3] )
				m_sState = AddressRow[3];
				if( AddressRow && AddressRow[1] )
				m_sZip = AddressRow[1];
				break;
			case	PKID_PHONENUMBER:
				if( rsPhone.r )
				{
					while( PhoneRow=mysql_fetch_row(rsPhone.r) )
					{
						VIPPhoneNumber *pn = new VIPPhoneNumber();
						pn->m_iPhoneNumberType=(PhoneRow[3] ? atoi(PhoneRow[3]) : 0);
						pn->m_sNumber=string("(") + (PhoneRow[0] ? PhoneRow[0] : "") + 
							") " + (PhoneRow[1] ? PhoneRow[1] : "") + 
							(PhoneRow[2]!=NULL ? string(" ext: ") + PhoneRow[2] : "");
						MYSTL_ADDTO_LIST(m_listPhoneNumbers,pn);
					}
				}
				break;
			case	_FP_PICTURE:
				FILE *file = fopen( (g_pPlutoConfig->m_sPicturePath + StringUtils::itos(m_iPlutoId) + ".jpg").c_str(),"rb");
				cout << "Tried to open " << g_pPlutoConfig->m_sPicturePath << m_iPlutoId << ".jpg result: " << file << "\n";
				m_iImageType=GRAPHICTYPE_JPG;
				if( file )
				{
					fseek(file, 0L, SEEK_END);
					m_pdbImage.m_dwSize=ftell(file);
					if( m_pdbImage.m_dwSize==0 )
						fclose(file);
					else
					{
						fseek(file, 0L, SEEK_SET);
						m_pdbImage.m_pBlock=(char *)malloc(m_pdbImage.m_dwSize+1);
						size_t bytesread=fread(m_pdbImage.m_pBlock,1,m_pdbImage.m_dwSize,file);
						fclose(file);
					}
				}
			};
		}
	}

    m_iUseCache=0;

CheckForVMC:
	// Let's see if we're storing the Establishment's customer ID, or if there's a VMC
	// that we should automatically send to the phone
	MYSQL_ROW CustomerDataRow=NULL;
	s.str("");
	s << "SELECT Users_Establishment.EstablishmentCustomerID,Users_Establishment.AutoSendVMC,Establishment.AutoSendVMC "
		"FROM Establishment LEFT JOIN Users_Establishment ON FKID_PlutoId_Establishment=Establishment.FKID_PlutoId WHERE "
		"FKID_PlutoId_User=" << m_iPlutoId << " AND FKID_PlutoId_Establishment=" << m_iEstablishmentID;
	cout << "Looking up user: " << m_iPlutoId << " establishment: " << m_iEstablishmentID << "\n";
	PlutoSqlResult rsCustomerDataRow;
	if( (rsCustomerDataRow.r = g_pPlutoConfig->mysql_query_result(s.str())) && (CustomerDataRow=mysql_fetch_row(rsCustomerDataRow.r)) )
	{
		cout << "Found a record\n";
		const char *AutoSendVMC=NULL;
		if( CustomerDataRow[0] )
			m_sEstablishmentCustomerId = CustomerDataRow[0];
		if( CustomerDataRow[1] )
			AutoSendVMC = CustomerDataRow[1];
		else if( CustomerDataRow[2] )
			AutoSendVMC = CustomerDataRow[2];
			
		if( AutoSendVMC )
		{
			cout << "Sending menu " << AutoSendVMC;
			cout << "Sending menu " << g_pPlutoConfig->m_sMenuPath << AutoSendVMC; 

			VR_ShowMenu *pMenu = new VR_ShowMenu((g_pPlutoConfig->m_sMenuPath + AutoSendVMC).c_str());
			if( !pMenu->m_pMenuCollection )
			{
				cout << "Couldn't send menu " << g_pPlutoConfig->m_sMenuPath << AutoSendVMC; 
				delete pMenu;   // Menu wasn't found
			}
			else
			{
				VA_ForwardRequestToPhone *pbefore = new VA_ForwardRequestToPhone(0,pMenu,m_iMacAddress);
				MYSTL_ADDTO_LIST(m_listActions, pbefore);
			}
		}
	}

	LogVisit();

#endif
	return true;
}


void VR_IdentifyPhone::LogVisit()
{
#ifdef VIPSERVER
	char sql[100];
	cout << "Visit Logged\n";
	sprintf(sql,"INSERT INTO Visit(FKID_PlutoId_User,FKID_PlutoId_Establishment) VALUES(%d,%d)",m_iPlutoId,m_iEstablishmentID);
	g_pPlutoConfig->threaded_mysql_query(sql);
#endif
}

bool VR_IdentifyPhone::UnknownSerialize( ItemToSerialize *pItem, bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition )
{
	m_pcDataBlock = pcDataBlock; m_dwAllocatedSize = dwAllocatedSize; m_pcCurrentPosition = pcCurrentPosition;
	
	if( bWriting ) // writing
	{
		switch( pItem->m_iSerializeDataType )
		{
		case SERIALIZE_DATA_TYPE_LIST_PHONE_NUM:
			{
				list<VIPPhoneNumber *> *p_listPhoneNumbers = (list<VIPPhoneNumber *> *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) p_listPhoneNumbers->size() );
				for( list<VIPPhoneNumber *>::iterator it = p_listPhoneNumbers->begin(); it != p_listPhoneNumbers->end(); ++it )
				{
					VIPPhoneNumber *pVIPPhoneNumber = *it;
					Write_unsigned_long(pVIPPhoneNumber->m_iPhoneNumberType);
					Write_string(pVIPPhoneNumber->m_sNumber);
				}
				return true; // We handled it
			}
			break;
			
		case SERIALIZE_DATA_TYPE_LIST_VIP_ATTR:
			{
				list<MiscVIPAttribute *> *p_listAttributes = (list<MiscVIPAttribute *> *) pItem->m_pItem;
				Write_unsigned_long( (unsigned long) p_listAttributes->size() );
				for( list<MiscVIPAttribute *>::iterator it = p_listAttributes->begin(); it != p_listAttributes->end(); ++it )
				{
					MiscVIPAttribute *pMiscVIPAttribute = *it;
					Write_string(pMiscVIPAttribute->m_sType);
					Write_string(pMiscVIPAttribute->m_sValue);
				}
				return true; // We handled it
			}
			break;
			
		};
	}
	else // reading
	{
		switch( pItem->m_iSerializeDataType )
		{
		
		case SERIALIZE_DATA_TYPE_LIST_PHONE_NUM:
			{
				list<VIPPhoneNumber *> *p_listPhoneNumbers = (list<VIPPhoneNumber *> *) pItem->m_pItem;
				long Count=Read_unsigned_long();
				for(long c=0;c<Count;++c)
				{
					VIPPhoneNumber *pVIPPhoneNumber = new VIPPhoneNumber();
					pVIPPhoneNumber->m_iPhoneNumberType=Read_unsigned_long();
					Read_string(pVIPPhoneNumber->m_sNumber);
					p_listPhoneNumbers->push_back(pVIPPhoneNumber);
				}
				return true; // We handled it
			}
			break;
			
		case SERIALIZE_DATA_TYPE_LIST_VIP_ATTR:
			{
				list<MiscVIPAttribute *> *p_listAttributes = (list<MiscVIPAttribute *> *) pItem->m_pItem;
				long Count=Read_unsigned_long();
				for(long c=0;c<Count;++c)
				{
					string sType,sValue;
					Read_string(sType);
					Read_string(sValue);
					MiscVIPAttribute *pMiscVIPAttribute = new MiscVIPAttribute(sType,sValue);
					p_listAttributes->push_back(pMiscVIPAttribute);
				}
				return true; // We handled it
			}
			break;
			
		};
	}

	pcDataBlock = m_pcDataBlock; dwAllocatedSize = m_dwAllocatedSize; pcCurrentPosition = m_pcCurrentPosition;
	return true;
}

