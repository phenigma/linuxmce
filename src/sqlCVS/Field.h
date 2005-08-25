/**
 *
 * @file Field.h
 * @brief header file for the Field class
 * @author
 *
 */
 
#ifndef Field_h
#define Field_h

#include "mysql.h"
#include "sql2cpp/FieldInfo.h"
#include "ChangedRow.h"

#include <iostream>

namespace sqlCVS
{
	class Field;
	typedef map<string, Field *> MapField;
	typedef list<Field *> ListField;

	/**
	 * @brief  class modelling the fields in the database
	 */
	 
	class Field : public ::FieldInfo
	{
	public:
		class Table *m_pTable; /**< points to the table the field belongs to 
					* @todo ask
					*/

		Field *m_pField_IReferTo_Directly;
		list< pair<string,Field *> > m_listField_IReferTo_Indirectly;  // string is the description used in the reference

		ListField m_listField_ReferringToMe_Directly;
		list< pair<string,Field *> > m_listField_ReferringToMe_Indirectly;

		Field *m_pField_IndirectSpecifier;

		bool m_bIsAutoIncrement;

		/**
		 * @brief constructor
		 */
		Field( class Table *pTable, MYSQL_FIELD *pMYSQL_FIELD );

		~Field()
		{
		}

		/**
		 * @brief Accessors 
		 */
		 
		string Name_get( ) { return m_pcFieldName; }
		class Table *Table_get( ) { return m_pTable; }
	};
}

#endif
