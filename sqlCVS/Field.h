#ifndef Field_h
#define Field_h

#include "mysql.h"
#include "sql2cpp/FieldInfo.h"
#include "ChangedRow.h"

#include <iostream>

namespace sqlCVS
{
	class Field;
	typedef map<string,Field *> MapField;
	typedef list<Field *> ListField;

	class Field : public ::FieldInfo
	{
		friend class Table;
		class Table *m_pTable;

		Field *m_pField_IReferTo_Directly;
		ListField m_listField_IReferTo_Indirectly;

		ListField m_listField_ReferringToMe_Directly;
		ListField m_listField_ReferringToMe_Indirectly;

		Field *m_pField_IndirectSpecifier;

		string m_sName;

		bool m_bIsAutoIncrement;

	public:
		Field(class Table *pTable, MYSQL_FIELD *pMYSQL_FIELD);

		// Accessors
		string Name_get() { return m_sName; }
		class Table *Table_get() { return m_pTable; }
	};
}

#endif
