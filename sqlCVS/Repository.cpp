#include "PlutoUtils/CommonIncludes.h"	

#include "CommonFunctions.h"
#include <iostream>
#include <sstream>

#include "Database.h"
#include "Repository.h"
#include "Table.h"

#include "RA/RA_Processor.h"
#include "R_CommitChanges.h"
#include "ChangedRow.h"

using namespace sqlCVS;

void Repository::AddDefinitionTable(class Table *pTable)
{
	if( pTable->GetTrailingString()=="repset" )
		m_pTable_Setting = pTable;
	else if( pTable->GetTrailingString()=="bathdr" )
		m_pTable_BatchHeader = pTable;
	else if( pTable->GetTrailingString()=="batdet" )
		m_pTable_BatchDetail = pTable;
	else if( pTable->GetTrailingString()=="tables" )
		m_pTable_Tables = pTable;
	else
	{
		cerr << "Cannot figure out what table: " << pTable->Name_get() << " is.";
		throw("Schema problem");
	}

	// We found a match, and this is a system table
	pTable->SetRepository(this,true);
}

void Repository::MatchUpTables()
{
	if( m_pTable_Setting==NULL || m_pTable_BatchHeader==NULL || m_pTable_BatchDetail==NULL || m_pTable_Tables==NULL )
	{
		cerr << "Could not find all the system tables for Repository: " << m_sName << endl
			<< "I need _repset, _bathdr, _batdet, _tables" << endl
			<< "This repository has been corrupted, and will need to be rebuilt in order to continue." << endl;
		if( AskQuestion("Rebuild repository: " + m_sName,false) )
		{
			MapTable mapTable; // Don't add any tables, just setup an empty repository
			Setup();
		}
		else
			throw "Repository corrupt";
	}

	string Tablename = "psc_" + m_sName + "_tables";

	ostringstream sql;
	sql << "SELECT Tablename FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( (result_set.r=m_pDatabase->mysql_query_result(sql.str())) )
	{
		while( (row = mysql_fetch_row(result_set.r)) )
		{
			Table *pTable = m_pDatabase->m_mapTable_Find(row[0]);
			if( !pTable )
			{
				cerr << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it no longer exists in the database." << endl
					<< "I must delete it from the repository's database to continue." << endl;

				if( !AskQuestion("Delete the bad data, and cleanup the repository table?",false) )
					throw "Repository corrupt";

				sql.str("");
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
		        m_pDatabase->mysql_query(sql.str());
				continue; // Nothing more to do
			}
			if( pTable->Repository_get() && pTable->Repository_get()!=this )
			{
				cerr << "Found an entry in " << Tablename << " for the table: " << row[0] << endl
					<< "But it already exists in the repository: " << pTable->Repository_get()->Name_get() << endl
					<< "I must delete it from this repository's database to continue." << endl;

				if( !AskQuestion("Delete the bad data, and cleanup the repository table?",false) )
					throw "Repository corrupt";

				sql.str("");
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
		        m_pDatabase->mysql_query(sql.str());
				continue; // Nothing more to do
			}
			// Go ahead and add the table
			m_mapTable[ pTable->Name_get() ] = pTable;
			pTable->SetRepository(this,false);
		}
	}
}

void Repository::Setup()
{
	if( !m_pTable_Setting )
		m_pTable_Setting = CreateSettingTable();
	if( !m_pTable_BatchHeader )
		m_pTable_BatchHeader = CreateBatchHeaderTable();
	if( !m_pTable_BatchDetail )
		m_pTable_BatchDetail = CreateBatchDetailTable();
	if( !m_pTable_Tables )
		m_pTable_Tables = CreateTablesTable();

	string Tablename = "psc_" + m_sName + "_tables"; // Our _tables table

	// Go through all the tables in the map and be sure we have them in our tracking table, and also in our internal map
	for(MapTable::iterator it=g_GlobalConfig.m_mapTable.begin();it!=g_GlobalConfig.m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		pTable->SetRepository(this,false);
		pTable->TrackChanges_set(true);
		m_mapTable[ pTable->Name_get() ] = pTable;
		ostringstream sql;

		sql << "SELECT PK_" << Tablename << " FROM `" << Tablename << "` WHERE Tablename='" << pTable->Name_get() << "'";
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		if( !(result_set.r=m_pDatabase->mysql_query_result(sql.str())) || !(row = mysql_fetch_row(result_set.r)) )
		{
			sql.str("");
			sql << "INSERT INTO `" << Tablename << "` (Tablename) VALUES('" << pTable->Name_get() << "')";
			if( !m_pDatabase->mysql_query(sql.str()) )
				throw "Database Error";
		}
	}

	// If we have any tables in our existing internal map that aren't in this new list, remove them.  They're not part of the repository any more
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();)
	{
		if( g_GlobalConfig.m_mapTable.find( (*it).first ) == g_GlobalConfig.m_mapTable.end() )
		{
			(*it).second->TrackChanges_set(false);
            m_mapTable.erase(it++); // This isn't in the repository any more
		}
		else
			it++;
	}

	// If we have any tables in the _tables table that aren't in this new list, remove them.  They're not part of the repository any more
	ostringstream sql;
	sql << "SELECT Tablename FROM `" << Tablename << "`";
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( (result_set.r=m_pDatabase->mysql_query_result(sql.str())) )
	{
		while( (row = mysql_fetch_row(result_set.r)) )
		{
			if( g_GlobalConfig.m_mapTable.find( row[0] ) == g_GlobalConfig.m_mapTable.end() )
			{
				sql.str("");
				sql << "DELETE FROM `" << Tablename << "` WHERE Tablename='" << row[0] << "'";
		        m_pDatabase->mysql_query(sql.str());
			}
		}
	}
}

