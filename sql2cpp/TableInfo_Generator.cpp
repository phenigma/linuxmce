#include "PlutoUtils/CommonIncludes.h"	
#include "TableInfo_Generator.h"

#include <iostream>
using namespace std;

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "CommonFunctions.h"

string TableInfo_Generator::ExpandString(string s)
{
	if (s=="table_name")
		return get_table_name();
	else
	if (s=="fields_declaration")
		return get_fields_declaration();
	else
		if (s=="getters_declaration")
			return get_getters_declaration();
	else
		if (s=="setters_declaration")
			return get_setters_declaration();
	else
		if (s=="null_getters_declaration")
			return get_null_getters_declaration();
	else
		if (s=="null_setters_declaration")
			return get_null_setters_declaration();
	else
		if (s=="getters_definition")
			return get_getters_definition();
	else
		if (s=="setters_definition")
			return get_setters_definition();
	else
		if (s=="null_getters_definition")
			return get_null_getters_definition();
	else
		if (s=="null_setters_definition")
			return get_null_setters_definition();
	else
		if (s=="primary_fields_declaration")
			return get_primary_fields_declaration();
	else
		if (s=="primary_fields_in_typed_list")
			return get_primary_fields_in_typed_list();
	else
		if (s=="primary_fields_in_untyped_list")
			return get_primary_fields_in_untyped_list();
	else
		if (s=="primary_fields_assign_list")
			return get_primary_fields_assign_list();
	else
		if (s=="key_less_statement")
			return get_key_less_statement();
	else
		if (s=="set_default_values")
			return get_set_default_values();
	else
		if (s=="fields_count")
			return get_fields_count();
	else
		if (s=="primary_fields_count")
			return get_primary_fields_count();
	else
		if (s=="primary_fields_where_list")
			return get_primary_fields_where_list();
	else
		if (s=="bind_parameters_to_key")
			return get_bind_parameters_to_key();
	else
		if (s=="bind_results_to_fields")
			return get_bind_results_to_fields();
	else
		if (s=="init_row_from_temp_string_fields")
			return get_init_row_from_temp_string_fields();
	else
		if (s=="primary_fields_assign_from_row")
			return get_primary_fields_assign_from_row();
	else
		if (s=="build_query_for_key")
			return get_build_query_for_key();
	else
		if (s=="init_fields_from_row")
			return get_init_fields_from_row();
	else
		if (s=="ifdef_have_primary_keys")
			return get_ifdef_have_primary_keys();
	else
		if (s=="endif_have_primary_keys")
			return get_endif_have_primary_keys();
	else
		if (s=="generate_insert_query")
			return get_generate_insert_query();
	else
		if (s=="generate_update_query")
			return get_generate_update_query();
	else 
		if (s=="fields_list_comma_separated")
			return get_fields_list_comma_separated();
	else
		if (s=="primary_autoinc_key_init")
			return get_primary_autoinc_key_init();
	else
		if (s=="fields_sql_getters_declaration")
			return get_fields_sql_getters_declaration();
	else
		if (s=="fields_sql_getters_definition")
			return get_fields_sql_getters_definition();
	else
		if (s=="rows_getters_declaration")
			return rows_getters_declaration();
	else
		if (s=="rows_getters_definition")
			return rows_getters_definition();
	else
		if (s=="rows_getters_includes")
			return rows_getters_includes();
	else
		if (s=="fk_rows_getters_declaration")
			return fk_rows_getters_declaration();
	else
		if (s=="fk_rows_getters_definition")
			return fk_rows_getters_definition();
	else
		if (s=="fk_rows_getters_includes")
			return fk_rows_getters_includes();
	else
		if (s=="table_constants_defines")
			return table_constants_defines();
	else
		if (s=="table_struct_defines")
			return table_struct_defines();
	else
		if (s=="member_variable_list")
			return get_member_variable_list();
	else
	{
		cerr << "Unknown: " << s << endl;
		return "";
	}
	/*	
	get_ifdef_have_primary_keys	else
		if (s=="")
			return ();
	*/	

	
}

