#include "PlutoUtils/CommonIncludes.h"	
#include "Database.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "CommonFunctions.h"
#include <iomanip>

using namespace sqlCVS;

Database::Database(string db_host, string db_user, string db_pass, string db_name, int db_port)
{
	// Establishing database connection			  	
	m_pMYSQL = mysql_init(NULL);
	m_bInteractiveMode=false;

	if (mysql_real_connect(m_pMYSQL, db_host.c_str(), db_user.c_str(), db_pass.c_str(), db_name.c_str(), db_port, NULL, 0) == NULL)
	{
		cerr << "MySQL connection failed" << endl;
		throw string("MySQL connect failed");
	}

	// And build our list of tables and repositories
	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	if( res.r=mysql_list_tables(m_pMYSQL, NULL) )
	{
		while ((row = mysql_fetch_row(res.r)))
		{
			Table *pTable = new Table(this,row[0]);
			m_mapTable[row[0]] = pTable;
			// Does this table help define a repository (ie table is psc_repositoryname_[x]
			string sRepository = pTable->DefinesRepository();
			if( sRepository.size() )
			{
				Repository *pRepository = m_mapRepository[sRepository];
				if( !pRepository )
				{
					pRepository = new Repository(this,sRepository);
					m_mapRepository[sRepository] = pRepository;
				}
				pRepository->AddDefinitionTable(pTable);
			}
		}
	}

	for(MapRepository::iterator it=m_mapRepository.begin();it!=m_mapRepository.end();++it)
		(*it).second->MatchUpTables();
	for(MapTable::iterator itT=m_mapTable.begin();itT!=m_mapTable.end();++itT)
	{
		(*itT).second->MatchUpHistory();
		(*itT).second->GetDependencies();
	}

}

void Database::GetRepositoriesTables()
{
	string s,sUnknownRepository;
	string::size_type pos=0;

	while( (s=StringUtils::Tokenize(g_GlobalConfig.m_sRepository,",",pos)).length() )
	{
		Repository *pRepository = m_mapRepository_Find(s);
		if( !pRepository )
			sUnknownRepository += (sUnknownRepository.length() ? "," : "") + s;
		else
			g_GlobalConfig.m_mapRepository[s]=pRepository;
	}
	g_GlobalConfig.m_sRepository=sUnknownRepository;

	pos=0;
	while( (s=StringUtils::Tokenize(g_GlobalConfig.m_sTable,",",pos)).length() )
	{
		Table *pTable = m_mapTable[s];
		if( !pTable )
		{
			cerr << "Unknown Table: " << s << endl;
			throw "Unknown Table";
		}
		g_GlobalConfig.m_mapTable[s]=pTable;
	}
}

void Database::CreateRepository()
{
	vector<string> vect_sRepository;
	StringUtils::Tokenize(g_GlobalConfig.m_sRepository,",",vect_sRepository);

	if( vect_sRepository.size()>1 && g_GlobalConfig.m_mapTable.size()>0 )
	{
		cerr << "You cannot specify more than 1 repository to create at a time if you explicitly list tables" << endl;
		throw "Bad arguments";
	}

	if( vect_sRepository.size()==0 )
	{
		while( g_GlobalConfig.m_sRepository.length()==0 )
		{
			cout << "What do you want to call this repository? (enter q to quit): ";
			cin >> g_GlobalConfig.m_sRepository;
			if( g_GlobalConfig.m_sRepository=="q" )
				return;  // Do nothing
			vect_sRepository.push_back(g_GlobalConfig.m_sRepository);
		}
	}

	// We must have at least 1 repository specified, maybe more if the user passed them in the command line
	for(size_t s=0;s<vect_sRepository.size();++s)
	{
		Repository *pRepository = m_mapRepository_Find(vect_sRepository[s]);
		if( pRepository )
		{
			if( !AskQuestion("That repository exists.  Edit the exiting one?",false ) )
				return;  // Nope, just abort
		}
		else
			pRepository = new Repository(this,vect_sRepository[s]);

		int Result;
		if( g_GlobalConfig.m_mapTable.size()==0 && (Result=PromptForTablesInRepository(pRepository,g_GlobalConfig.m_mapTable))<1 )  // If the user didn't specify tables on the command line, prompt for them
		{
			if( Result<0 )
				return; // Just return without doing anything
			pRepository->Remove();  //Remove this repository if there are no tables
			MapRepository::iterator it = m_mapRepository.find( pRepository->Name_get() );
			if( it!=m_mapRepository.end() )  // It could be that, even though the user chose create, he typed in the name of an existing repository
				m_mapRepository.erase(it);
			delete pRepository;

			return; // PromptForTablesInRepository returns the number of tables the user selected, or -1 to exit
		}

		pRepository->Setup();
		m_mapRepository[ pRepository->Name_get() ] = pRepository;
	}
}



