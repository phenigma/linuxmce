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
//#include "Repository.h"
#include "Database.h"
//#include "Table.h"
#include "PlutoUtils/MySQLHelper.h"

namespace Tribune
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
		/** @brief constructor */
		
		Database( string db_host, string db_user, string db_pass, string db_name, int db_port );
		 
		MYSQL *MYSQL_get( ) { return m_pMySQL; }

		void StartTransaction( );
		void Commit( );
		void Rollback( );

		string Name_get( ) { return m_sMySQLDBName; }
		bool bIsInvalid( ) { return m_bInvalid; }
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
		SafetyTransaction( class Database *pDatabase ) { m_pDatabase=pDatabase; m_pDatabase->StartTransaction( ); m_bIsOpen=true; printf("Starting transaction: %p\n",this); }
		
		/** @brief destructor */
		~SafetyTransaction( ) 
		{ 
			if( m_bIsOpen )
			{
				printf("Transaction %p is open.  Rolling back\n",this);
				cerr << "System error: Rolling back an incomplete transaction" << endl;
				m_pDatabase->Rollback( );
			}
		}
		
		/**
		 * @brief This will roll back the transaction
		 */
		 void Rollback( ) 
		 {
			 if( m_bIsOpen )
			 {
				printf("Transaction %p is open.  Explicit rolling back\n",this);
				m_pDatabase->Rollback( ); 
				m_bIsOpen=false; 
			 }
			 else
				printf("Transaction %p is closed.  Ignoring explicit rolling back\n",this);
		 }
		 
		 /**
		  * @brief  This will commit the transaction
		  */
		 void Commit( ) 
		 { 
			 if( m_bIsOpen )
			 {
				printf("Transaction %p is open.  Explicit Commit\n",this);
				m_pDatabase->Commit( ); 
				m_bIsOpen=false; 
			 }
			 else
				printf("Transaction %p is closed.  Ignoring explicit Commit\n",this);
		}
		 /**
		  * @brief  True if the transaction is open or pending and has not been rolledback or committed
		  */
		 bool m_bIsOpen_get() { return m_bIsOpen; }
	};
}
#endif