string TableInfo_Generator::table_constants_defines()
{
	string s="";

	string SQL = "SELECT PK_" + get_table_name() + ",Description,Define FROM " + get_table_name() + " ORDER BY PK_" + get_table_name();
	int iresult=mysql_query(db,SQL.c_str());
	if( iresult!=0 )
	{
		return "";
	}
	MYSQL_RES *res = mysql_store_result(db);
	MYSQL_ROW row;

	map<string,int> mapDefines; // Be sure there's no dups
	bool bFoundDups=false;

	while ((row = mysql_fetch_row(res)))
	{
		if( !row[1] && !row[2] )
			continue; // There must be either a description or define

		const char *define = row[2] && row[2][0]!=0 ? row[2] : row[1]; // Use the Define column unless it's empty
		
		string DefineString = FileUtils::ValidCPPName(define);
		if( mapDefines.find(DefineString)!=mapDefines.end() )
		{
			bFoundDups=true;
		}
		else
		{
			mapDefines[DefineString]=1;
			s += "#define " + StringUtils::ToUpper(get_table_name()) + "_" + DefineString + "_CONST " + row[0] + "\n";
		}
	}
	if( bFoundDups )
		cout << "There were duplicate define constants for table " << get_table_name() << endl;

	return s;
}

string TableInfo_Generator::table_struct_defines()
{
	string s="#define " + StringUtils::ToUpper(get_table_name()) + "_TABLE \"" + get_table_name() + "\"\n";
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
	{
		s += "#define " + StringUtils::ToUpper(get_table_name()) + "_" + StringUtils::ToUpper((*i)->name) + "_FIELD \"" + (*i)->name + "\"\n";
		s += "#define " + StringUtils::ToUpper(get_table_name()) + "_" + StringUtils::ToUpper((*i)->name) + "_TABLE_FIELD \"" + get_table_name() + "." + (*i)->name + "\"\n";
	}
	return s;
}

string TableInfo_Generator::rows_getters_includes()
{
	string s="";
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
	{
      	string table_name = getTableFromForeignKey((*i)->name,p_tables);
        if( table_name.length()==0 && table_name!=get_table_name() )
            continue;
		s = s + "#include \"Table_"+table_name+".h\"\n";

	}
	
	return s;
}


string TableInfo_Generator::rows_getters_declaration()
{
	string s="";
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
	{
		string table_name2 =  getTableFromForeignKey((*i)->name,p_tables);
		if( table_name2.length()==0 )
			continue;

		s = s + "class Row_" + table_name2 + "* "+(*i)->name+"_getrow();\n";
	}
	
	return s;
}

string TableInfo_Generator::rows_getters_definition()
{
	string s="";
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
	{
			
		string ref_table =  getTableFromForeignKey((*i)->name,p_tables);
		if( ref_table.length()==0 )
			continue;

		string field_name = (*i)->name;
							
		s = s + "class Row_" + ref_table + "* Row_" + table_name + "::" + (*i)->name+"_getrow()\n";
		s = s + "{\nPLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\n";
		s = s + "class Table_"+ref_table+" *pTable = table->database->"+ref_table+"_get();\n";
		s = s + "return pTable->GetRow(m_" + field_name + ");\n";
		/*
		s = s + "vector <TableRow *> v;\n";
		s = s + "vector <Row_"+ref_table+" *> d;\n";
		s = s + "d = table->database->tbl"+ref_table+"->GetRows(\"PK_"+ref_table+"=\"+"+field_name+"_asSQL());\n";
		s = s + "for (vector <Row_"+ref_table+" *>::iterator i=d.begin(); i!=d.end(); i++)\n v.push_back((*i));\n";
		s = s + "return v;\n";
		*/
		s = s + "}\n";
	}
	
	return s;
}

string TableInfo_Generator::fk_rows_getters_declaration()
{
    string s="";
	
	map<string,TableInfo_Generator *>::iterator i;
    for (i=p_tables->begin(); i!=p_tables->end(); ++i)
    {
		TableInfo_Generator *info = (*i).second;
		for(size_t i=0;i<info->fields.size();++i)
		{
			FieldInfo *field = info->fields[i];
			if( getTableFromForeignKey(field->name,p_tables)==get_table_name() )
			{
				s = s + "void " + info->get_table_name() + "_" + field->name + "_getrows(vector <class Row_" + info->get_table_name() + "*> *rows);\n";
			}
		}
    }

    return s;
}

