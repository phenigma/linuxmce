#ifndef __Table_Package_Directory_H__
#define __Table_Package_Directory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Directory.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Package_Directory
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Package_Directory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Package_Directory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Package_Directory();

private:		
	friend class Row_Package_Directory;
	struct Key
	{
		friend class Row_Package_Directory;
		long int pk_PK_Package_Directory;

		
		Key(long int in_PK_Package_Directory);
	
		Key(class Row_Package_Directory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Directory::Key &key1, const Table_Package_Directory::Key &key2) const;
	};	

	map<Table_Package_Directory::Key, class Row_Package_Directory*, Table_Package_Directory::Key_Less> cachedRows;
	map<Table_Package_Directory::Key, class Row_Package_Directory*, Table_Package_Directory::Key_Less> deleted_cachedRows;
	vector<class Row_Package_Directory*> addedRows;
	vector<class Row_Package_Directory*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Package_Directory*> *rows);
	class Row_Package_Directory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Directory* GetRow(long int in_PK_Package_Directory);
	

private:	
	
		
	class Row_Package_Directory* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Package_Directory : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Directory::Key;
		friend class Table_Package_Directory;
	private:
		Table_Package_Directory *table;
		
		long int m_PK_Package_Directory;
long int m_FK_Package;
long int m_FK_Directory;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
string m_Path;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Package_Directory_get();
long int FK_Package_get();
long int FK_Directory_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
string Path_get();

		
		void PK_Package_Directory_set(long int val);
void FK_Package_set(long int val);
void FK_Directory_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void Path_set(string val);

		
		bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();

			
		void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_Directory(Table_Package_Directory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Directory *Table_Package_Directory_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();
class Row_Directory* FK_Directory_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Device_Package_Directory_FK_Package_Directory_getrows(vector <class Row_Device_Package_Directory*> *rows);
void Package_Directory_File_FK_Package_Directory_getrows(vector <class Row_Package_Directory_File*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Package_Directory+ m_FK_Package+ m_FK_Directory+ m_FK_OperatingSystem+ m_FK_Distro+ m_Path;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_Directory_asSQL();
string FK_Package_asSQL();
string FK_Directory_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string Path_asSQL();

	};

#endif

