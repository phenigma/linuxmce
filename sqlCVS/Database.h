#ifndef Database_h
#define Database_h

#include "mysql.h"
#include <iostream>
#include "Repository.h"
#include "Database.h"
#include "Table.h"
#include "PlutoUtils/MySQLHelper.h"

#pragma warning("do new's first, resetting the pkid, and the mod, so we can then do the mod's which will include the changed id's, lastly delete'sdon't need new, delete in tables (new = mod and no batch), delete we'll calculate automatically")
#pragma warning("for each table, keep last batch, and last pscid.  to calculate delete's, get list of pscid id's from server.  Delete any local rows deleted on server, then fire delete actions.  server returns unsorted--do on client side to minimize server load.")

namespace sqlCVS
{
	class Database : public MySqlHelper
	{
		MapRepository m_mapRepository;
		MapTable m_mapTable;
		bool m_bInvalid;

	public:
		// Whenever we have to prompt for anything, this flag will be set to true and we will confirm changes
		// If the user specified everything on the command line, then we will not ask for confirmation
		bool m_bInteractiveMode;

		Database(string db_host, string db_user, string db_pass, string db_name, int db_port);

		// This will take a string of repositories and tables, and add the actual names to the lists
		void GetRepositoriesTables();
		void CreateRepository();
		int PromptForTablesInRepository(Repository *pRepository,MapTable &mapTable);
		size_t MaxTablenameLength();

		MYSQL *MYSQL_get() { return &m_MySQL; }

		class Repository *m_mapRepository_Find(string sRepository) { MapRepository::iterator it = m_mapRepository.find(sRepository); return it==m_mapRepository.end() ? NULL : (*it).second; }
		class Table *m_mapTable_Find(string sTable) { MapTable::iterator it = m_mapTable.find(sTable); return it==m_mapTable.end() ? NULL : (*it).second; }
		void m_mapTable_Remove(string sTable) { MapTable::iterator it = m_mapTable.find(sTable); if( it!=m_mapTable.end() ) m_mapTable.erase(it); }
		void m_mapTable_Add(string sTable,class Table *pTable) { m_mapTable[sTable]=pTable; }

		class Table *GetTableFromForeignKey(class Field *pField);
		class Table *GetTableFromForeignKeyString(Table *pTable,string sField);

		// Implement all these
		void Lock() {}
		void Unlock() {}
		void StartTransaction() {}
		void Commit() {}
		void Rollback() {}

		void CheckIn();
		void Update();
		void Dump();
		void Import();
		void Import(string sRepository,Repository *pRepository);
		int PromptForRepositories();
		int PromptForSqlCvsFiles();
		void GetTablesToCheckIn();
		int ConfirmUsersToCheckIn();
		bool ConfirmRecordsToCheckIn();

		string Name_get() { return m_sMySQLDBName; }
		bool bIsInvalid() { return m_bInvalid; }
	};

	class SafetyDBLock
	{
		bool m_bIsLocked;
		Database *m_pDatabase;
	public:
		SafetyDBLock(class Database *pDatabase) { m_pDatabase=pDatabase; m_pDatabase->Lock(); m_bIsLocked=true; }
		~SafetyDBLock() { if( m_bIsLocked ) Unlock(); }
		void Unlock() { m_pDatabase->Unlock(); m_bIsLocked=false; }
	};

	class SafetyTransaction
	{
		Database *m_pDatabase;
		bool m_bIsOpen;
	public:
		SafetyTransaction(class Database *pDatabase) { m_pDatabase=pDatabase; m_pDatabase->StartTransaction(); m_bIsOpen=true; }
		~SafetyTransaction() 
		{ 
			if( m_bIsOpen )
			{
				cerr << "System error: Rolling back an incomplete transaction";
				m_pDatabase->Rollback();
				throw "Transaction error";
			}
		}
		void Rollback() { m_pDatabase->Rollback(); m_bIsOpen=false; }
		void Commit() { m_pDatabase->Commit(); m_bIsOpen=false; }
	};
}
#endif
