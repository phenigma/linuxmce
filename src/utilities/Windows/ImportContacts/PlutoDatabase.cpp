#include "StdAfx.h"
#include ".\plutoDatabase.h"

#include "Log.h"

PlutoDatabase::PlutoDatabase(void)
{
	m_ServerName = "10.0.0.150";
	m_DatabaseName = "pluto_telecom";
	m_User = "root";
	m_Password = "";
	setConnectionString();
	
	m_pContactsTable = NULL;
	m_pPhoneTable = NULL;

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

void PlutoDatabase::setConnectionString(string serverName,string database,
			string user,string password)
{
	m_ServerName = serverName;
	m_DatabaseName = database;
	m_User = user;
	m_Password = password;

	setConnectionString();
}

void PlutoDatabase::setConnectionString()
{
	m_ConnStr = string("Driver=") + "{MySQL ODBC 3.51 Driver}" + ";";
	
	if( !m_ServerName.empty() )
		m_ConnStr += "SERVER=" + m_ServerName + ";";

	m_ConnStr += string("PORT=") + "3306" + ";";
	m_ConnStr += "Data Source Name=PlutoTelecom;";

	if( !m_DatabaseName.empty() )
		m_ConnStr += "Database=" + m_DatabaseName + ";";
	if( !m_User.empty() )
		m_ConnStr += "Uid=" + m_User + ";"; 
}

bool PlutoDatabase::connect()
{
	bool bRes;
	setConnectionString();
	m_pContactsTable = new DatabaseWrapper;
	m_pPhoneTable = new DatabaseWrapper;

	bRes = m_pContactsTable->connect( m_ConnStr );
	if( bRes )
		Log::m_pLog->writeLine( "Connect to Contact table" );
	else
	{
		Log::m_pLog->writeLine( "Couldn't connect to Contact table" );
		return false;
	}
	bRes = m_pPhoneTable->connect( m_ConnStr );
	if( bRes )
		Log::m_pLog->writeLine( "Connect to  Phone table" );
	else
	{
		Log::m_pLog->writeLine( "Couldn't connect to Phone table" );
		return false;
	}

	return true;
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
	string outMess;

	for(it=list.begin();it!=list.end();it++)
	{
		pContact = *it;
		if( existContact(pContact) != -1 )
		{
			outMess = "Already exist:" +  pContact->firstName + " " + pContact->lastName;
			Log::m_pLog->writeLine( outMess );
		}
		else 
			writeContact( pContact );
	}

	return nContact;
}