Table *Repository::CreateSettingTable()
{
	string Tablename = "psc_" + m_sName + "_repset";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	sql.str("");
	sql	<< "CREATE TABLE `" << Tablename << "`(" << endl
		<< "`PK_" << Tablename << "` int(11) NOT NULL auto_increment," << endl
		<< "`Value` varchar(30) NOT NULL default ''," << endl
		<< "PRIMARY KEY  (`PK_" << Tablename << "`)" << endl
		<< ") TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	return new Table(m_pDatabase,Tablename);
}

class Table *Repository::CreateBatchHeaderTable()
{
	string Tablename = "psc_" + m_sName + "_bathdr";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	sql.str("");
	sql	<< "CREATE TABLE `" << Tablename << "`(" << endl
		<< "`PK_" << Tablename << "` int(11) NOT NULL auto_increment," << endl
		<< "`Value` varchar(30) NOT NULL default ''," << endl
		<< "PRIMARY KEY  (`PK_" << Tablename << "`)" << endl
		<< ") TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	return new Table(m_pDatabase,Tablename);
}

class Table *Repository::CreateBatchDetailTable()
{
	string Tablename = "psc_" + m_sName + "_batdet";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	sql.str("");
	sql	<< "CREATE TABLE `" << Tablename << "`(" << endl
		<< "`PK_" << Tablename << "` int(11) NOT NULL auto_increment," << endl
		<< "`Value` varchar(30) NOT NULL default ''," << endl
		<< "PRIMARY KEY  (`PK_" << Tablename << "`)" << endl
		<< ") TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	return new Table(m_pDatabase,Tablename);
}

class Table *Repository::CreateTablesTable()
{
	string Tablename = "psc_" + m_sName + "_tables";
	ostringstream sql;

	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	sql.str("");
	sql	<< "CREATE TABLE `" << Tablename << "`(" << endl
		<< "`PK_" << Tablename << "` int(11) NOT NULL auto_increment," << endl
		<< "`Tablename` varchar(30) NOT NULL default ''," << endl
		<< "PRIMARY KEY  (`PK_" << Tablename << "`)" << endl
		<< ") TYPE=" << g_GlobalConfig.m_sTableType << ";" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	return new Table(m_pDatabase,Tablename);
}

void Repository::Remove()
{
	ostringstream sql;
	string Tablename = "psc_" + m_sName + "_repset";
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	Tablename = "psc_" + m_sName + "_bathdr";
	sql.str("");
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	Tablename = "psc_" + m_sName + "_batdet";
	sql.str("");
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	Tablename = "psc_" + m_sName + "_tables";
	sql.str("");
	sql << "DROP TABLE IF EXISTS `" << Tablename << "`;" << endl;
	if( !m_pDatabase->mysql_query(sql.str()) )
		throw "Database error";

	m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchDetail=m_pTable_Tables=NULL;
}

#pragma warning("as you add a table to a repository, automatically add all related tables which depend on it, or it depends on them--no, what about constants that are shared???--put warnings next to them")

bool Repository::CheckIn()
{

int ClientID=1,SoftwareVersion=1; // HACK!!!
	RA_Processor ra_Processor(ClientID,SoftwareVersion);

	R_CommitChanges r_CommitChanges;
	ra_Processor.AddRequest(&r_CommitChanges);
	DCE::Socket *pSocket=NULL;
	while( ra_Processor.SendRequests("localhost:3485",&pSocket) );

	if( r_CommitChanges.m_cProcessOutcome!=SUCCESSFULLY_PROCESSED )
	{
		cerr << "Unable to commit repository to server" << endl;
		delete pSocket;
		return false;
	}

	for(MapTable::iterator it=g_GlobalConfig.m_mapTable.begin();it!=g_GlobalConfig.m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Repository_get()==this )
			pTable->CheckIn(ra_Processor,pSocket,toc_New);
	}
	return true;
}

void Repository::AddTablesToMap()
{
	// Be sure all tables in this repository are in the map we got
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
		g_GlobalConfig.m_mapTable[ (*it).first ] = (*it).second;
}
