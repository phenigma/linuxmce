/*
	CommonFunctions
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/


/**
 *
 * @file CommonFunctions.cpp
 * @brief source file containing some common functions
 *
 */


#include "PlutoUtils/CommonIncludes.h"
#include "CommonFunctions.h"
#include <iostream>
	
string int2string(int i)
{
	char s[1000];
	sprintf(s, "%i", i);
	return s;
}

string getTableFromForeignKey(string sField,map<string,class TableInfo_Generator *> *mapTableNames)
{
	// Skip fields that are not foreign keys, and also the _RecordInfo.  Otherwise
	// Users will reference every other table because of the FK_Users_RecordInfo
	// and create a .cpp file that is too big to compile
	if( sField.find("FK_") != 0 || sField.find("_RecordInfo") != string::npos )
		return "";

	string table = sField.substr( 3 );

	
	// See if table is a valid Table Name
	if( mapTableNames->find( table )!= mapTableNames->end() )
	{
		return table;
	}

	// No, see if it has a trailing _
	size_t last_underscore=string::npos, s=-1;  // s is a temporary value, start with -1 since we always search for the last pos +1 
	while( ( s = table.find('_',s+1)) != string::npos )
		last_underscore = s;

	if( last_underscore != string::npos )
	{
		table = table.substr( 0, last_underscore );
		if( mapTableNames->find( table ) != mapTableNames->end() )
			return table;
	}

	return "";
}