int Database::PromptForTablesInRepository(Repository *pRepository,MapTable &mapTable)
{
	m_bInteractiveMode=true;

	int iNumColumns = SCREEN_WIDTH / ((int)MaxTablenameLength()+8);
	int iColumnWidth = SCREEN_WIDTH/iNumColumns;

	// Since the user may have called this with an existing repository, do a quick run 
	// first to be sure we add any tables that are already in the repository to the map
	// The user can always remove them in the main loop
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		Table *pTable = (*it).second;
		if( pTable->Name_get().substr(0,4)=="psc_" )
			continue; // It's a system table

		// If the table is not in any repository, that's okay -- the user will be adding it.  But if it already is in a repository,
		// it must point to this one if it's already in the list
		if( pTable->Repository_get() )
		{
			// If it's in the list, and points to us, that's ok.  If it's not in the list and points to another, that's also ok.

			// But if it's in the list and points to another, that's an error
			if( mapTable.find(pTable->Name_get())!=mapTable.end() && pTable->Repository_get()!=pRepository )
			{
				// We would never allow this to happen, but maybe the user specified everything in the command line and 
				// is trying to put a table into a repository that is already in another repository
				cerr << "Table: " << pTable->Name_get() << " is already in the repository " << pTable->Repository_get()->Name_get() << endl;
				cerr << "It cannot be added to repository: " << pRepository->Name_get() << endl;
				throw "Bad arguments";
			}
			// and if it's not in the list, but points to us, we need to add it, since, by default, it will be selected.  The user can remove it
			else if( mapTable.find(pTable->Name_get())==mapTable.end() && pTable->Repository_get()==pRepository )
				mapTable[ pTable->Name_get() ] = pTable;
		}
	}

	string sError="";
	while(true)
	{
		NewSection();
		int iColumnNum=0;
		vector<Table *> vectTable;  // A numeric reference to the tables the user can change
		for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
		{
			Table *pTable = (*it).second;
			if( pTable->Name_get().substr(0,4)=="psc_" )
				continue; // It's a system table

			bool bTableAlreadyInList = mapTable.find(pTable->Name_get())!=mapTable.end();

			// 001 *H TableName
			if( pTable->Repository_get() && pTable->Repository_get()!=pRepository )
				cout << "XXX";  // It's in another repository, we can't touch it
			else
			{
				vectTable.push_back(pTable);
				cout << setw(3) << (int) vectTable.size();
			}

			cout << (bTableAlreadyInList ? " *" : "  ") << (pTable->HasFullHistory_get() ? "H " : "  ");
			cout << pTable->Name_get();
			if( ++iColumnNum >= iNumColumns )
			{
				iColumnNum=0;
				cout << endl;
			}
			else 
				cout << StringUtils::RepeatChar(' ',iColumnWidth - (int)pTable->Name_get().length());
		}

		cout << "Tables marked with * are to be included in the repository: " << pRepository->Name_get() << endl
			<< "Enter the table numbers, separated by commas, to toggle the * flag." << endl
			<< "XXX means the table is already in another repository, and cannot be added" << endl
			<< "Tables with an H in front will have history tracking." << endl
			<< "Enter H followed by the table numbers to toggle the history flag." << endl
			<< "Enter D when you have finished, Q to quit." << endl;
		if( sError.length() )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return (int) mapTable.size();
		if( sAction=="Q" || sAction=="q" )
			return -1;  // means abort

		bool bHistory = sAction[0]=='h' || sAction[0]=='H';
		string::size_type pos = bHistory ? 1 : 0;
		string sNumber;
		while( (sNumber=StringUtils::Tokenize(sAction,",",pos)).length() )
		{
			int iNumber=atoi(sNumber.c_str())-1;  // Our vector is 0 based, subtract 1
			if( iNumber<0 || iNumber>(int) vectTable.size()-1 )
			{
				sError ="Invalid table number";
				break;
			}
			Table *pTable = vectTable[iNumber];
			if( bHistory )
				pTable->HasFullHistory_set( !pTable->HasFullHistory_get() );
			else if( mapTable.find(pTable->Name_get())!=mapTable.end() )
				mapTable.erase( pTable->Name_get() );
			else
				mapTable[pTable->Name_get()] = pTable;
		}
	}
	return (int) mapTable.size();
}

