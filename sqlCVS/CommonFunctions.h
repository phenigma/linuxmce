/**
 *
 * @file CommonFunctions.h
 * @brief header file for the GlobalConfig, SerializeableStrings classes; it uses the Table, Repository and Database classes
 * @author
 *
 */
 
#ifndef CommonFunctions_h
#define CommonFunctions_h

#define SCREEN_WIDTH	120
#include <iostream>
#include <map>
#include <string>
#include "SerializeClass/SerializeClass.h"
#include "DCE/DCEConfig.h"

#define NULL_TOKEN	"**( NULL )**"

namespace sqlCVS
{
	class Table;
	class Repository;
	class Database;
	typedef map<string,Table *> MapTable;
	typedef map<string,Repository *> MapRepository;

	/**
	 * @brief  class modelling the global configuration
	 */
	 
	class GlobalConfig
	{
	public:
		DCEConfig dceConfig;
		string m_sTableType;	/**< the table type */
		
		string m_sDBHost;	/**< MySQL information: the host */
		string m_sDBUser;	/**< MYSQL information: the user */
		string m_sDBPassword;	/**< MYSQL information: the password */
		string m_sDBName;	/**< MySQL information: the name */
		
		string m_sCommand;      /**< Command line */
		string m_sRepository;   /**< Command line  */
		string m_sTable; 	/**< Command line  */
		
		string m_sPSC_Host;	/**< The host name or IP for the sqlCVS listener */
		
		int m_iDBPort;		/**< The port of the database */
		int m_iSqlCVSPort;	/**< The port of the sqlCVS */

		MapRepository m_mapRepository;  /**< The repositories we're currently operating on */
		
		MapTable m_mapTable;  /**< The tables we're currently operating on */

		string m_sUsers;  /**< The users specified on the command line */
		
		map<int,MapTable *> m_mapUsersTables; 	/**< This will have all the users who have made changes,
							 * pointing to a list of the tables they modified
							 */
		map<string,string> m_mapUsersPasswords;	/**< This will have all the users and their passwords */
		string m_sDefaultUser; /**< If checking in for multiple users, this will be the one who is assigned ownership of all unclaimed new records */

		Database *m_pDatabase;		/**< points to the database */
		Repository *m_pRepository;	/**< points to the Repository */

		bool m_bNewDatabase;

		/**
		 * @ brief constructor
		 */
		 
		GlobalConfig()
		{
			m_sTableType="InnoDB";
			m_sDBHost=dceConfig.ReadString("sqlCVS_MySqlHost","localhost");
			m_sDBUser=dceConfig.ReadString("sqlCVS_MySqlUser","root");
			m_sDBName=dceConfig.ReadString("sqlCVS_MySqlDatabase","pluto_main");
			m_iDBPort=dceConfig.ReadInteger("sqlCVS_MySqlPort",3306);
			m_iSqlCVSPort=dceConfig.ReadInteger("sqlCVS_Port",3485);
			m_sUsers=dceConfig.ReadString("sqlCVS_Users","");
			m_pDatabase=NULL;
			m_pRepository=NULL;
			m_bNewDatabase=false;
		}
	};

	/**
	 * @brief  class modelling the serializable strings
	 * @todo complete documentation
	 */
	 
	class SerializeableStrings : public SerializeClass
	{
	public:
		vector<string> m_vectString;
		
		/**
		 * @brief Sets up serialization
		 */
		
		void SetupSerialization(int iSC_Version) {	StartSerializeList() + m_vectString; 	}
	};

	extern GlobalConfig g_GlobalConfig;

	/**
	 * @brief   brief documentation 
	 * @todo complete documentation
	 */
	 
	void NewSection();
}
#include <string>
#include <map>
using namespace std;

string int2string(int i); /**< casts ints to strings */


string getTableFromForeignKey(string field,map<string,class TableInfo_Generator *> *mapTableNames);

#endif
