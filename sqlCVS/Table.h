#ifndef Table_h
#define Table_h

#include "mysql.h"
#include "Field.h"
#include "ChangedRow.h"
#include "Socket.h"

#include <iostream>

class RA_Processor;

namespace sqlCVS
{
	typedef map<string,class Table *> MapTable;
	typedef list<class Table *> ListTable;

	class Table
	{
		class Database *m_pDatabase;

		// We will put a list of all new rows here and all the rows dependent on this one, stored by the ID.  After we have added all the rows, and merged
		// in all the modifications, then we will go through the map of dependencies (m_mapDependentRows) and see what has changed.  If we find a list of changes
		// and it refers to a changed row that has not been committed, that is a fatal error.
		MapAutoIncrChanges m_mapAutoIncrChanges;

		map<int,ListChangedRow *> m_mapUsers2ChangedRowList;

#pragma warning("this seems retarded with teh ::")
		MapField m_mapField;
		string m_sName;
		class Repository *m_pRepository;
		bool m_bIsSystemTable,m_bHasAutoIncrement;
		class Field *m_pField_id, *m_pField_batch, *m_pField_user, *m_pField_mod;
		class Table *m_pTable_History,*m_pTable_WeAreHistoryFor;  // A pointer to our history table, if we have one, or the table we are logging history for if we're a history table

	public:
		Table(class Database *pDatabase,string sName);
		~Table();

		string GetTrailingString(string sName="");  // If the table is in the format some_text_something it returns 'something'--the text following the final _
		string DefinesRepository();  // If this is one of the tables defining a repository (ie psc_RepositoryName_myfunction) it returns RepositoryName, otherwise empty string
		void SetRepository(class Repository *pRepository,bool bIsSystemTable) { m_pRepository=pRepository; m_bIsSystemTable=bIsSystemTable; }

		void GetFields();
		void GetDependencies();
		void MatchUpHistory();
		bool HasFullHistory_get() { return m_pTable_History!=NULL; }
		void HasFullHistory_set(bool bOn);

		void GetChanges();
		bool CheckIn(RA_Processor &ra_Processor,DCE::Socket *pSocket,enum TypeOfChange toc);

		bool TrackChanges_get() { return m_pField_id && m_pField_batch && m_pField_user && m_pField_mod; }
		void TrackChanges_set(bool bOn);

		class Field *m_mapField_Find(string sField) { MapField::iterator it = m_mapField.find(sField); return it==m_mapField.end() ? NULL : (*it).second; }

		// Accessors
		string Name_get() { return m_sName; }
		class Repository *Repository_get() { return m_pRepository; }
	};
}

#endif
