#ifndef __Table_Template_H__
#define __Table_Template_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Template.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Template
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Template(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Template")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Template();

private:		
	friend class Row_Template;
	struct Key
	{
		friend class Row_Template;
		long int pk_PK_Template;

		
		Key(long int in_PK_Template);
	
		Key(class Row_Template *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Template::Key &key1, const Table_Template::Key &key2) const;
	};	

	map<Table_Template::Key, class Row_Template*, Table_Template::Key_Less> cachedRows;
	map<Table_Template::Key, class Row_Template*, Table_Template::Key_Less> deleted_cachedRows;
	vector<class Row_Template*> addedRows;
	vector<class Row_Template*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Template*> *rows);
	class Row_Template* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Template* GetRow(long int in_PK_Template);
	

private:	
	
		
	class Row_Template* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Template : public TableRow, public SerializeClass
	{
		friend struct Table_Template::Key;
		friend class Table_Template;
	private:
		Table_Template *table;
		
		long int m_PK_Template;
string m_Description;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Template_get();
string Description_get();

		
		void PK_Template_set(long int val);
void Description_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Template(Table_Template *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Template *Table_Template_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Template_getrows(vector <class Row_CommandGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Template+ m_Description;
		}
	private:
		void SetDefaultValues();
		
		string PK_Template_asSQL();
string Description_asSQL();

	};

#endif

