#ifndef __DCEMYSQLCONFIG_H__
#define __DCEMYSQLCONFIG_H__
//------------------------------------------------------------------------------------------------------
#include "DCEConfig.h"
#include "PlutoUtils/MultiThreadIncludes.h"
//------------------------------------------------------------------------------------------------------
#include <mysql.h>
//------------------------------------------------------------------------------------------------------
class DCEMySqlConfig : public DCEConfig
{
protected:
	pluto_pthread_mutex_t m_MySqlMutex;
	MYSQL m_MySQL;

public:
	int m_iTransNumber; //maybe we should derive  a class from DCEMySqlConfig with VIPServer specifics (?)

public:
	DCEMySqlConfig();
	virtual ~DCEMySqlConfig();

	bool MysqlConnect(bool bReset);
	MYSQL_RES *MySqlQueryResult(string sQuery);
	long ThreadedMysqlQuery(string sQuery, bool bIgnoreErrors = false);
	long ThreadedMysqlQueryWithID(string sQuery);
};
//------------------------------------------------------------------------------------------------------
#endif //__DCEMYSQLCONFIG_H__