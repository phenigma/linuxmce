#ifndef __Table_OperatingSystem_Directory_H__
#define __Table_OperatingSystem_Directory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_OperatingSystem_Directory.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_OperatingSystem_Directory
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_OperatingSystem_Directory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("OperatingSystem_Directory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_OperatingSystem_Directory();

private:		
	friend class Row_OperatingSystem_Directory;
	struct Key
	{
		friend class Row_OperatingSystem_Directory;
		long int pk_PK_OperatingSystem_Directory;

		
		Key(long int in_PK_OperatingSystem_Directory);
	
		Key(class Row_OperatingSystem_Directory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_OperatingSystem_Directory::Key &key1, const Table_OperatingSystem_Directory::Key &key2) const;
	};	

	map<Table_OperatingSystem_Directory::Key, class Row_OperatingSystem_Directory*, Table_OperatingSystem_Directory::Key_Less> cachedRows;
	map<Table_OperatingSystem_Directory::Key, class Row_OperatingSystem_Directory*, Table_OperatingSystem_Directory::Key_Less> deleted_cachedRows;
	vector<class Row_OperatingSystem_Directory*> addedRows;
	vector<class Row_OperatingSystem_Directory*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_OperatingSystem_Directory*> *rows);
	class Row_OperatingSystem_Directory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_OperatingSystem_Directory* GetRow(long int in_PK_OperatingSystem_Directory);
	

private:	
	
		
	class Row_OperatingSystem_Directory* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_OperatingSystem_Directory : public TableRow, public SerializeClass
	{
		friend struct Table_OperatingSystem_Directory::Key;
		friend class Table_OperatingSystem_Directory;
	private:
		Table_OperatingSystem_Directory *table;
		
		long int m_PK_OperatingSystem_Directory;
long int m_FK_OperatingSystem;
long int m_FK_Directory;
string m_Path;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_OperatingSystem_Directory_get();
long int FK_OperatingSystem_get();
long int FK_Directory_get();
string Path_get();

		
		void PK_OperatingSystem_Directory_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Directory_set(long int val);
void Path_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_OperatingSystem_Directory(Table_OperatingSystem_Directory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_OperatingSystem_Directory *Table_OperatingSystem_Directory_get() { return table; };

		// Return the rows for foreign keys 
		class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Directory* FK_Directory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_OperatingSystem_Directory+ m_FK_OperatingSystem+ m_FK_Directory+ m_Path;
		}
	private:
		void SetDefaultValues();
		
		string PK_OperatingSystem_Directory_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Directory_asSQL();
string Path_asSQL();

	};

#endif

