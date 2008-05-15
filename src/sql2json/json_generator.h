/*
json_generator
Copyright (C) 2008 Pluto, Inc., a Florida Corporation

www.plutohome.com		

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

See the GNU General Public License for more details.
*/

/**
*
* @file json_generator.cpp
* @brief generates a json file with the needed tables
* @author Cristian Miron
*/

#ifndef __JSON_GENERATOR_H__
#define __JSON_GENERATOR_H__
//-------------------------------------------------------------------------------------------------------
#include "PlutoUtils/CommonIncludes.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>

#include <string>
#include <vector>
#include <map>
#include <list>

#include <mysql.h>

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "DatabaseInfo.h"
#include "CommonFunctions.h"
#include "DCE/Logger.h"
//-------------------------------------------------------------------------------------------------------
class json_generator
{
private:

	string m_sBuffer;
	map<string, string> m_mapTableFields;
	map<string, string> m_mapRowFilter;

	string generate_table_json(TableInfo_Generator *pTableInfo, bool bHasPKField);
	string get_fields_needed_for_table(string sTableName);
	bool is_needed_table(string sTableName);

	void LoadFilterForColumns(string sConfigFile);
	void LoadFilterForRows(string sConfigFile);

public:
	json_generator(string sDBName);
	~json_generator();

	const string& generate_json(vector<class TableInfo_Generator *> *p_listTableInfo);
};
//-------------------------------------------------------------------------------------------------------
#endif // __JSON_GENERATOR_H__