string TableInfo_Generator::fk_rows_getters_definition()
{
    string s="";

    map<string,TableInfo_Generator *>::iterator i;
    for (i=p_tables->begin(); i!=p_tables->end(); ++i)
    {
        TableInfo_Generator *info = (*i).second;
        for(size_t i=0;i<info->fields.size();++i)
        {
            FieldInfo *field = info->fields[i];
            if( getTableFromForeignKey(field->name,p_tables)==get_table_name() )
            {
                s = s + "void Row_" + get_table_name() + "::"+ info->get_table_name() + "_" + field->name + "_getrows(vector <class Row_" + info->get_table_name() + "*> *rows)\n";
				s = s + "{\nPLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\n";
				s = s + "class Table_"+info->get_table_name()+" *pTable = table->database->"+info->get_table_name()+"_get();\n";
				s = s + "pTable->GetRows(\""+ field->name + "=\" + StringUtils::itos(m_PK_" + get_table_name() + "),rows);\n";
                s = s + "}\n";
            }
        }
    }

    return s;
}

string TableInfo_Generator::fk_rows_getters_includes()
{
    string s="";

   	map<string,TableInfo_Generator *>::iterator i;
    for (i=p_tables->begin(); i!=p_tables->end(); ++i)
    {
        TableInfo_Generator *info = (*i).second;
        for(size_t i=0;i<info->fields.size();++i)
        {
            FieldInfo *field = info->fields[i];
            if( getTableFromForeignKey(field->name,p_tables)==get_table_name() )
            {
                s = s + "#include \"Table_" + info->get_table_name() + ".h\"\n";
            }
        }
    }


    return s;
}

string TableInfo_Generator::get_ifdef_have_primary_keys()
{
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			return "";
	
	return "/*\n";
}

string TableInfo_Generator::get_endif_have_primary_keys()
{
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			return "";
	
	return "*/\n";
}

string TableInfo_Generator::get_fields_declaration()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		s = s + (*i)->getCType() + " m_" + (*i)->name + ";" + "\n";
	
	return s;
}

string TableInfo_Generator::get_member_variable_list()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		s = s + "+" + " m_" + (*i)->name;
	
	return s;
}

string TableInfo_Generator::get_primary_fields_declaration()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			s = s + (*i)->getCType() + " pk_" + (*i)->name + ";" + "\n";
	
	return s;
}

string TableInfo_Generator::get_primary_fields_in_typed_list()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
	{
		if (s != "")
			s = s + ", ";
			
		s = s + (*i)->getCType() + " in_" + (*i)->name;
	}
	
	return s;
}

string TableInfo_Generator::get_fields_list_comma_separated()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
	{
		if (s != "")
			s = s + ", ";
			
		s = s + (*i)->name;
	}
	
	return s;
}

string TableInfo_Generator::get_primary_fields_in_untyped_list()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
	{
		if (s != "")
			s = s + ", ";
			
		s = s + "in_" + (*i)->name;
	}
	
	return s;
}

string TableInfo_Generator::get_primary_fields_assign_list()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			s = s + "pk_" + (*i)->name + " = in_" + (*i)->name + ";\n";
	
	return s;
}

string TableInfo_Generator::get_primary_autoinc_key_init()
{
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if (((*i)->flags & PRI_KEY_FLAG) && ((*i)->flags & AUTO_INCREMENT_FLAG ))
			return string("if (id!=0)\npRow->m_") + (*i)->name + "=id;\n";
	
	return "";
}

string TableInfo_Generator::get_primary_fields_assign_from_row()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			s = s + "pk_" + (*i)->name + " = pRow->m_" + (*i)->name + ";\n";
	
	return s;
}


