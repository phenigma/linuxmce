/**
 *
 * @file Field.cpp
 * @brief source file for the Field class
 *
 */
 
 /**
  *
  * Copyright information goes here
  *
  */

#include "PlutoUtils/CommonIncludes.h"	
#include "Field.h"

#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

#include "Database.h"

using namespace sqlCVS;

Field::Field(class Table *pTable, MYSQL_FIELD *pMYSQL_FIELD)
	::FieldInfo(pMYSQL_FIELD)
{
	m_pTable=pTable;

	m_bIsAutoIncrement = (m_iFlags & PRI_KEY_FLAG) && (m_iFlags & AUTO_INCREMENT_FLAG);

	m_sName = name;

	m_pField_IReferTo_Directly=m_pField_IndirectSpecifier=NULL;
}


