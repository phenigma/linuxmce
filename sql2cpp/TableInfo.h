/**
 *
 * @file TableInfo.h
 * @brief header file for the TableInfo class
 *
 */


#ifndef TABLEINFO_H
#define TABLEINFO_H

#include "mysql.h"
#include "FieldInfo.h"

#include <string>
#include <vector>
using namespace std;

/**
 * @brief class storing table parameters and column parameters
 */
class TableInfo
{

protected:

	string m_sTableName; /** < table name */
	vector<FieldInfo*> m_Fields; /** < vector of column parameters */
	MYSQL *m_pDB; /** < pointer to database */
	vector<FieldInfo*> m_vectFieldInfoDependencies; /** < vector of dependencie fields */

public:

	/**
	 * @brief constructor, initializes the database pointer member
	 */
	TableInfo( MYSQL * pDB ) : m_pDB( pDB ) {}

	/** @todo add destructor */

	/**
	 * @brief load table definition from table
	 */
	virtual bool loadFromDB( string sTable );
	
	/**
	 * @brief returns true if table has primary keys, false otherwise
	 */
	bool HasPrimaryKeys();

	/**
	 * @brief returns the table name
	 */
	string get_table_name();

};

#endif
