#ifndef __Table_psc_designer_batdet_H__
#define __Table_psc_designer_batdet_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_designer_batdet.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_psc_designer_batdet
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_designer_batdet(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("psc_designer_batdet")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_designer_batdet();

private:		
	friend class Row_psc_designer_batdet;
	struct Key
	{
		friend class Row_psc_designer_batdet;
		long int pk_PK_psc_designer_batdet;

		
		Key(long int in_PK_psc_designer_batdet);
	
		Key(class Row_psc_designer_batdet *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_designer_batdet::Key &key1, const Table_psc_designer_batdet::Key &key2) const;
	};	

	map<Table_psc_designer_batdet::Key, class Row_psc_designer_batdet*, Table_psc_designer_batdet::Key_Less> cachedRows;
	map<Table_psc_designer_batdet::Key, class Row_psc_designer_batdet*, Table_psc_designer_batdet::Key_Less> deleted_cachedRows;
	vector<class Row_psc_designer_batdet*> addedRows;
	vector<class Row_psc_designer_batdet*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_psc_designer_batdet*> *rows);
	class Row_psc_designer_batdet* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_designer_batdet* GetRow(long int in_PK_psc_designer_batdet);
	

private:	
	
		
	class Row_psc_designer_batdet* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_psc_designer_batdet : public TableRow, public SerializeClass
	{
		friend struct Table_psc_designer_batdet::Key;
		friend class Table_psc_designer_batdet;
	private:
		Table_psc_designer_batdet *table;
		
		long int m_PK_psc_designer_batdet;
string m_Value;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_psc_designer_batdet_get();
string Value_get();

		
		void PK_psc_designer_batdet_set(long int val);
void Value_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_designer_batdet(Table_psc_designer_batdet *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_designer_batdet *Table_psc_designer_batdet_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_psc_designer_batdet+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_designer_batdet_asSQL();
string Value_asSQL();

	};

#endif

