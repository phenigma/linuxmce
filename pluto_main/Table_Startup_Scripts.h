#ifndef __Table_Startup_Scripts_H__
#define __Table_Startup_Scripts_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Startup_Scripts.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Startup_Scripts
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Startup_Scripts(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Startup_Scripts")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Startup_Scripts();

private:		
	friend class Row_Startup_Scripts;
	struct Key
	{
		friend class Row_Startup_Scripts;
		long int pk_PK_Startup_Scripts;

		
		Key(long int in_PK_Startup_Scripts);
	
		Key(class Row_Startup_Scripts *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Startup_Scripts::Key &key1, const Table_Startup_Scripts::Key &key2) const;
	};	

	map<Table_Startup_Scripts::Key, class Row_Startup_Scripts*, Table_Startup_Scripts::Key_Less> cachedRows;
	map<Table_Startup_Scripts::Key, class Row_Startup_Scripts*, Table_Startup_Scripts::Key_Less> deleted_cachedRows;
	vector<class Row_Startup_Scripts*> addedRows;
	vector<class Row_Startup_Scripts*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Startup_Scripts*> *rows);
	class Row_Startup_Scripts* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Startup_Scripts* GetRow(long int in_PK_Startup_Scripts);
	

private:	
	
		
	class Row_Startup_Scripts* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Startup_Scripts : public TableRow, public SerializeClass
	{
		friend struct Table_Startup_Scripts::Key;
		friend class Table_Startup_Scripts;
	private:
		Table_Startup_Scripts *table;
		
		long int m_PK_Startup_Scripts;
string m_Command;
string m_Description;
long int m_Boot_Order;
long int m_Background;
long int m_Enabled;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Startup_Scripts_get();
string Command_get();
string Description_get();
long int Boot_Order_get();
long int Background_get();
long int Enabled_get();

		
		void PK_Startup_Scripts_set(long int val);
void Command_set(string val);
void Description_set(string val);
void Boot_Order_set(long int val);
void Background_set(long int val);
void Enabled_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Startup_Scripts(Table_Startup_Scripts *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Startup_Scripts *Table_Startup_Scripts_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Startup_Scripts+ m_Command+ m_Description+ m_Boot_Order+ m_Background+ m_Enabled;
		}
	private:
		void SetDefaultValues();
		
		string PK_Startup_Scripts_asSQL();
string Command_asSQL();
string Description_asSQL();
string Boot_Order_asSQL();
string Background_asSQL();
string Enabled_asSQL();

	};

#endif

