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

namespace sqlCVS
{
	/**
	 * @brief class modelling the repository
	 * @todo complete documentation
	 */
	 
	class Repository
	{
		class Database *m_pDatabase;
		
		/** @brief All our system tables */		
		class Table *m_pTable_Setting;
		class Table *m_pTable_BatchHeader;
		class Table *m_pTable_BatchDetail;
		class Table *m_pTable_Tables; 
		
		map<int, class ChangedRow *> m_mapUsers2ChangedRow;
		string m_sName;
		MapTable m_mapTable; 

	public:
		/** @brief constructor */
		Repository( class Database *pDatabase, string sName )
		{
			m_pDatabase=pDatabase;
			m_sName=sName;
			m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchDetail=m_pTable_Tables=NULL;
		}
		
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
		int CreateBatch( );
		int psc_id_last_sync_get( Table *pTable );
		void psc_id_last_sync_set( Table *pTable, int psc_id );
		int psc_batch_last_sync_get( Table *pTable );
		void psc_batch_last_sync_set( Table *pTable, int psc_id );

		class Table *CreateSettingTable( );
		class Table *CreateBatchHeaderTable( );
		class Table *CreateBatchDetailTable( );
		class Table *CreateTablesTable( );

	};
	
	typedef map<string, Repository *> MapRepository;
	typedef list<Repository *> ListRepository;
}

#endif
