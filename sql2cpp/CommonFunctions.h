/**
 *
 * @file CommonFunctions.h
 * @brief header file for the common functions
 *
 */


#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <string>
#include <map>
using namespace std;

string int2string( int i ); /** < converts the integer parameter to a string */
string getTableFromForeignKey( string sField, map<string, class TableInfo_Generator *> *mapTableNames ); /** returnes a table name from a foreign key field name */

#endif