size_t Database::MaxTablenameLength()
{
	size_t MaxLength=0;
	for(MapTable::iterator it=m_mapTable.begin();it!=m_mapTable.end();++it)
	{
		if( (*it).first.length()>MaxLength )
			MaxLength = (*it).first.length();
	}

	return MaxLength;
}

bool Database::mysql_query(string sql,bool bIgnoreErrors)
{
	int iresult;
	if( (iresult=::mysql_query(m_pMYSQL,sql.c_str()))!=0 )
	{
		if( bIgnoreErrors )
			return true;
		cerr << "Query failed " << mysql_error(m_pMYSQL) << " with result " << iresult << endl;
		cerr << "SQL: " << sql << endl;
		return false;
	}
	return true;
}


MYSQL_RES *Database::mysql_query_result(string query)
{
	if( !mysql_query(query) )
		throw "Database error";

	return mysql_store_result(m_pMYSQL);
}

int Database::mysql_query_withID(string query)
{
	if( !mysql_query(query) )
		throw "Database error";

	return (int) mysql_insert_id(m_pMYSQL);
}

class Table *Database::GetTableFromForeignKey(Field *pField)
{
	string sTable = pField->Name_get().substr(3);

	while(sTable.length()>0)
	{
		// See if table is a valid Table Name
		Table *pTable = m_mapTable_Find(sTable);
		if( pTable )
			return pTable;

		string Tail = pField->Table_get()->GetTrailingString( sTable );  // Use the helper function in table
		sTable = sTable.substr( 0, sTable.length() - Tail.length() - 1 );  // Strip off the last part
	}

	cerr << "I cannot find the table referred to in the field: " << pField->Table_get()->Name_get() << ":" << pField->Name_get() << endl;
	throw "Bad Schema";
}

class Table *Database::GetTableFromForeignKeyString(Table *pTable,string sField)
{
	string sTable = sField.substr(3);

	while(sTable.length()>0)
	{
		// See if table is a valid Table Name
		Table *pTable = m_mapTable_Find(sTable);
		if( pTable )
			return pTable;

		string Tail = pTable->GetTrailingString( sTable );  // Use the helper function in table
		sTable = sTable.substr( 0, sTable.length() - Tail.length() - 1 );  // Strip off the last part
	}

	cerr << "I cannot find the table referred to in the table: " << pTable->Name_get() << " by " << sField << endl;
	throw "Bad Schema";
}