string TableInfo_Generator::get_build_query_for_key()
{
	string code="";
	string condition="";	//default true to keep ..AND stuff happy
		
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
		{
			if (condition!="")
				condition = condition + "+\" AND \"+";
			
			condition = condition + "\"" + (*i)->name + "=\"";
			
			//temporary buffer			
			
			if (((*i)->getCType() != "string") && ((*i)->getCType() != "unsupported_type"))
			{
				code = code + "char tmp_" + (*i)->name + "[" ;
				code = code + "32];\n";
				code = code + "sprintf(tmp_" + (*i)->name+", \"" + (*i)->getPrintFormat() + "\", " + "key.pk_" + (*i)->name + ");\n\n";
				condition = condition + " + tmp_" + (*i)->name;
			}
			else
				if ((*i)->getCType() == "string")
				{
					code = code + "char tmp_" + (*i)->name + "[" ;
					code = code + int2string(1+2*(*i)->length) + "];\n";
					code = code + "mysql_real_escape_string(database->db_handle,tmp_" + (*i)->name 
							+ ", key.pk_" + (*i)->name + ".c_str(), (unsigned long) key.pk_" + (*i)->name + ".size());\n\n";
					condition = condition + " + \"\\\"\" + tmp_" + (*i)->name + "+ \"\\\"\"";
				}
				else
					condition = condition + " + \"\\\"\" + key.pk_" + (*i)->name + ".c_str()+ \"\\\"\"";
			
			
		}
			
				
//		return  code + "\n" + "string query;\n" + "query=query+\"select * from "+table_name+" where \" + " +condition+";\n";
		return  code + "\n" + "string condition;\n" + "condition = condition + " +condition+";\n";
}



string TableInfo_Generator::get_generate_insert_query()
{
	string code="";
	string condition="";
	int index=0;	
	
	string values_list="";
	
		
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
	{
		if (values_list!="")
			values_list = values_list + "+\", \"+";
		
		values_list = values_list + "pRow->"+(*i)->name+"_asSQL()";
	}
	
	return  code + "\n" + "string values_list_comma_separated;\n" + "values_list_comma_separated = values_list_comma_separated + " +values_list+";\n";
}

string TableInfo_Generator::get_generate_update_query()
{
	string code="";
	string condition="";
	int index=0;	
	
	string values_list="";
	
		
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
	{
		if (values_list!="")
			values_list = values_list + "+\", "+(*i)->name+"=\"+";
		else
			values_list = values_list + "\""+(*i)->name+"=\"+";
		
		values_list = values_list + "pRow->"+(*i)->name+"_asSQL()";
	}
	
	return  code + "\n" + "string update_values_list;\n" + "update_values_list = update_values_list + " +values_list+";\n";
}

string TableInfo_Generator::get_fields_sql_getters_declaration()
{
	string code="";
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		code = code + "string "+(*i)->name+"_asSQL();\n";
		
	return code;
}


string TableInfo_Generator::get_fields_sql_getters_definition()
{
	string code="";
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
	{
		code = code + "string Row_"+table_name+"::"+(*i)->name+"_asSQL()\n{\nPLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\n";
		code = code + "if (is_null["+int2string(index)+"])\n";
		code = code + "return \"NULL\";\n\n";
		
		
		if (((*i)->getCType() != "string") && ((*i)->getCType() != "unsupported_type"))
		{
			code = code + "char buf[32];\n";
			code = code + "sprintf(buf, \"" + (*i)->getPrintFormat() + "\", " + "m_" + (*i)->name + ");\n\n";
			code = code + "return buf;\n";
		}
		else
			if ((*i)->getCType() == "string")
		{
			code = code + "char buf[" + int2string(1+2*(*i)->length) + "];\n";		
			code = code + "mysql_real_escape_string(table->database->db_handle, buf, m_" + (*i)->name + ".c_str(), (unsigned long) m_" + (*i)->name + ".size());\n";
			code = code + "return string("")+\"\\\"\"+buf+\"\\\"\";\n";
		}
		else
			code = code + "return string("")+\"\\\"\"+m_" + (*i)->name+"+\"\\\"\";\n";
		
		code = code + "}\n\n";
	}	
		
	return code;
}


string TableInfo_Generator::get_getters_declaration()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		s = s + (*i)->getCType() + " " + (*i)->name + "_get();" + "\n";
	
	return s;
}

