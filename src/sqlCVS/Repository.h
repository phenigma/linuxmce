/**
 *
 * @file Repository.h
 * @brief header file for the Repository class
 * @author
 *
 */
 
#ifndef Repository_h
#define Repository_h

#include "mysql.h"
#include <iostream>
#include "Table.h"
#include "ChangedRow.h"

class R_ApproveBatch;

namespace sqlCVS
{
	/**
	 * @brief class modelling the repository
	 * @todo complete documentation
	 */
	 
	class Repository
	{
		friend class Database;
		
		/** @brief All our system tables */		
		class Table *m_pTable_Setting;
		class Table *m_pTable_BatchHeader;
		class Table *m_pTable_BatchUser;
		class Table *m_pTable_BatchDetail;
		class Table *m_pTable_Tables;
		class Table *m_pTable_Schema;
		
		string m_sName;
		MapTable m_mapTable; 

	public:
		class Database *m_pDatabase;

		/** @brief constructor */
		Repository( class Database *pDatabase, string sName )
		{
			m_pDatabase=pDatabase;
			m_sName=sName;
			m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchUser=m_pTable_BatchDetail=m_pTable_Tables=m_pTable_Schema=NULL;
		}
		
		/**
		 *  @brief This lists all the tables and what repositories they belong to
		 */
		void ListTables( );

		/** @brief Displays a summary of all the rows changed in the repository.  Called by the 'diff' command.  returns false means the user wants to quit */
		bool ShowChanges();

		string Name_get( ) { return m_sName; }
		void AddDefinitionTable( class Table *pTable );
		void MatchUpTables( );
		void Setup( );
		void Remove( );
		bool CheckIn( );
		bool Update( );
		void Dump( );
		bool DetermineDeletions( );
		void AddTablesToMap( );
		int CreateBatch( sqlCVSprocessor *psqlCVSprocessor, map<int,ValidatedUser *> *mapValidatedUsers );
		int psc_id_last_sync_get( Table *pTable );
		void psc_id_last_sync_set( Table *pTable, int psc_id );
		int psc_batch_last_sync_get( Table *pTable );
		void psc_batch_last_sync_set( Table *pTable, int psc_id );
		bool HasChangedRecords();  /** @brief True if there are changed records in any of the tables */

		/** @brief Rebuilds all the system tables in case something changed */
		void ResetSystemTables();

		class Table *CreateSettingTable( );
		class Table *CreateBatchHeaderTable( );
		class Table *CreateBatchUserTable( );
		class Table *CreateBatchDetailTable( );
		class Table *CreateTablesTable( );
		class Table *CreateSchemaTable( );
	
		void VerifyIntegrity() { }

		string GetSetting(string Setting,string Default);
		void SetSetting(string Setting,string Value);
		void ImportTable(string sTableName,SerializeableStrings &str,size_t &pos,Table *pTable);
		bool UpdateSchema(int PriorSchema);

		bool ApproveBatch(R_ApproveBatch *pR_ApproveBatch,sqlCVSprocessor *psqlCVSprocessor);

		class Table *m_mapTable_Find( string sTable ) { MapTable::iterator it = m_mapTable.find( sTable ); return it==m_mapTable.end( ) ? NULL : ( *it ).second; }
	};
	
	typedef map<string, Repository *> MapRepository;
	typedef list<Repository *> ListRepository;
}

#endif