void Database::CheckIn()
{
	GetTablesToCheckIn();
	
	if( g_GlobalConfig.m_mapTable.size()==0 )
	{
		cerr << "No tables found to add.  Aborting." << endl;
		return;
	}

	SafetyDBLock sl(this);  // The DB will unlock when this falls out of scope

	// Make a copy because GetChanges will add tables that refer to it, but were not in the map already.
	// We can't modify the map while we're iterating it
	MapTable mapTable_Copy = g_GlobalConfig.m_mapTable; 

	for(MapTable::iterator it=mapTable_Copy.begin();it!=mapTable_Copy.end();++it)
		(*it).second->GetChanges();

	if( mapTable_Copy.size() != g_GlobalConfig.m_mapTable.size() && !AskQuestion("Tables were added to the selection to meet dependency requirements.  Continue?",false) )
		return;

	map<int,MapTable *> mapSelectedUsers;

#pragma warning("Allow users to be on the command line so this can be non-interactive")
	// Now mapTable has all the tables we need to check in
	if( ConfirmUsersToCheckIn(mapSelectedUsers)<1 || g_GlobalConfig.m_mapUsers.size()==0 )
		return;

	// If any of the records to be checked in refer to a record with an auto-increement field that is new and is not being checked in, this will fail
	if( !ConfirmRecordsToCheckIn() )
		return;

	SafetyTransaction st(this);  // An exception will be thrown and a roll back called if this falls out of scope and hasn't been committed or rolled back

	for(MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin();it!=g_GlobalConfig.m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		if( !pRepository->CheckIn() )
			return;  // This will throw an exception with the transaction
	}
}

