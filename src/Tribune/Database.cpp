#include "PlutoUtils/CommonIncludes.h"	
#include "Database.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "RA/RA_Processor.h"
#include "CommonFunctions.h"

#include <iomanip>
#include <sstream>

using namespace Tribune;

Database::Database( string db_host, string db_user, string db_pass, string db_name, int db_port )
	: MySqlHelper( db_host, db_user, db_pass, db_name, db_port )
{
	if( !m_bConnected )
	{
		m_sMySQLDBName = db_name; // Set it back and try again
		MySQLConnect();
		if( !m_bConnected )
			return;
	}
}

void Database::StartTransaction( ) 
{
	if( threaded_mysql_query("START TRANSACTION")<0 )
	{
		cerr << "Could not start transaction" << endl;
		throw "Database error";
	}
}

void Database::Commit( )
{
	cout << "Doing a Commit" << endl;
	if( threaded_mysql_query("COMMIT")<0 )
	{
		cerr << "Could not commit transaction" << endl;
		throw "Database error";
	}
}

void Database::Rollback( )
{
	cout << "Doing a rollback" << endl;
	if( threaded_mysql_query("ROLLBACK")<0 )
	{
		cerr << "Could not rollback transaction" << endl;
		throw "Database error";
	}
}

