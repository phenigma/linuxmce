#ifndef __Table_StartupScript_H__
#define __Table_StartupScript_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_StartupScript.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_StartupScript
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_StartupScript(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("StartupScript")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_StartupScript();

private:		
	friend class Row_StartupScript;
	struct Key
	{
		friend class Row_StartupScript;
		long int pk_PK_StartupScript;

		
		Key(long int in_PK_StartupScript);
	
		Key(class Row_StartupScript *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_StartupScript::Key &key1, const Table_StartupScript::Key &key2) const;
	};	

	map<Table_StartupScript::Key, class Row_StartupScript*, Table_StartupScript::Key_Less> cachedRows;
	map<Table_StartupScript::Key, class Row_StartupScript*, Table_StartupScript::Key_Less> deleted_cachedRows;
	vector<class Row_StartupScript*> addedRows;
	vector<class Row_StartupScript*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_StartupScript*> *rows);
	class Row_StartupScript* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_StartupScript* GetRow(long int in_PK_StartupScript);
	

private:	
	
		
	class Row_StartupScript* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_StartupScript : public TableRow, public SerializeClass
	{
		friend struct Table_StartupScript::Key;
		friend class Table_StartupScript;
	private:
		Table_StartupScript *table;
		
		long int m_PK_StartupScript;
string m_Command;
string m_Description;
string m_Parameter_Syntax;
long int m_Core_Boot_Order;
long int m_Core_Background;
long int m_Core_Enabled;
string m_Core_Parameter;
long int m_MD_Boot_Order;
short int m_MD_Background;
short int m_MD_Enabled;
string m_MD_Parameter;
long int m_Hybrid_Boot_Order;
short int m_Hybrid_Background;
short int m_Hybrid_Enabled;
string m_Hybrid_Parameter;

		bool is_null[16];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_StartupScript_get();
string Command_get();
string Description_get();
string Parameter_Syntax_get();
long int Core_Boot_Order_get();
long int Core_Background_get();
long int Core_Enabled_get();
string Core_Parameter_get();
long int MD_Boot_Order_get();
short int MD_Background_get();
short int MD_Enabled_get();
string MD_Parameter_get();
long int Hybrid_Boot_Order_get();
short int Hybrid_Background_get();
short int Hybrid_Enabled_get();
string Hybrid_Parameter_get();

		
		void PK_StartupScript_set(long int val);
void Command_set(string val);
void Description_set(string val);
void Parameter_Syntax_set(string val);
void Core_Boot_Order_set(long int val);
void Core_Background_set(long int val);
void Core_Enabled_set(long int val);
void Core_Parameter_set(string val);
void MD_Boot_Order_set(long int val);
void MD_Background_set(short int val);
void MD_Enabled_set(short int val);
void MD_Parameter_set(string val);
void Hybrid_Boot_Order_set(long int val);
void Hybrid_Background_set(short int val);
void Hybrid_Enabled_set(short int val);
void Hybrid_Parameter_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_StartupScript(Table_StartupScript *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_StartupScript *Table_StartupScript_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Device_StartupScript_FK_StartupScript_getrows(vector <class Row_Device_StartupScript*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_StartupScript+ m_Command+ m_Description+ m_Parameter_Syntax+ m_Core_Boot_Order+ m_Core_Background+ m_Core_Enabled+ m_Core_Parameter+ m_MD_Boot_Order+ m_MD_Background+ m_MD_Enabled+ m_MD_Parameter+ m_Hybrid_Boot_Order+ m_Hybrid_Background+ m_Hybrid_Enabled+ m_Hybrid_Parameter;
		}
	private:
		void SetDefaultValues();
		
		string PK_StartupScript_asSQL();
string Command_asSQL();
string Description_asSQL();
string Parameter_Syntax_asSQL();
string Core_Boot_Order_asSQL();
string Core_Background_asSQL();
string Core_Enabled_asSQL();
string Core_Parameter_asSQL();
string MD_Boot_Order_asSQL();
string MD_Background_asSQL();
string MD_Enabled_asSQL();
string MD_Parameter_asSQL();
string Hybrid_Boot_Order_asSQL();
string Hybrid_Background_asSQL();
string Hybrid_Enabled_asSQL();
string Hybrid_Parameter_asSQL();

	};

#endif