void Database::GetTablesToCheckIn()
{
	// If the user passed specific tables on the command line, that's okay.  Otherwise we need to add the tables from the repository
	
	int flag = 0;
	MapTable mapTableToCheckIn;
	MapTable::iterator itToChkIn = g_GlobalConfig.m_mapTable.begin();

	if( g_GlobalConfig.m_mapTable.size()==0 )
	{
		if( g_GlobalConfig.m_mapRepository.size()==0 )
			if( PromptForRepositories()<1 )
				return;

		// Since the user didn't specify tables explicitly, but rather repositories, just add all tables in the repository
		for(MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin();it!=g_GlobalConfig.m_mapRepository.end();++it)
		{
			Repository *pRepository = (*it).second;
			pRepository->AddTablesToMap();  // Be sure all the tables in the repositories are added
		}
	}

	return;
// following is Cristelu's code -- not used I guess	
	int iMaxLenofTableName = 0;

	PlutoSqlResult result_set, result_set2;
	MYSQL_ROW row;
	string query_string;

	for(MapRepository::iterator it=g_GlobalConfig.m_mapRepository.begin();it!=g_GlobalConfig.m_mapRepository.end();++it)
	{
		Repository *pRepository = (*it).second;
		
		query_string = "SELECT Tablename FROM psc_" + pRepository->Name_get() + "_tables ";
		result_set.r = mysql_query_result(query_string);
        
		while (row = mysql_fetch_row(result_set.r)){
			string sTable = row[0];	
			
			Table *pTable;
			
			if (pTable = m_mapTable_Find(sTable))			
				g_GlobalConfig.m_mapTable[ sTable ] = pTable;
			
			if (iMaxLenofTableName < (int) pTable->Name_get().length())
				iMaxLenofTableName = (int) pTable->Name_get().length();
		}
	}

	if ( g_GlobalConfig.m_mapTable.size() != 0 )
		while (true)
		{
			NewSection();
			
			vector<Table *> vectTable;
			int i;
			
			for (i = 0; i< iMaxLenofTableName; i++)
                cout << " " ; 
			cout << "\t\t\tNew\tMod\tDel" << endl;
		
			PlutoSqlResult result_set;
			MYSQL_ROW row;
			string query_string;
			i = 1;

			for (MapTable::iterator it=g_GlobalConfig.m_mapTable.begin(); it!=g_GlobalConfig.m_mapTable.end(); ++it)
			{
				Table *pTable = (*it).second;
				vectTable.push_back(pTable);
				
				cout << "   " << i++ << "\t" << pTable->Name_get() << "\t";

				for (int j = (int) pTable->Name_get().length();j < iMaxLenofTableName; j++)
					cout << " ";

				cout << "\t";
				
				//number of new rows
				query_string = "SELECT COUNT(*) FROM " + pTable->Name_get() + " WHERE psc_new>0";
				result_set.r = mysql_query_result(query_string);
				row = mysql_fetch_row(result_set.r);
				cout << row[0] << "\t"; 

				//number of deleted rows
				query_string = "SELECT COUNT(*) FROM " + pTable->Name_get() + " WHERE psc_del > 0";
				result_set.r = mysql_query_result(query_string);
				row = mysql_fetch_row(result_set.r);
				cout << row[0] << "\t"; 

				//number of modified rows
				query_string = "SELECT COUNT(*) FROM " + pTable->Name_get() + " WHERE psc_mod > 0";
				result_set.r = mysql_query_result(query_string);
				row = mysql_fetch_row(result_set.r);
				cout << row[0] << "\t" << endl;
			}
		
			cout << "To view specific changes for the tables listed above"  
				<< " enter the table numbers," << endl << " separated by commas" << endl
				<< "Enter D when you have finished, Q to quit." << endl;
	
			string sError;

			if( sError.length() )
			{
				cout << "***" + sError + "***" << endl;
				sError="";
			}

			string sAction;
			cin >> sAction;
			
			if( sAction=="D" || sAction=="d" )
				break;
			if( sAction=="Q" || sAction=="q" )
				return;  //abort

			string::size_type pos = 0;
			string sNumber;
	
			while( (sNumber=StringUtils::Tokenize(sAction,",",pos)).length() )
			{
				int iNumber=atoi(sNumber.c_str())-1;  //Our vector is 0 based, subtract 1
				if( iNumber<0 || iNumber>(int) vectTable.size()-1 )
				{
					sError ="Invalid table number";
					break;
				}
				
				flag = 1;

				Table *pTable = vectTable[iNumber];
				
				NewSection();

				cout << pTable->Name_get() << endl;
				
				//showing the modified rows
				query_string = "DESCRIBE " + pTable->Name_get();
				result_set.r = mysql_query_result(query_string);
				query_string = "";
				i = 0;
				int flagbreak = 0; 
                while ((row = mysql_fetch_row(result_set.r)) && (flagbreak == 0))
				{
					string s = "Modification_RecordInfo";
					if (row[0] == s){
                        flagbreak = 1;
					}
					else
						if (query_string == ""){
							query_string = row[0];
							i++;
						}
						else{
							query_string = query_string + ", " + row[0];
							i++;
						}
				}

				query_string = "SELECT " + query_string + " FROM " + pTable->Name_get() + " WHERE ((psc_new>0) || ((psc_del>0) || (psc_mod>0)))";
				result_set.r = mysql_query_result(query_string);

				while (row = mysql_fetch_row(result_set.r)){					
					for (int j = 0;j < i; j++)
						if (row[j])
							cout << row[j] << "    ";
						else
							cout << "NULL" << "    ";
					cout << endl;
				}
	 
				cout << endl << "If you want to check in changes press S." << endl
					<< "If you don't want to check in changes press Q." << endl;

				string sAnsw;
				cin >> sAnsw;
				
				if (sAnsw == "S" || sAnsw == "s"){
					itToChkIn = g_GlobalConfig.m_mapTable.find(pTable->Name_get());
					mapTableToCheckIn.insert(*itToChkIn);

					g_GlobalConfig.m_mapTable.erase(pTable->Name_get());
				}

				if (sAnsw == "Q" || sAnsw == "q")
					g_GlobalConfig.m_mapTable.erase(pTable->Name_get());
			}
		}

	if (flag == 1) {
		g_GlobalConfig.m_mapTable.clear();
		for(MapTable::iterator it=mapTableToCheckIn.begin();it!=mapTableToCheckIn.end();++it)
			g_GlobalConfig.m_mapTable[ (*it).first ] = (*it).second;
	}

}