string TableInfo_Generator::get_getters_definition()
{
	string s;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		s = s + (*i)->getCType() + " Row_"+table_name+"::" + (*i)->name + "_get(){PLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\nreturn m_" + (*i)->name + ";}" + "\n";
	
	return s;
}

string TableInfo_Generator::get_setters_declaration()
{
	string s;
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		s = s + "void " + (*i)->name + "_set(" + (*i)->getCType() + " val);" + "\n";
	
	return s;
}


string TableInfo_Generator::get_setters_definition()
{
	string s;
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		s = s + "void " + "Row_"+table_name+"::"+ (*i)->name + "_set(" + (*i)->getCType() + " val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\nm_" + (*i)->name + " = val; is_modified=true; is_null["+int2string(index)+"]=false;}" + "\n";
	
	return s;
}

string TableInfo_Generator::get_null_getters_declaration()
{
	string s;
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		if (!((*i)->flags & NOT_NULL_FLAG))
			s = s + "bool " + (*i)->name + "_isNull();" + "\n";
	
	return s;
}

string TableInfo_Generator::get_null_getters_definition()
{
	string s;
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		if (!((*i)->flags & NOT_NULL_FLAG))
			s = s + "bool " + "Row_"+table_name+"::" + (*i)->name + "_isNull() {PLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\nreturn is_null["+int2string(index)+"];}" + "\n";
	
	return s;
}


string TableInfo_Generator::get_null_setters_declaration()
{
	string s;
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		if (!((*i)->flags & NOT_NULL_FLAG))
			s = s + "void " + (*i)->name + "_setNull(bool val);" + "\n";

	return s;	
}

string TableInfo_Generator::get_null_setters_definition()
{
	string s;
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		if (!((*i)->flags & NOT_NULL_FLAG))
			s = s + "void " +  "Row_"+table_name+"::"+(*i)->name + "_setNull(bool val){PLUTO_SAFETY_LOCK(M, table->m_Mutex);\n\nis_null["+int2string(index)+"]=val;}" + "\n";

	return s;	
}

string TableInfo_Generator::get_key_less_statement()
{
	string s;

	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			s = s + "if (key1.pk_" + (*i)->name + "!=key2.pk_" + (*i)->name + ")"
					+ "\n" + "return key1.pk_" + (*i)->name + "<key2.pk_" + (*i)->name + ";"
					+ "\n" + "else" + "\n";
	
	s = s + "return false;";
	
	return s;
}

string TableInfo_Generator::get_set_default_values()
{
	string s;
	
	int field_index=0;
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, field_index++)
	{
		if ((*i)->def == NULL)
			if (!((*i)->flags & NOT_NULL_FLAG))
				s = s + "is_null[" + int2string(field_index) + "] = true;\n";
			else
				s = s + "is_null[" + int2string(field_index) + "] = false;\n";
		else
			if ((*i)->getCType() == "string")
			{
				char *to = new char[2*(*i)->length + 1];
				mysql_real_escape_string(db, to, (*i)->def, (unsigned long) strlen((*i)->def)); 
				s = s + "m_" + (*i)->name + " = \"" + to + "\";\n";
				s = s + "is_null[" + int2string(field_index) + "] = false;\n";
				delete to;
			}
			else
				if (((*i)->getCType() != "unsupported_type") && ((*i)->getCType() != "MYSQL_DATE")) //TODO: how to handle it?
				{
					s = s + "m_" + (*i)->name + " = " + (*i)->def + ";\n";
					s = s + "is_null[" + int2string(field_index) + "] = false;\n";
				}
		
	}
	
	return s;
}
	

string TableInfo_Generator::get_fields_count()
{
	return int2string((int) fields.size());
}

string TableInfo_Generator::get_primary_fields_count()
{
	int count=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
			count++;

	return int2string(count);
}

string TableInfo_Generator::get_primary_fields_where_list()
{
	string s="";
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++)
		if ((*i)->flags & PRI_KEY_FLAG)
		{
			if (s != "")
				s = s + " AND ";
			s = s + (*i)->name + "=?";
		}
		
	return s;
}

