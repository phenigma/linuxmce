#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/StringUtils.h"
#include "DatabaseInfo.h"

using namespace StringUtils;

void DatabaseInfo::getDatabaseTablesList()
{
	MYSQL_RES *res = mysql_list_tables(db, NULL);
	
	if (res != NULL)
	{
		MYSQL_ROW row;
		
		while ((row = mysql_fetch_row(res)))
		{
			string sTable = row[0];
			if( StringUtils::EndsWith(sTable,"_pschist") || StringUtils::StartsWith(sTable,"psc_") )
				continue;

			// Each table needs a list of all the other tables so that it can find foreign keys pointing
			// to itself
			TABLEINFO_TYPE *info = new TABLEINFO_TYPE(db, &map_tables_info);

			if (info->loadFromDB(row[0]))
			{
				m_listTableInfo.push_back(info);
				map_tables_info[info->get_table_name()]=info;
			}
			else
			{
				delete info;			
				cout << "Cannot load info for table: " << row[0] << endl;
			}
		}

		//tables.push_back(row[0]);
	}

	mysql_free_result(res);
}

void DatabaseInfo::ConvertTablesToStrings(vector<string> *vect_string)
{
	vector<class TABLEINFO_TYPE *>::iterator it;

	for(it=m_listTableInfo.begin();it!=m_listTableInfo.end();++it)
	{
		TableInfo *pTableInfo = *it;
		vect_string->push_back(pTableInfo->get_table_name());
	}
}