int Database::PromptForRepositories()
{
	m_bInteractiveMode=true;

	string sError="";
	while(true)
	{
		NewSection();
		vector<Repository *> vectRepository;  // A numeric reference to the tables the user can change
		for(MapRepository::iterator it=m_mapRepository.begin();it!=m_mapRepository.end();++it)
		{
			Repository *pRepository = (*it).second;
			vectRepository.push_back(pRepository);
			cout << setw(3) << (int) vectRepository.size();
			cout << ( g_GlobalConfig.m_mapRepository.find(pRepository->Name_get())!=g_GlobalConfig.m_mapRepository.end() ? " *" : "  ") << pRepository->Name_get() << endl;
		}

		cout << "Repositories marked with * are to be included." << endl
			<< "Enter the Repository numbers, separated by commas, to toggle the * flag." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length() )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return (int) g_GlobalConfig.m_mapRepository.size();
		if( sAction=="Q" || sAction=="q" )
			return -1;  // means abort

		string::size_type pos=0;
		string sNumber;
		while( (sNumber=StringUtils::Tokenize(sAction,",",pos)).length() )
		{
			int iNumber=atoi(sNumber.c_str())-1;  // Our vector is 0 based, subtract 1
			if( iNumber<0 || iNumber>(int) vectRepository.size()-1 )
			{
				sError ="Invalid Repository number";
				break;
			}
			Repository *pRepository = vectRepository[iNumber];
			if( g_GlobalConfig.m_mapRepository.find(pRepository->Name_get())!=g_GlobalConfig.m_mapRepository.end() )
				g_GlobalConfig.m_mapRepository.erase( pRepository->Name_get() );
			else
				g_GlobalConfig.m_mapRepository[pRepository->Name_get()] = pRepository;
		}
	}
}

int Database::ConfirmUsersToCheckIn(map<int,MapTable *> &mapSelectedUsers)
{
	string sError="";
	while(true)
	{
		NewSection();
		for(map<int,MapTable *>::iterator it=g_GlobalConfig.m_mapUsers.begin();it!=g_GlobalConfig.m_mapUsers.end();++it)
		{
			MapTable *pMapTable = (*it).second;
			cout << (mapSelectedUsers.find( (*it).first )!=mapSelectedUsers.end() ? "* " : "  ")
				<< "User ID: " << (*it).first << " modified the following tables: " << endl
				<< "\t\t\t";
			int ColumnPosition=0;
			for(MapTable::iterator it=pMapTable->begin();it!=pMapTable->end();++it)
			{
				Table *pTable = (*it).second;
				if( ColumnPosition && ColumnPosition + pTable->Name_get().length() > SCREEN_WIDTH )
				{
					cout << endl;
					ColumnPosition=0;
				}
				else
				{
					if( ColumnPosition )
						cout << ",";
					++ColumnPosition;
				}
				cout << pTable->Name_get();
			}
			if( ColumnPosition )
				cout << endl;
		}

		cout << "Users marked with * are to be included." << endl
			<< "Enter the users ID's, separated by commas, to toggle the * flag." << endl
			<< "Enter + to select all users, - to deselect all users." << endl
			<< "Enter D when you have finished, Q to quit." << endl;

		if( sError.length() )
		{
			cout << "***" + sError + "***" << endl;
			sError="";
		}

		string sAction;
		cin >> sAction;
		if( sAction=="D" || sAction=="d" )
			return (int) mapSelectedUsers.size();
		else if( sAction=="Q" || sAction=="q" )
			return -1;  // means abort

		if ( sAction == "-" )
			mapSelectedUsers.clear();
		else 
			if ( sAction == "+" )
				for (map<int,MapTable*>::iterator it=g_GlobalConfig.m_mapUsers.begin();it!=g_GlobalConfig.m_mapUsers.end();++it)
					mapSelectedUsers = g_GlobalConfig.m_mapUsers;

			else
			{
				string::size_type pos=0;
				string sNumber;
				while( (sNumber=StringUtils::Tokenize(sAction,",",pos)).length() )
				{
					int iNumber=atoi(sNumber.c_str());
					if( g_GlobalConfig.m_mapUsers.find(iNumber)==g_GlobalConfig.m_mapUsers.end() )
					{
						sError ="Invalid User ID";
						break;
					}
					if( mapSelectedUsers.find(iNumber)!=mapSelectedUsers.end() )
						mapSelectedUsers.erase( iNumber );
					else
						mapSelectedUsers[iNumber] = g_GlobalConfig.m_mapUsers[iNumber];
				}
			}
	}
}

bool Database::ConfirmRecordsToCheckIn()
{
// If any of the records to be checked in refer to a record with an auto-increement field that is new and is not being checked in, this will fail

	return true;
}


