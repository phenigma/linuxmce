#ifndef CommonFunctions_h
#define CommonFunctions_h

#define SCREEN_WIDTH	120
#include <iostream>
#include <map>
#include <string>
#include "SerializeClass/SerializeClass.h"

namespace sqlCVS
{
	class Table;
	class Repository;
	class Database;
	typedef map<string,Table *> MapTable;
	typedef map<string,Repository *> MapRepository;

	class GlobalConfig
	{
	public:
		string m_sTableType;
		string m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName;  // MySQL info
		string m_sCommand,m_sRepository,m_sTable; // Command lines
		string m_sPSC_Host; // The host name or IP for the sqlCVS listener
		int m_iDBPort,m_iSqlCVSPort;  // The ports

		MapRepository m_mapRepository;  // The repositories we're currently operating on
		MapTable m_mapTable;  // The tables we're currently operating on
		map<int,MapTable *> m_mapUsersTables; 	// This will have all the users who have made changes, pointing to a list of the tables they modified
		map<int,string> m_mapUsersPasswords;

		Database *m_pDatabase;
		Repository *m_pRepository;

		GlobalConfig()
		{
			m_sTableType="InnoDB";
			m_sDBHost="10.0.0.150";
			m_sDBUser="root";
			m_sDBName="pluto_main";
			m_iDBPort=3306;
			m_iSqlCVSPort=3485;
			m_pDatabase=NULL;
			m_pRepository=NULL;
		}
	};

	class SerializeableStrings : public SerializeClass
	{
	public:
		vector<string> m_vectString;
		void SetupSerialization() {	StartSerializeList() + m_vectString; 	}
	};

	extern GlobalConfig g_GlobalConfig;

	void NewSection();
}
#include <string>
#include <map>
using namespace std;

string int2string(int i);
string getTableFromForeignKey(string field,map<string,class TableInfo_Generator *> *mapTableNames);

#endif
