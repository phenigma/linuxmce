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

#include "json_generator.h"
//-------------------------------------------------------------------------------------------------------
json_generator::json_generator(string sDBName)
{
	LoadFilterForColumns(sDBName + ".conf");
	LoadFilterForRows(sDBName + "_filter.conf");
}
//-------------------------------------------------------------------------------------------------------
void json_generator::LoadFilterForColumns(string sConfigFile)
{
	vector<string> vectLines;
	FileUtils::ReadFileIntoVector(sConfigFile, vectLines);

	for(vector<string>::iterator it = vectLines.begin(); it != vectLines.end(); ++it)
	{
		string sLine = *it;

		vector<string> vectObjects;
		StringUtils::Tokenize(sLine, ":", vectObjects);

		if(vectObjects.size() == 2)
			m_mapTableFields[vectObjects[0]] = vectObjects[1];
		else
			cout << "INVALID LINE: " << sLine << endl;
	}
}
//-------------------------------------------------------------------------------------------------------
void json_generator::LoadFilterForRows(string sConfigFile)
{
	vector<string> vectLines;
	FileUtils::ReadFileIntoVector(sConfigFile, vectLines);

	for(vector<string>::iterator it = vectLines.begin(); it != vectLines.end(); ++it)
	{
		string sLine = *it;

		vector<string> vectObjects;
		StringUtils::Tokenize(sLine, ":", vectObjects);

		if(vectObjects.size() == 2)
		{
			m_mapRowFilter["PK_" + vectObjects[0]] = vectObjects[1];
			m_mapRowFilter["FK_" + vectObjects[0]] = vectObjects[1];
		}
		else
			cout << "INVALID LINE: " << sLine << endl;
	}
}
//-------------------------------------------------------------------------------------------------------
json_generator::~json_generator()
{
}
//-------------------------------------------------------------------------------------------------------
string json_generator::generate_table_json(TableInfo_Generator *pTableInfo, bool bHasPKField)
{
	string sTableInfo;
	string sFields = get_fields_needed_for_table(pTableInfo->get_table_name());
	string sIds;
	string sWhereCondition;
	string sAllFields;

	//list with all fields, including the PK_... if it has any
	if(bHasPKField)
		sAllFields = "PK_" + pTableInfo->get_table_name() + ",";
   	sAllFields += sFields;

	//vector with all fields
	vector<string> vectAllFieldNames;
	StringUtils::Tokenize(sAllFields, ",", vectAllFieldNames);

	//generate where condition
	bool bFirstCondition = true;
	for(vector<string>::iterator it = vectAllFieldNames.begin(); it != vectAllFieldNames.end(); ++it)
	{
		string sFieldName = *it;

		map<string,string>::iterator it_item = m_mapRowFilter.find(sFieldName);
		if(it_item != m_mapRowFilter.end())
		{
			if(bFirstCondition)
			{
				bFirstCondition = false;
				sWhereCondition = "WHERE ";
			}
			else
			{
				sWhereCondition += ", ";
			}

			sWhereCondition += sFieldName + " IN (" + it_item->second + ")";
		}
	}

	//get the rows
	string sSQL = "SELECT " + 
		(bHasPKField ? "PK_" + pTableInfo->get_table_name() + "," : "") + 
		sFields + 
		" FROM " + pTableInfo->get_table_name() + " " +
		sWhereCondition + " " +
		(bHasPKField? " ORDER BY PK_" + pTableInfo->get_table_name() : "");

	int iresult = mysql_query(pTableInfo->db(), sSQL.c_str());
	if(iresult != 0)
	{
		cout << "INVALID SQL : " << sSQL << endl;
		return "";
	}

	//"tablename_ids":[1,2,3,...],
	//	"tablename":{
	//		"PK_tablename_1": {
	//			"field1":"value1_1",
	//				"field2":"value1_2",
	//				...
	//		},
	//		"PK_tablename_2": {
	//			"field1":"value2_1",
	//				"field2":"value1_2",
	//				...
	//		},
	//		...
	//	},
	//	...

	//the vector with the other fields
	vector<string> vectFieldNames;
	StringUtils::Tokenize(sFields, ",", vectFieldNames);

	MYSQL_RES *res = mysql_store_result(pTableInfo->db());
	MYSQL_ROW row;

	bool bFirstRow = true;
	map<string,int> mapDefines; // Be sure there's no dups
	while ((row = mysql_fetch_row(res)))
	{
		if(bHasPKField && NULL == row[0])
			continue; // There must be either a description or define
	
		if(!bHasPKField && (NULL == row[0] || NULL == row[1]))
			continue;

		string sRowInfo = 
			bHasPKField ? 
			("\t\t\"PK_" + pTableInfo->get_table_name() + "_" + row[0] + "\": {\n") :
			("\t\t\"" + pTableInfo->get_table_name() + "_" + row[0] + "_" + row[1] + "\": {\n");

		size_t nNumFields = vectFieldNames.size();
		for(size_t i = 0; i < nNumFields; ++i)
		{
			sRowInfo += "\t\t\t\"" + vectFieldNames[i] + "\":\"" + (NULL != row[i + 1] ? JSONEscape(row[i + 1]) : "") + "\"";

			if(i != nNumFields - 1)
				sRowInfo += ",";

			sRowInfo += "\n";
		}

		sRowInfo += "\t\t}";

		if(bFirstRow)
		{
			bFirstRow = false;
		}
		else
		{
			sTableInfo += ",\n";
			sIds += ",";
		}

		sIds += row[0];
		sTableInfo += sRowInfo;
	}

	sIds = "\t\"" + pTableInfo->get_table_name() + "_ids\":[" + sIds + "],\n";
	sTableInfo = 
		sIds + 
		"\t\"" + pTableInfo->get_table_name() + "\":{\n" +
		sTableInfo + "\n" +
		"\t}\n";

	return sTableInfo;
}
//-------------------------------------------------------------------------------------------------------
string json_generator::get_fields_needed_for_table(string sTableName)
{
	map<string, string>::iterator it = m_mapTableFields.find(sTableName);

	if(it != m_mapTableFields.end())
		return it->second;

	return "";
}
//-------------------------------------------------------------------------------------------------------
bool json_generator::is_needed_table(string sTableName)
{
	return m_mapTableFields.find(sTableName) != m_mapTableFields.end();
}
//-------------------------------------------------------------------------------------------------------
const string& json_generator::generate_json(vector<class TableInfo_Generator *> *p_listTableInfo)
{
	vector<class TableInfo_Generator *>::iterator it;

	bool bFirstTable = true;
	m_sBuffer = "{\n";

	for(it = p_listTableInfo->begin(); it < p_listTableInfo->end(); ++it) // processing tables
	{
		TableInfo_Generator *pTableInfo = (*it);

		if(is_needed_table(pTableInfo->get_table_name()))
		{
			cout << "Table : " << pTableInfo->get_table_name() << ": " << "processing......................." << endl;

			bool bHasPKField = pTableInfo->HasField("PK_" + pTableInfo->get_table_name());
			string sTableInfo = generate_table_json(pTableInfo, bHasPKField);

			if(bFirstTable)
				bFirstTable = false;
			else
				m_sBuffer += "\t,\n";

			m_sBuffer += sTableInfo;
		}
		else
		{
			cout << "Table : " << pTableInfo->get_table_name() << ": " << "skipping..." << endl;
		}
	}

	m_sBuffer += "}\n";

	return m_sBuffer;
}
//-------------------------------------------------------------------------------------------------------
string json_generator::JSONEscape(string sValue)
{
	StringUtils::Replace(&sValue, "\"", "\\\"");
	StringUtils::Replace(&sValue, "\\", "\\\\");
	StringUtils::Replace(&sValue, "/", "\\/");
	StringUtils::Replace(&sValue, "\n", "\\n");
	StringUtils::Replace(&sValue, "\r", "\\r");
	StringUtils::Replace(&sValue, "\t", "\\t");

	return sValue;
}
//-------------------------------------------------------------------------------------------------------
