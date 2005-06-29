/**
 *
 * @file CommonFunctions.h
 * @brief header file for the GlobalConfig, SerializeableStrings classes; it uses the Table, Repository and Database classes
 * @author
 *
 */
 
#ifndef CommonFunctions_h
#define CommonFunctions_h

#include <iostream>
#include <map>
#include <string>
#include "SerializeClass/SerializeClass.h"
#include "DCE/DCEConfig.h"

#define NULL_TOKEN	"**( NULL )**"

typedef map<string,string> MapStringString;
extern int ValidateUser(string Username,string Password,bool &NoPassword,bool &bSupervisor);

namespace sqlCVS
{
	class Table;
	class Repository;
	class Database;
	class ChangedRow;
	typedef list<ChangedRow *> ListChangedRow;

	typedef map<string,Table *> MapTable;
	typedef map<string,Repository *> MapRepository;

	class ValidatedUser
	{
	public:
		bool m_bWithoutPassword,m_bIsSupervisor;
		int m_psc_user;
		ValidatedUser(int psc_user,bool bWithoutPassword,bool bIsSupervisor) { m_psc_user=psc_user; m_bWithoutPassword=bWithoutPassword; m_bIsSupervisor=bIsSupervisor; }
	};
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

		string m_sSqlCVSHost;	/**< The host name or IP for the sqlCVS listener */
		string m_sComments; /**< Comments to be included during a checkin  */

		string m_sSkipVerification; /**< Table:Field's to be skipped during a verify integrity */

		vector<int> m_vectRestrictions; /** if Restrictions are specified on the command line, they will be here.  0 matches no restrictions */

		int m_iDBPort;		/**< The port of the database */
		int m_iSqlCVSPort;	/**< The port of the sqlCVS */

		int m_psc_batch;    /**< The batch number the user passed on the command line */
		int m_iScreenWidth;  /**< The width of the screen */

		MapRepository m_mapRepository;  /**< The repositories we're currently operating on */
		
		MapTable m_mapTable;  /**< The tables we're currently operating on */

		string m_sUsers;  /**< The users specified on the command line */
		
		map<int,MapTable *> m_mapUsersTables; 	/**< This will have all the users who have made changes,
							 * pointing to a list of the tables they modified
							 */
		MapStringString m_mapUsersPasswords;	/**< This will have all the users and their passwords */
		string m_sDefaultUser; /**< If checking in for multiple users, this will be the one who is assigned ownership of all unclaimed new records */

		Database *m_pDatabase;		/**< points to the database */
		Repository *m_pRepository;	/**< points to the Repository */

		map<int,ValidatedUser *> m_mapValidatedUsers; /**< Map of user id's to bool's if the user is a supervisor */

		map<int, ListChangedRow *> m_mapBatch_ChangedRow;  // Records that couldn't be checked in, by the unauthorized batch number (a negative number)

		bool m_bNewDatabase,m_bVerify,m_bVerifyID,m_bAllowUnmetDependencies,m_bCheckinEveryone,m_bNoPrompts;

		/**
		 * @ brief constructor
		 */
		 
		GlobalConfig()
		{
			m_sTableType="InnoDB";
			m_sDBHost=dceConfig.ReadString("sqlCVS_MySqlHost","localhost");
			m_sDBUser=dceConfig.ReadString("sqlCVS_MySqlUser","root");
			m_sDBPassword=dceConfig.ReadString("sqlCVS_MySqlPassword","");
			m_sDBName=dceConfig.ReadString("sqlCVS_MySqlDatabase","pluto_main");
			m_iDBPort=dceConfig.ReadInteger("sqlCVS_MySqlPort",3306);
			m_iSqlCVSPort=dceConfig.ReadInteger("sqlCVS_Port",3485);
			m_sSqlCVSHost=dceConfig.ReadString("sqlCVS_Host","localhost");
			m_sUsers=dceConfig.ReadString("sqlCVS_Users","");
			m_pDatabase=NULL;
			m_pRepository=NULL;
			m_bNewDatabase=false;
			m_psc_batch=0;
			m_bVerify=m_bVerifyID=m_bAllowUnmetDependencies=m_bCheckinEveryone=m_bNoPrompts=false;
			m_iScreenWidth=dceConfig.ReadInteger("ScreenWidth",120);
		}

		string GetRestrictionClause(vector<int> *p_vectRestrictions=NULL)
		{
			if( p_vectRestrictions==NULL )
				p_vectRestrictions=&m_vectRestrictions;

			if( p_vectRestrictions->size()==0 )
				return "(psc_restrict IS NULL)";

			string sResult;
			bool bIncludeNoRestriction=false;
			for(vector<int>::iterator it=p_vectRestrictions->begin();it!=p_vectRestrictions->end();++it)
			{
				if( (*it) == 0 )
					bIncludeNoRestriction=true;
				else
					sResult += (sResult.size() ? "," : "") + StringUtils::itos(*it);
			}

			if( sResult.size() )
				sResult = "(psc_restrict in (" + sResult + ")";
			
			if( bIncludeNoRestriction && sResult.size() )
				sResult += " OR psc_restrict IS NULL)";
			else if( bIncludeNoRestriction )
				sResult += "(psc_restrict IS NULL)";
			else
				sResult += ")";

			return sResult;
		}

		/**
		 * @ brief return all the validated user id's in a comma delimited string
		 */
		string csvUserID();

		/**
		 * @ brief returns true if the user has been validated
		 */
		bool UserIsValidated(int psc_user)
		{
			return m_mapValidatedUsers.find(psc_user)!=m_mapValidatedUsers.end();
		}

		/**
		 * @ brief validates all the users who logged in
		 */
		bool ValidateUsers(bool &bSupervisor,bool bExpectAtLeastOne=true,MapStringString *pmapUsersPasswords=NULL)
		{
			if( !pmapUsersPasswords )
			{
				pmapUsersPasswords = &m_mapUsersPasswords;
cout << "Map was null, using internal" << endl;
			}
			bSupervisor=false;
			bool bValidatedUser=false;  // Be sure there is at least 1 user that was validated, and the map isn't empty
cout << "Ready to loop.  size is: " << (int) pmapUsersPasswords->size() << endl;
			for(MapStringString::iterator it=pmapUsersPasswords->begin();it!=pmapUsersPasswords->end();++it)
			{
				bool bNoPassword,bSupervisor2;
cout << "before validate user" << endl;
				int psc_user = ValidateUser( (*it).first, (*it).second, bNoPassword, bSupervisor2 );
cout << "validate user:" << psc_user << " bnopass: " << bNoPassword << " is sup: " << bSupervisor2 << endl;
				if( !psc_user )
				{
					return false;
				}
				cout << "Validated user: " << psc_user << " Is sup: " << bSupervisor2 << endl;
				bValidatedUser=true;
				m_mapValidatedUsers[psc_user]=new ValidatedUser(psc_user,bNoPassword,bSupervisor2);
				if( bSupervisor2 && !bNoPassword )
					bSupervisor=true;
			}
cout << "exiting validate user is sup: " << bSupervisor << endl;
			return bExpectAtLeastOne==false || bValidatedUser;
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
