#ifndef Repository_h
#define Repository_h

#include "mysql.h"
#include <iostream>
#include "Table.h"
#include "ChangedRow.h"

namespace sqlCVS
{
	class Repository
	{
		class Database *m_pDatabase;
		class Table *m_pTable_Setting,*m_pTable_BatchHeader,*m_pTable_BatchDetail,*m_pTable_Tables; // All our system tables
		map<int,class ChangedRow *> m_mapUsers2ChangedRow;
		string m_sName;
		MapTable m_mapTable; 

	public:
		Repository(class Database *pDatabase,string sName)
		{
			m_pDatabase=pDatabase;
			m_sName=sName;
			m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchDetail=m_pTable_Tables=NULL;
		}
		string Name_get() { return m_sName; }
		void AddDefinitionTable(class Table *pTable);
		void MatchUpTables();
		void Setup();
		void Remove();
		bool CheckIn();
		void AddTablesToMap();

		class Table *CreateSettingTable();
		class Table *CreateBatchHeaderTable();
		class Table *CreateBatchDetailTable();
		class Table *CreateTablesTable();

	};
	
	typedef map<string,Repository *> MapRepository;
	typedef list<Repository *> ListRepository;
}

#endif
