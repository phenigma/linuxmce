#ifndef MySqlHelper_h
#define MySqlHelper_h

#include <mysql.h>
#include "DCE/Logger.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using namespace DCE;

// Use this to be sure the results are automatically freed
class PlutoSqlResult
{
public:
	MYSQL_RES *r;
	PlutoSqlResult() : r(NULL) {};
	~PlutoSqlResult() { 
		if( r )
			mysql_free_result(r);
	}
};

// You can add this to a base class to get some helper mysql functions
class MySqlHelper
{
public:
	pluto_pthread_mutex_t m_MySqlMutex;
	MYSQL *m_pMySQL;
	bool m_bConnected;
	string m_sMySQLHost,m_sMySQLUser,m_sMySQLPass,m_sMySQLDBName;
	int m_iMySQLPort;

	MySqlHelper()
		: m_MySqlMutex("mysql")
	{
		m_pMySQL=NULL;
		m_bConnected=false;
		m_MySqlMutex.Init(NULL);
	}
	
	MySqlHelper(string host, string user, string pass, string db_name, int port=3306)
		: m_MySqlMutex("mysql")
	{
		m_MySqlMutex.Init(NULL);
		m_pMySQL = mysql_init(NULL);
		m_sMySQLHost=host;
		m_sMySQLUser=user;
		m_sMySQLPass=pass;
		m_sMySQLDBName=db_name;
		m_iMySQLPort=port;
		MySQLConnect();
	}

	virtual ~MySqlHelper()
	{
		if( m_pMySQL )
			mysql_close(m_pMySQL);
		m_pMySQL=NULL;
	}

	bool MySQLConnect(string host, string user, string pass, string db_name, int port=3306,bool bReset=false)
	{
		m_pMySQL = mysql_init(NULL);
		m_sMySQLHost=host;
		m_sMySQLUser=user;
		m_sMySQLPass=pass;
		m_sMySQLDBName=db_name;
		m_iMySQLPort=port;
		return MySQLConnect(bReset);
	}

	bool MySQLConnect(bool bReset=false)
	{
		if( bReset && m_bConnected )
		{
			mysql_close(m_pMySQL);
			m_pMySQL = mysql_init(NULL);
		}

		if (mysql_real_connect(m_pMySQL, m_sMySQLHost.c_str(), m_sMySQLUser.c_str(), m_sMySQLPass.c_str(), m_sMySQLDBName.c_str(), m_iMySQLPort, NULL, 0) == NULL)
			m_bConnected=false;
		else
			m_bConnected=true;

		return m_bConnected;
	}

	void SetConnection(MYSQL *pMySQL)
	{
		m_pMySQL = pMySQL;
		m_bConnected = true;
	}

	string md5(string Input)
	{
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( (result_set.r=mysql_query_result("SELECT md5('" + Input +"')"))==0 || (row = mysql_fetch_row(result_set.r))==NULL )
			throw "error getting md5";

		return row[0];
	}

	MYSQL_RES *mysql_query_result(string query)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		int iresult;
		if( (iresult=mysql_query(m_pMySQL,query.c_str()))!=0 )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",mysql_error(m_pMySQL),query.c_str(),iresult);
			MySQLConnect(true);
			return NULL;
		}
	//	else
	//		g_pPlutoLogger->Write(LV_WARNING,"Query succeeded: %s (%d)",query.c_str(),iresult);

	//	g_pPlutoLogger->Write(LV_STATUS,"store result: %s",query.c_str());

	#ifdef DEBUG
		clock_t cStop = clock();
		if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
		{
			g_pPlutoLogger->Write(LV_WARNING,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		}
	#endif

		MYSQL_RES *pMYSQL_RES = mysql_store_result(m_pMySQL);
#ifdef LOG_ALL_QUERIES
		g_pPlutoLogger->Write(LV_STATUS,"Query returned %d rows: %s",(int) (pMYSQL_RES ? pMYSQL_RES->row_count : 0), query.c_str());
#endif
		return pMYSQL_RES;
	}

	int threaded_mysql_query(string query,bool bIgnoreErrors=false)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		int iresult;
		if( (iresult=mysql_query(m_pMySQL,query.c_str()))!=0 )
		{
			if( bIgnoreErrors )
				return -1;
			g_pPlutoLogger->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",mysql_error(m_pMySQL),query.c_str(),iresult);
			MySQLConnect(true);
		}

	#ifdef DEBUG
		clock_t cStop = clock();
		if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
		{
			g_pPlutoLogger->Write(LV_WARNING,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		}
	#endif
#ifdef LOG_ALL_QUERIES
		g_pPlutoLogger->Write(LV_STATUS,"Executed query %s", query.c_str());
#endif
		return iresult;
	}

	int threaded_mysql_query_withID(string query)
	{
	#ifdef DEBUG
		clock_t cStart = clock();
	#endif
		PLUTO_SAFETY_LOCK_ERRORSONLY(sl,m_MySqlMutex);
		int iresult;
		if( (iresult=mysql_query(m_pMySQL,query.c_str()))!=0 )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Query failed (%s): %s (%d)",mysql_error(m_pMySQL),query.c_str(),iresult);
			MySQLConnect(true);
			return 0;
		}

	#ifdef DEBUG
		clock_t cStop = clock();
		if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		} else if( cStop-cStart>CLOCKS_PER_SEC/10 )
		{
			g_pPlutoLogger->Write(LV_WARNING,"Query: %s took %d ms",query.c_str(),(int) (cStop-cStart));
		}
	#endif
#ifdef LOG_ALL_QUERIES
		g_pPlutoLogger->Write(LV_STATUS,"Query has ID %d: %s", (int) mysql_insert_id(m_pMySQL), query.c_str());
#endif
		return (int) mysql_insert_id(m_pMySQL);
	}
};
#endif
