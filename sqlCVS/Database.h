/**
 *
 * @file Database.h
 * @brief header file for the Database, SafetyDBLock, SafetyTransaction classes
 * @author
 *
 */
 
#ifndef Database_h
#define Database_h

#include "mysql.h"
#include <iostream>
#include "Repository.h"
#include "Database.h"
#include "Table.h"
#include "PlutoUtils/MySQLHelper.h"

#pragma warning( "do new's first, resetting the pkid, and the mod, so we can then do the mod's which will include the changed id's, lastly delete'sdon't need new, delete in tables ( new = mod and no batch ), delete we'll calculate automatically" )
#pragma warning( "for each table, keep last batch, and last pscid. to calculate delete's, get list of pscid id's from server. Delete any local rows deleted on server, then fire delete actions. server returns unsorted--do on client side to minimize server load." )

namespace sqlCVS
{
	/**
	 * @brief class modelling the database
	 * @todo complete documentation
	 */
	 
	class Database : public MySqlHelper
	{
		MapRepository m_mapRepository;
		MapTable m_mapTable;
		bool m_bInvalid;

	public:
		/**
		 * @brief Whenever we have to prompt for anything, this flag will be set to true and we will confirm changes 
		 * If the user specified everything on the command line, then we will not ask for confirmation
		 */
		bool m_bInteractiveMode;

		/** @brief constructor */
		
		Database( string db_host, string db_user, string db_pass, string db_name, int db_port );

		/** @brief This will take a string of repositories and tables, and add the actual names to the lists */
		void GetRepositoriesTables( );
		
		/** 
		 * @ brief This will create the repository 
		 */
		
		void CreateRepository( );
		
		/**
		 * @brief brief documentation
		 * @todo complete documentation
		 */
		 
		int PromptForTablesInRepository( Repository *pRepository, MapTable &mapTable );
		
		/** @brief This will return the maximum length of the tablename */
		size_t MaxTablenameLength( );

		/** 
		 * @brief brief documentation
		 * @todo complete documentation
		 */
		 
		MYSQL *MYSQL_get( ) { return &m_MySQL; }

		/**
		 * @brief This will search in the repository 
		 */
		 
		class Repository *m_mapRepository_Find( string sRepository ) 
		{
		 MapRepository::iterator it = m_mapRepository.find( sRepository ); 
		 return it==m_mapRepository.end( ) ? NULL : ( *it ).second; 
		 }
		
		/**
		 * @brief This will search in the Table
		 */
		 
		class Table *m_mapTable_Find( string sTable ) 
		{ 
		MapTable::iterator it = m_mapTable.find( sTable ); 
		return it==m_mapTable.end( ) ? NULL : ( *it ).second; 
		}
		
		/**
		 * @brief This will remove a record from the Table
		 */
		 
		void m_mapTable_Remove( string sTable ) 
		{ 
		MapTable::iterator it = m_mapTable.find( sTable ); 
		if( it!=m_mapTable.end( ) ) m_mapTable.erase( it ); 
		}
		
		/**
		 * @brief  This will add a record in the Table
		 */
		 
		void m_mapTable_Add( string sTable, class Table *pTable ) { m_mapTable[sTable]=pTable; }

		/**
		 *  @brief This will search the tables reffered to in the Field paramater
		 */
		 
		class Table *GetTableFromForeignKey( class Field *pField );
		
		/**
		 *  @brief This will search the tables reffered to in the Table parameter
		 */
		 		
		class Table *GetTableFromForeignKeyString( Table *pTable, string sField );

		/**
		 *  @brief This lists all the tables and what repositories they belong to
		 */
		void ListTables( );

		/**
		 *  @brief List all the repositories and what tables they have
		 */
		void ListRepositories( );

		/**
		 *  @brief Turn on/off history tracking for all tables
		 */
		void HasFullHistory_set_all( bool bOn );

		/**
		 *  @brief Reset all the tables, updating the psc_id, clearing out the 'mod', recreating the system tables, etc.
		 */
		void Reset_all();

		/**
		 *  @brief Update the psc_id fields for any records that are new
		 */
		void Update_psc();

		/** Implement all these */
		void Lock( ) {}
		void Unlock( ) {}
		void StartTransaction( ) {}
		void Commit( ) {}
		void Rollback( ) {}

		void CheckIn( );
		void Update( );
		void Dump( );
		void Import( );
		void Import( string sRepository, Repository *pRepository );
		int PromptForRepositories( );
		int PromptForSqlCvsFiles( );
		void GetTablesToCheckIn( );
		int ConfirmUsersToCheckIn( );
		bool ConfirmRecordsToCheckIn( );

		string Name_get( ) { return m_sMySQLDBName; }
		bool bIsInvalid( ) { return m_bInvalid; }
	};

	/**
	 *  @brief This will manage the locking/unlocking of the database
	 */
		 	
	class SafetyDBLock
	{
		bool m_bIsLocked;	/**< This will say whether the database is locked or not */
		Database *m_pDatabase;  /**< Points to the database */
	public:
		/** @brief constructor */
		SafetyDBLock( class Database *pDatabase ) { m_pDatabase=pDatabase; m_pDatabase->Lock( ); m_bIsLocked=true; }
		
		/** @brief destructor */
		~SafetyDBLock( ) { if( m_bIsLocked ) Unlock( ); }
		
		/** @brief This will unlock the database */
		void Unlock( ) { m_pDatabase->Unlock( ); m_bIsLocked=false; }
	};

	/**
	 * @brief  This will manage the transactions in the database
	 */
	 
	class SafetyTransaction
	{
		Database *m_pDatabase;	/**< Points to the database */
		bool m_bIsOpen;		/**< This will say whether the database is opened or not */
	public:
		/** @brief constructor */
		SafetyTransaction( class Database *pDatabase ) { m_pDatabase=pDatabase; m_pDatabase->StartTransaction( ); m_bIsOpen=true; }
		
		/** @brief destructor */
		~SafetyTransaction( ) 
		{ 
			if( m_bIsOpen )
			{
				cerr << "System error: Rolling back an incomplete transaction";
				m_pDatabase->Rollback( );
				throw "Transaction error";
			}
		}
		
		/**
		 * @brief This will roll back the transaction
		 */
		 void Rollback( ) { m_pDatabase->Rollback( ); m_bIsOpen=false; }
		 
		 /**
		  * @brief  This will commit the transaction
		  */
		 void Commit( ) { m_pDatabase->Commit( ); m_bIsOpen=false; }
	};
}
#endif
