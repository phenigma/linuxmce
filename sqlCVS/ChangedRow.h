/**
 *
 * @file ChangedRow.h
 * @brief header file for the Dependency, DependetRow and ChangedRow classes
 * @author
 *
 */

  
#ifndef ChangedRow_h
#define ChangedRow_h

#include "mysql.h"
#include <iostream>

namespace sqlCVS
{
	/**
	 * @brief  This is a general dependency between 2 tables
	 */
	 
	class Dependency
	{
		class Field *m_pField_Dependent;
		class Field *m_pFieldinfo_Master;
		class Field *m_pField_Indirect;
	
		/**
	 	 * @brief constructor
	 	 */	
	 
		Dependency(class Field *pField_Dependent,class Field *pField_Master,class Field *pField_Indirect)
		{
			m_pField_Dependent=pField_Dependent;
			m_pFieldinfo_Master=pField_Master;
			m_pField_Indirect=pField_Indirect;
		}
	};

	/**
	 * @brief This is a particular row that depends on another
	 */

	class DependentRow
	{
		class Dependency *m_pDependency;  /**< The general dependency */
		
		class ChangedRow *m_pChangedRow_Master; /**< points to a changed row */
		
		class ChangedRow *m_pChangedRow_Dependent;   /**< points to a changed row */
		
		int m_psc_id_Dependent;
		int m_psc_id_Master;

		/**
		 * @brief constructor
		 */
		 
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

	/**
	 * @brief This is a row that has been changed on the client
	 */
	 
	class ChangedRow
	{
	public:
		int m_psc_id;		/**< the id of the row */
		int m_psc_batch;	
		int m_psc_user;         /**< the user */
		int m_iOriginalAutoIncrID;
		int m_iNewAutoIncrID;
		bool m_bCommitted;     /**< whether or not the change was committed */
		class Table *m_pTable; /**< points to a Table */
		vector<string> m_vectPrimaryKey;
		TypeOfChange m_eTypeOfChange;	/**< the type of the change committed */

		/**
		 * @brief constructor
		 */
		 
		ChangedRow(class Table *pTable, TypeOfChange eTypeOfChange, int psc_id, int psc_batch, int psc_user, int iOriginalAutoIncrID, vector<string> &vectPrimaryKey)
		{
			m_pTable=pTable;
			m_vectPrimaryKey = vectPrimaryKey;
			m_eTypeOfChange = eTypeOfChange;
			m_psc_id=psc_id; m_psc_batch=psc_batch; m_psc_user=psc_user;
			m_iOriginalAutoIncrID=iOriginalAutoIncrID;
			m_iNewAutoIncrID=-1;
			m_bCommitted=false;
		}
		
		/**
		 * @brief This constructor is used ONLY for rows that were deleted on the client side
		 */
		 
		ChangedRow(class Table *pTable, int psc_id)
		{
			m_pTable=pTable;
			m_eTypeOfChange = toc_Delete;
			m_psc_id=psc_id;
			m_bCommitted=false;
		}

		/** 
		 * @brief  brief documentation
		 * @todo complete documentation
		 */
		 
		string GetWhereClause();
	};
	typedef list<ChangedRow *> ListChangedRow;
}

#endif
