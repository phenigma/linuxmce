#ifndef __Table_Device_Package_Directory_H__
#define __Table_Device_Package_Directory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_Package_Directory.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Device_Package_Directory
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Device_Package_Directory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Device_Package_Directory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Device_Package_Directory();

private:		
	friend class Row_Device_Package_Directory;
	struct Key
	{
		friend class Row_Device_Package_Directory;
		long int pk_FK_Device;
long int pk_FK_Package_Directory;

		
		Key(long int in_FK_Device, long int in_FK_Package_Directory);
	
		Key(class Row_Device_Package_Directory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_Package_Directory::Key &key1, const Table_Device_Package_Directory::Key &key2) const;
	};	

	map<Table_Device_Package_Directory::Key, class Row_Device_Package_Directory*, Table_Device_Package_Directory::Key_Less> cachedRows;
	map<Table_Device_Package_Directory::Key, class Row_Device_Package_Directory*, Table_Device_Package_Directory::Key_Less> deleted_cachedRows;
	vector<class Row_Device_Package_Directory*> addedRows;
	vector<class Row_Device_Package_Directory*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Device_Package_Directory*> *rows);
	class Row_Device_Package_Directory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_Package_Directory* GetRow(long int in_FK_Device, long int in_FK_Package_Directory);
	

private:	
	
		
	class Row_Device_Package_Directory* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Device_Package_Directory : public TableRow, public SerializeClass
	{
		friend struct Table_Device_Package_Directory::Key;
		friend class Table_Device_Package_Directory;
	private:
		Table_Device_Package_Directory *table;
		
		long int m_FK_Device;
long int m_FK_Package_Directory;
string m_Path;
short int m_Verified;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Device_get();
long int FK_Package_Directory_get();
string Path_get();
short int Verified_get();

		
		void FK_Device_set(long int val);
void FK_Package_Directory_set(long int val);
void Path_set(string val);
void Verified_set(short int val);

		
		bool Path_isNull();

			
		void Path_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Device_Package_Directory(Table_Device_Package_Directory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_Package_Directory *Table_Device_Package_Directory_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_Package_Directory* FK_Package_Directory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Device+ m_FK_Package_Directory+ m_Path+ m_Verified;
		}
	private:
		void SetDefaultValues();
		
		string FK_Device_asSQL();
string FK_Package_Directory_asSQL();
string Path_asSQL();
string Verified_asSQL();

	};

#endif

