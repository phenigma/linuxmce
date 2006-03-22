#include "StdAfx.h"
#include ".\plutoDatabase.h"

PlutoDatabase::PlutoDatabase(void)
{
	
	bool bRes;
	m_connStr = "Driver={MySQL ODBC 3.51 Driver};SERVER=10.0.0.150;PORT=3306;Data Source Name=PlutoTelecom;Database=pluto_telecom;Uid=root;"; 
	
	m_pContactsTable = new DatabaseWrapper;
	m_pPhoneTable = new DatabaseWrapper;

	bRes = m_pContactsTable->connect( m_connStr );
	bRes = m_pPhoneTable->connect( m_connStr );

	// Contact table data
	m_ContactParam.push_back( "Name" );
	m_ContactParam.push_back( "Company" );
	m_ContactParam.push_back( "JobDescription" );
	m_ContactParam.push_back( "Title" );
	m_ContactParam.push_back( "Email" );
	
	m_ContactType.push_back( true );
	m_ContactType.push_back( true );
	m_ContactType.push_back( true );
	m_ContactType.push_back( true );
	m_ContactType.push_back( true );


	// Contact table data
	m_PhoneParam.push_back( "FK_Contact" );
	m_PhoneParam.push_back( "FK_PhoneType" );
	//m_PhoneParam.push_back( "CountryCode" );
	m_PhoneParam.push_back( "PhoneNumber" );
	
	m_PhoneType.push_back( true );
	m_PhoneType.push_back( true );
	m_PhoneType.push_back( true );
}

PlutoDatabase::~PlutoDatabase(void)
{
	if( m_pContactsTable )
		delete m_pContactsTable;
	if( m_pPhoneTable )
		delete m_pPhoneTable;
}

bool PlutoDatabase::writePhoneNumber(string phoneNumber,int nType,int nUserID)
{
	string sqlInsert;
	int nRows = 0;
	char aux[20];

	m_PhoneValues.clear();
	m_PhoneValues.push_back( itoa(nUserID,aux,10) );
	m_PhoneValues.push_back( itoa(nType,aux,10) );
	m_PhoneValues.push_back( phoneNumber );

	m_pContactsTable->constructInsert( "PhoneNumber", m_PhoneParam, m_PhoneValues, m_PhoneType, sqlInsert );
	return m_pContactsTable->executeSql( sqlInsert, nRows, false );
}

bool PlutoDatabase::writeContact( Contact *pContact )
{
	string sqlInsert;
	int nRows = 0, userId;

	m_ContactValues.clear();

	m_ContactValues.push_back( pContact->firstName + " " + pContact->lastName );
	m_ContactValues.push_back( pContact->companyName );
	m_ContactValues.push_back( pContact->jobDescription );
	m_ContactValues.push_back( pContact->title );
	m_ContactValues.push_back( pContact->email );

	m_pContactsTable->constructInsert( "Contact", m_ContactParam, m_ContactValues, m_ContactType, sqlInsert );
	if( !m_pContactsTable->executeSql( sqlInsert, nRows, false ) )
		return false;

	userId = existContact( pContact );

	if( !pContact->phoneHome.empty() )
		writePhoneNumber( pContact->phoneHome, 1, userId );

	if( !pContact->homeBusiness.empty() )
		writePhoneNumber( pContact->homeBusiness, 2, userId );

	if( !pContact->phoneMobile.empty() )
		writePhoneNumber( pContact->phoneMobile, 3, userId );
}

int PlutoDatabase::existContact(Contact *pContact)
{
	string sqlSelect,cond;
	int nRows = 0;

	cond = string( "NAME='" ) + pContact->firstName + " " + pContact->lastName + "'"; 
	m_pContactsTable->constructSelect( "Contact", cond, sqlSelect);
	if( !m_pContactsTable->executeSql( sqlSelect , nRows ) )
		return 0;

	if( !nRows )
		return -1;
	else 
		return atoi( m_pContactsTable->getFieldValue( "PK_Contact" ).c_str() );
}

int PlutoDatabase::writeContacts(ContactsList &list)
{
	vector<Contact *>::iterator it;
	Contact *pContact;
	int nContact = 0;

	for(it=list.begin();it!=list.end();it++)
	{
		pContact = *it;
		if( existContact(pContact) != -1 )
			MessageBox( NULL, (pContact->firstName + " " + pContact->lastName).c_str(),
				"Already exist", MB_OK );
		else 
			writeContact( pContact );
	}

	return nContact;
}
