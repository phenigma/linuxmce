#ifndef ChangedRow_h
#define ChangedRow_h

#include "mysql.h"
#include "Field.h"
#include <iostream>

namespace sqlCVS
{
	// This is a general dependency between 2 tables
	class Dependency
	{
		class Field *m_pField_Dependent, *m_pFieldinfo_Master;
		class Field *m_pField_Indirect;

		Dependency(class Field *pField_Dependent,class Field *pField_Master,class Field *pField_Indirect)
		{
			m_pField_Dependent=pField_Dependent;
			m_pFieldinfo_Master=pField_Master;
			m_pField_Indirect=pField_Indirect;
		}
	};

	// This is a particular row that depends on another
	class DependentRow
	{
		class Dependency *m_pDependency;  // The general dependency
		class ChangedRow *m_pChangedRow_Master;
		class ChangedRow *m_pChangedRow_Dependent;
		int m_psc_id_Dependent, m_psc_id_Master;

		DependentRow(class Dependency *pDependency)
		{
			m_pDependency=pDependency;
			m_pChangedRow_Dependent=NULL;
			m_psc_id_Dependent=-1;

			m_pChangedRow_Master=NULL;
			m_psc_id_Master=-1;
		}
	};
	
	typedef list<DependentRow *> ListDependentRow;

	typedef pair<ChangedRow *, ListDependentRow *> AutoIncrChanges;
	typedef map<int,AutoIncrChanges> MapAutoIncrChanges;
	typedef pair<Field *,char *> FieldValues;

	enum TypeOfChange { toc_New, toc_Delete, toc_Modify };

	// This is a row that has been changed
	class ChangedRow
	{
	public:
		int m_psc_id, m_psc_batch, m_psc_user;
		int m_iOriginalAutoIncrID,m_iNewAutoIncrID;
		bool m_bCommitted;
		class Table *m_pTable;
		list<string> m_listPrimaryKey;
		TypeOfChange m_eTypeOfChange;

		ChangedRow(class Table *pTable, TypeOfChange eTypeOfChange, int psc_id, int psc_batch, int psc_user, list<string> &listPrimaryKey)
		{
			m_pTable=pTable;
			m_listPrimaryKey = listPrimaryKey;
			m_eTypeOfChange = eTypeOfChange;
			m_psc_id=psc_id; m_psc_batch=psc_batch; m_psc_user=psc_user;
			m_iOriginalAutoIncrID=m_iNewAutoIncrID=-1;
			m_bCommitted=false;
		}
	};
	typedef list<ChangedRow *> ListChangedRow;
}

#endif
