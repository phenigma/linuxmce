#include "PlutoUtils/CommonIncludes.h"	
#include "ChangedRow.h"	
#include "Table.h"
#include "Field.h"
#include <sstream>

using namespace sqlCVS;

string ChangedRow::GetWhereClause()
{
	std::ostringstream sWhere;
	// If it's not a new row, it will have m_psc_id assigned
	if( m_psc_id )
		sWhere << " WHERE psc_id=" << m_psc_id;
	else
	{
		long Count=0;
		for(ListField::iterator it=m_pTable->m_listField_PrimaryKey.begin();it!=m_pTable->m_listField_PrimaryKey.end();++it)
		{
			Field *pField = *it;
			if( Count )
				sWhere << " AND ";
			else
				sWhere << " WHERE ";

			sWhere << " `" << pField->Name_get() << "`=" << m_vectPrimaryKey[Count++];
		}
	}
	return sWhere.str();
}