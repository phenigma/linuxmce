/*
 * File: FieldInfo.h
 * Defining: class FieldInfo
 * Purpose: Storing table row field or table column parameters
 * 
 * Members:
 * Name         Type              Description           Extra info
 *
 * name         char *            Field name
 * def          char *            Default value         Can be NULL
 * type         enum_field_types  Field type            int-compatible-enum
 * length       unsigned int      Field length
 * flags        unsigned int      (not yet known)
 * decimals     unsigned int      Field decimal places
 */

#ifndef FieldInfo_h
#define FieldInfo_h

#include "mysql.h"
#include <string>
using namespace std;

class FieldInfo
{
public:
	char *name;
	char *def;
	enum_field_types type;
	unsigned int length;
	unsigned int flags;
	unsigned int decimals;
	
	//constructor
	FieldInfo(MYSQL_FIELD *info)
	{
		name = strdup(info->name);
		//is it correct? for BLOBs, etc.
		def = info->def?strdup(info->def):NULL;
		type = info->type;
		length = info->length;
		flags = info->flags;
		decimals = info->decimals;
	}
	
	//returns C type for the field
	string getCType();
	string getMType();
	string getDType();

	string getPrintFormat();
	string getScanFormat();
	
	//TODO add destructor
};

#endif
