/**
 * @file: TableInfo.h
 * @brief header file for the TableInfo class
 * Purpose: Storing table parameters and column parameters
 * 
 * Members:
 * Name     Type        Description         Extra info
 *
 * table_name  string       Table name
 * fields    vector<Field*> Vector of column parameters
 * 
 * loadFromDB  bool ( * )( MYSQL * db, string table ) Load table definition from database
 * HasPrimaryKeys bool ( * ) ( )   If table has Primay Keys
 */

#ifndef TableInfo_h
#define TableInfo_h

#include "mysql.h"
#include "Field.h"

#include <string>
#include <vector>
using namespace std;

using namespace sqlCVS;

/**
  * @brief a brief documentation of the class
  * @todo  complete the documentation
  */
  
class TableInfo
{
protected:
	string table_name;	/**<  the name of the table */
	vector<Field*> fields;	/**<  the fiels in the table */
	MYSQL *db;		/**<  points to the database the table belongs to */

	vector<Field *> m_vectFieldDependencies;

public:
	/** @brief constructor */
	
	TableInfo( MYSQL * use_db ) : db( use_db ) {}
	
	/** @todo add destructor */

	/** @brief Load table from database */
	
	bool loadFromDB( string table );
	
	/** Tests if the table has primary keys */
	
	bool HasPrimaryKeys( );

	/** Returns the table name */
	
	string get_table_name( );
};
#endif
