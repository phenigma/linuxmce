#include "DCE/DCEMySqlConfig.h"
//------------------------------------------------------------------------------------------------------
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------------------------------
DCEMySqlConfig::DCEMySqlConfig() : m_MySqlMutex("MYSQL"), m_iTransNumber(0)
{
	m_MySqlMutex.Init(NULL);

	MysqlConnect(false);
}
//------------------------------------------------------------------------------------------------------
/*virtual*/ DCEMySqlConfig::~DCEMySqlConfig()
{
	mysql_close(&m_MySQL);
}
//------------------------------------------------------------------------------------------------------
bool DCEMySqlConfig::MysqlConnect(bool bReset)
{
	PLUTO_SAFETY_LOCK(ms, m_MySqlMutex);

	if( bReset )
		mysql_close(&m_MySQL);

	mysql_init(&m_MySQL);
	if(! mysql_real_connect(&m_MySQL, m_sDBHost.c_str(), m_sDBUser.c_str(), m_sDBPassword.c_str(), 
		m_sDBName.c_str(), 0, NULL,0)
	)
	{
		cout << "Failed to connect to MySQL:" << mysql_error(&m_MySQL) << endl; 
		return false;
	} 

	return true;
}
//------------------------------------------------------------------------------------------------------
MYSQL_RES *DCEMySqlConfig::MySqlQueryResult(string sQuery)
{
	PLUTO_SAFETY_LOCK(sl, m_MySqlMutex);

	long iResult;
	if( (iResult = mysql_query(&m_MySQL, sQuery.c_str())) != 0 )
	{
		sl.Release();
		cout << "Query failed" << mysql_error(&m_MySQL) << sQuery.c_str() << iResult << endl;
		MysqlConnect(true);
		return NULL;
	}

	return mysql_store_result(&m_MySQL);
}
//------------------------------------------------------------------------------------------------------
long DCEMySqlConfig::ThreadedMysqlQuery(string sQuery, bool bIgnoreErrors/*= false*/)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);

	long iResult;
	if( (iResult = mysql_query(&m_MySQL, sQuery.c_str())) != 0 )
	{
		sl.Release();
		if( bIgnoreErrors )
			return -1;

		cout << "Query failed " << mysql_error(&m_MySQL) << sQuery << iResult << endl;
		MysqlConnect(true);
	}

	return iResult;
}
//------------------------------------------------------------------------------------------------------
long DCEMySqlConfig::ThreadedMysqlQueryWithID(string sQuery)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);

	long iResult;
	if( (iResult = mysql_query(&m_MySQL, sQuery.c_str())) != 0 )
	{
		sl.Release();

		cout << "Query failed " << mysql_error(&m_MySQL) << sQuery << iResult << endl; 
		MysqlConnect(true);
		return 0;
	}

	return (long) mysql_insert_id(&m_MySQL);
}
//------------------------------------------------------------------------------------------------------
