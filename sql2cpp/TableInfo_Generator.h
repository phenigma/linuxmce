#ifndef TableInfo_Generator_h
#define TableInfo_Generator_h

#include "TableInfo.h"

#include <string>
#include <map>
using namespace std;

class TableInfo_Generator : public TableInfo
{
private:
	string table_class_prefix;
	string row_class_prefix;
	string primary_key_class_prefix;
			
private:
	map<string,TableInfo_Generator*> *p_tables;
	
public:
	TableInfo_Generator(MYSQL * db, map<string,TableInfo_Generator*> *p):
		TableInfo(db),
		table_class_prefix("Table_"),
		row_class_prefix("Row_"),
		primary_key_class_prefix("PrimaryKey_")
	{
		p_tables=p;
	};
	
	string ExpandString(string s);
				
	//template variables -- get_VAR_NAME -- @VAR_NAME@
	string get_fields_declaration();
	string get_member_variable_list();

	string get_getters_declaration();
	string get_setters_declaration();
	string get_null_getters_declaration();
	string get_null_setters_declaration();
	
	string get_getters_definition();
	string get_setters_definition();
	string get_null_getters_definition();
	string get_null_setters_definition();
	
	string get_primary_fields_declaration();
	string get_primary_fields_in_typed_list();
	string get_primary_fields_in_untyped_list();
	string get_primary_fields_assign_list();
	string get_key_less_statement();
	string get_set_default_values();
	string get_fields_count();
	string get_primary_fields_count();
	string get_primary_fields_where_list();
	string get_bind_parameters_to_key();
	string get_bind_results_to_fields();
	string get_init_row_from_temp_string_fields();
	string get_primary_fields_assign_from_row();
	
	string get_build_query_for_key();
	string get_init_fields_from_row();
	string get_ifdef_have_primary_keys();
	string get_endif_have_primary_keys();
	string get_generate_insert_query();
	string get_generate_update_query();
	string get_fields_list_comma_separated();
	string get_primary_autoinc_key_init();
	string get_fields_sql_getters_declaration();
	string get_fields_sql_getters_definition();
																			
	string rows_getters_declaration();
	string rows_getters_definition();
	string rows_getters_includes();
	
	string fk_rows_getters_declaration();
	string fk_rows_getters_definition();
	string fk_rows_getters_includes();
				
	string table_constants_defines();
	string table_struct_defines();
	
	//TODO add destructor
};

#endif
