#include "PlutoUtils/CommonIncludes.h"	
#include "FieldInfo.h"

string FieldInfo::getCType()
{	
	switch(type)
	{
		case FIELD_TYPE_TINY:
#ifdef C99_FORMAT_SPECIFIERS
			return "char";
#else
			return "short int";
#endif
			
		case FIELD_TYPE_SHORT:
			return "short int";
			
		case FIELD_TYPE_LONG:
			return "long int";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			
		case FIELD_TYPE_LONGLONG:
#ifdef C99_FORMAT_SPECIFIERS
			return "long long int";
#else
			return "__int64";
#endif
			break;
					
		case FIELD_TYPE_FLOAT:
			return "float";
			break;
			
		case FIELD_TYPE_DOUBLE:
			return "double";
			break;
		
		case FIELD_TYPE_TIMESTAMP:
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_TIME:
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_YEAR:
			return "string";

		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_BLOB:
			return "string";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

string FieldInfo::getPrintFormat()
{	
	switch(type)
	{
		case FIELD_TYPE_TINY:
#ifdef C99_FORMAT_SPECIFIERS
			return "%hhi";
#else
			return "%hi";
#endif
			
		case FIELD_TYPE_SHORT:
			return "%hi";
			
		case FIELD_TYPE_LONG:
			return "%li";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			

		case FIELD_TYPE_LONGLONG:
#ifdef C99_FORMAT_SPECIFIERS
			return "%lli";
#else
			return "%I64i";
#endif
			break;
					
		case FIELD_TYPE_FLOAT:
			return "%f";
			break;

		case FIELD_TYPE_DOUBLE:
			return "%lf";
			break;
		
		case FIELD_TYPE_TIMESTAMP:
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_TIME:
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_YEAR:
			return "%s";

		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_BLOB:
			return "%s";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

string FieldInfo::getScanFormat()
{	
	switch(type)
	{
		case FIELD_TYPE_TINY:
#ifdef C99_FORMAT_SPECIFIERS
			return "%hhi";
#else
			return "%hi";
#endif
			
		case FIELD_TYPE_SHORT:
			return "%hi";
			
		case FIELD_TYPE_LONG:
			return "%li";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			
		
		case FIELD_TYPE_LONGLONG:
#ifdef C99_FORMAT_SPECIFIERS
			return "%lli";
#else
			return "%I64i";
#endif
			break;
					
		case FIELD_TYPE_FLOAT:
			return "%f";
			break;
			
		case FIELD_TYPE_DOUBLE:
			return "%lf";
			break;
		
		case FIELD_TYPE_TIMESTAMP:
		case FIELD_TYPE_DATE:
		case FIELD_TYPE_TIME:
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_YEAR:
			return "%s";

		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_BLOB:
			return "%s";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

string FieldInfo::getMType()
{	
	switch(type)
	{
		case FIELD_TYPE_TINY:
			return "FIELD_TYPE_TINY";
			
		case FIELD_TYPE_SHORT:
			return "FIELD_TYPE_SHORT";
			
		case FIELD_TYPE_LONG:
			return "FIELD_TYPE_LONG";
			
		case FIELD_TYPE_INT24:
			//TODO can it fail?			
		case FIELD_TYPE_LONGLONG:
			return "FIELD_TYPE_LONGLONG";
			break;
					
		case FIELD_TYPE_FLOAT:
			return "FIELD_TYPE_FLOAT";
			break;
			
		case FIELD_TYPE_DOUBLE:
			return "FIELD_TYPE_DOUBLE";
			break;
		
		case FIELD_TYPE_TIMESTAMP:
			return "FIELD_TYPE_TIMESTAMP";
		
		case FIELD_TYPE_DATE:
			return "FIELD_TYPE_DATE";
		
		case FIELD_TYPE_TIME:
			return "FIELD_TYPE_TIME";
		
		case FIELD_TYPE_DATETIME:
			return "FIELD_TYPE_DATETIME";
		
		case FIELD_TYPE_YEAR:
			return "FIELD_TYPE_YEAR";

		case FIELD_TYPE_STRING:
			return "FIELD_TYPE_STRING";
		
		case FIELD_TYPE_VAR_STRING:
			return "FIELD_TYPE_VAR_STRING";
		
		case FIELD_TYPE_BLOB:
			return "FIELD_TYPE_BLOB";
			
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
		case FIELD_TYPE_NULL:
		case FIELD_TYPE_DECIMAL:
			
		default:
			return "unsupported_type";
	}	
}

/* From the Mysql manual
FIELD_TYPE_TINY 	 TINYINT field
FIELD_TYPE_SHORT 	SMALLINT field
FIELD_TYPE_LONG 	INTEGER field
FIELD_TYPE_INT24 	MEDIUMINT field
FIELD_TYPE_LONGLONG 	BIGINT field
FIELD_TYPE_DECIMAL 	DECIMAL or NUMERIC field
FIELD_TYPE_FLOAT 	FLOAT field
FIELD_TYPE_DOUBLE 	DOUBLE or REAL field
FIELD_TYPE_TIMESTAMP 	TIMESTAMP field
FIELD_TYPE_DATE 	DATE field
FIELD_TYPE_TIME 	TIME field
FIELD_TYPE_DATETIME 	DATETIME field
FIELD_TYPE_YEAR 	YEAR field
FIELD_TYPE_STRING 	CHAR field
FIELD_TYPE_VAR_STRING 	VARCHAR field
FIELD_TYPE_BLOB 	BLOB or TEXT field (use max_length to determine the maximum length)
FIELD_TYPE_SET 	SET field
FIELD_TYPE_ENUM 	ENUM field
FIELD_TYPE_NULL 	NULL-type field
FIELD_TYPE_CHAR 	Deprecated; use FIELD_TYPE_TINY instead
*/

string FieldInfo::getDType()
{	
	switch(type)
	{
		case FIELD_TYPE_TINY:
			return "TINYINT";
		case FIELD_TYPE_SHORT:
			return "SMALLINT";
		case FIELD_TYPE_LONG:
			return "INTEGER";
		case FIELD_TYPE_INT24:
			return "MEDIUMINT";
		case FIELD_TYPE_LONGLONG:
			return "BIGINT";
		case FIELD_TYPE_DECIMAL:
			return "DECIMAL";
		case FIELD_TYPE_FLOAT:
			return "FLOAT";
		case FIELD_TYPE_DOUBLE:
			return "DOUBLE";
		case FIELD_TYPE_TIMESTAMP:
			return "TIMESTAMP";
		case FIELD_TYPE_DATE:
			return "FIELD_TYPE_DATE";
		case FIELD_TYPE_TIME:
			return "FIELD_TYPE_DATE";
		case FIELD_TYPE_DATETIME:
			return "FIELD_TYPE_DATETIME";
		case FIELD_TYPE_YEAR:
			return "FIELD_TYPE_YEAR";
		case FIELD_TYPE_STRING:
			return "CHAR";
		case FIELD_TYPE_VAR_STRING:
			return "VARCHAR";
		case FIELD_TYPE_BLOB:
			return "BLOB";
		case FIELD_TYPE_SET:
			return "SET";
		case FIELD_TYPE_ENUM:
			return "ENUM";
		case FIELD_TYPE_NULL:
			return "NULL";
			
		default:
			return "unsupported_type";
	}	
}
