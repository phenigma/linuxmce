#include "PlutoUtils/CommonIncludes.h"	
#include "TableInfo.h"

bool TableInfo::loadFromDB(string table)
{
	table_name = table;
	
	MYSQL_RES *res = mysql_list_fields(db, table_name.c_str(), NULL);
	if (res != NULL)
	{		
		int fields_count = mysql_num_fields(res);
			
		MYSQL_FIELD *mysql_fields = mysql_fetch_fields(res);
			
		for (int i=0; i<fields_count; i++)
			fields.push_back(new Field(mysql_fields+i));
		
		mysql_free_result(res);
		
		return true;
	}
	else
		return false;
	
}

bool TableInfo::HasPrimaryKeys()
{
	for (vector<Field*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			return true;
	
	return false;
}

string TableInfo::get_table_name()
{
	return table_name;
}