string TableInfo_Generator::get_bind_parameters_to_key()
{
	string s="";
	int index=0;
	int primary_index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
		if ((*i)->flags & PRI_KEY_FLAG)
		{
			s = s + "is_param_null[" + int2string(primary_index) + "]=false;\n";
			s = s + "parameters[" + int2string(primary_index) + "].buffer_type=" + (*i)->getMType() + ";\n";
			
			if ((*i)->getCType() != "string")
			{
				s = s + "parameters[" + int2string(primary_index) + "].buffer=&(key.pk_" + (*i)->name + ");\n";
				s = s + "parameters[" + int2string(primary_index) + "].length=0;\n";
			}
			else
			{
				s = s + "char tmp_" + (*i)->name + "[" + int2string((*i)->length+1) + "];\n";
				s = s + "snprintf(tmp_" + (*i)->name + ", "+ int2string((*i)->length+1)+", \"%*s\", key.pk_" + (*i)->name
						+ ".length(), key.pk_" + (*i)->name + ".c_str());\n";
				s = s + "parameters[" + int2string(primary_index) + "].buffer=tmp_" + (*i)->name + ";\n";
				s = s + "parameters[" + int2string(primary_index) + "].length=&param_lengths["  + int2string(primary_index) +  "];\n";
			}
			
			s = s + "parameters[" + int2string(primary_index) + "].is_null=0;\n";

			s = s + "\n";
					
			primary_index++;
		}
	
		return s;
}

string TableInfo_Generator::get_bind_results_to_fields()
{
	string s="";
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
	{
		s = s + "results[" + int2string(index) + "].buffer_type=" + (*i)->getMType() + ";\n";
			
		if ((*i)->getCType() != "string")
		{
			s = s + "results[" + int2string(index) + "].buffer=&(pRow->m_" + (*i)->name + ");\n";
			s = s + "results[" + int2string(index) + "].length=0;\n";
		}
		else
		{
			s = s + "char rtmp_" + (*i)->name + "[" + int2string((*i)->length+1) + "];\n";
			s = s + "results[" + int2string(index) + "].buffer=rtmp_" + (*i)->name + ";\n";
			s = s + "results[" + int2string(index) + "].length=&result_lengths["  + int2string(index) +  "];\n";
		}
			
		s = s + "results[" + int2string(index) + "].is_null=&(pRow->is_null["+int2string(index)+"]);\n";
		
		s = s + "\n";
	}
	
	return s;
}

string TableInfo_Generator::get_init_row_from_temp_string_fields()
{
	string s="";
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
	{
		if ((*i)->getCType() == "string")
			s = s + "pRow->m_" + (*i)->name + " = string(rtmp_" + (*i)->name + ",result_lengths["  + int2string(index) +  "]);\n" ;
		
		s = s + "\n";
	}
	
	return s;
}

string TableInfo_Generator::get_init_fields_from_row()
{
	string s="";
	int index=0;
	
	for (vector<FieldInfo*>::iterator i=fields.begin(); i!=fields.end(); i++, index++)
	{
		s = s + "if (row[" + int2string(index) + "] == NULL)\n" +
				"{\n"+ 
				"pRow->is_null["+int2string(index)+"]=true;\n";
		
		if (((*i)->getCType() == "string")||((*i)->getCType() == "unsupported_type"))
			s = s + "pRow->m_" + (*i)->name + " = \"\";\n" ;
		else
			s = s + "pRow->m_" + (*i)->name + " = 0;\n" ;
		
		s = s + "}\nelse\n{\n";
		s = s + "pRow->is_null["+int2string(index)+"]=false;\n";
		
		if (((*i)->getCType() == "string")||((*i)->getCType() == "unsupported_type"))
			s = s + "pRow->m_" + (*i)->name + " = string(row[" + int2string(index) + "],lengths["  + int2string(index) +  "]);\n" ;
		else
			s = s + "sscanf(row[" + int2string(index) + "], \"" + (*i)->getScanFormat() + "\", &(pRow->m_" + (*i)->name + "));\n";
		
		s = s + "}\n\n";
	}
	
	return s;
}

