#include "PlutoUtils/CommonIncludes.h"	
#include "CommonFunctions.h"
#include <iostream>

namespace sqlCVS
{
	void NewSection()
	{ 
		cout << endl << "==============================================================================" << endl; 
	}
}

string int2string(int i)
{
	char s[1000];
	sprintf(s, "%i", i);
	return s;
}

string getTableFromForeignKey(string field,map<string,class TableInfo_Generator *> *mapTableNames)
{
	// Skip fields that are not foreign keys, and also the _RecordInfo.  Otherwise
	// Users will reference every other table because of the FK_Users_RecordInfo
	// and create a .cpp file that is too big to compile
	if( field.find("FK_")!=0 || field.find("_RecordInfo")!=string::npos )
		return "";

	string table = field.substr(3);


	// See if table is a valid Table Name
	if( mapTableNames->find(table)!=mapTableNames->end() )
	{
		return table;
	}

	// No, see if it has a trailing _
	size_t last_underscore=string::npos,s=-1;  // s is a temporary value, start with -1 since we always search for the last pos +1 
	while( (s=table.find('_',s+1)) != string::npos )
		last_underscore = s;

	if( last_underscore!=string::npos )
	{
		table = table.substr(0,last_underscore);
		if( mapTableNames->find(table)!=mapTableNames->end() )
			return table;
	}

	//	cout << "**ERROR** Cannot determine referenced table in field: " << field << endl;
	return "";
}
