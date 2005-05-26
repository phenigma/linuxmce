#ifndef __Database_pluto_speech_H_
#define __Database_pluto_speech_H_
#include <mysql.h>
#include "PlutoUtils/MySQLHelper.h"
#ifdef WIN32
#ifdef EXPORT_DLL
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllexport)
	#endif
#else
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllimport)
	#endif
#endif
#else
#define DECLSPECIFIER
#endif
class DECLSPECIFIER MySqlHelper;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Database_pluto_speech: public MySqlHelper
{
public:
Database_pluto_speech();
~Database_pluto_speech();
void DeleteAllTables();
private:
class Table_Alias* tblAlias;
class Table_Keyword* tblKeyword;
public:
class Table_Alias* Alias_get() { if( !tblAlias ) CreateTable_Alias(); return tblAlias; }
class Table_Keyword* Keyword_get() { if( !tblKeyword ) CreateTable_Keyword(); return tblKeyword; }
string m_sLastMySqlError;
bool Connect(string host, string user, string pass, string sDBName, int port);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
private:
void CreateTable_Alias();
void CreateTable_Keyword();
void DeleteTable_Alias();
void DeleteTable_Keyword();
};
#endif
