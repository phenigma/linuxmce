#ifndef __Table_OperatingSystem_H__
#define __Table_OperatingSystem_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_OperatingSystem.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_OperatingSystem
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_OperatingSystem(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("OperatingSystem")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_OperatingSystem();

private:		
	friend class Row_OperatingSystem;
	struct Key
	{
		friend class Row_OperatingSystem;
		long int pk_PK_OperatingSystem;

		
		Key(long int in_PK_OperatingSystem);
	
		Key(class Row_OperatingSystem *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_OperatingSystem::Key &key1, const Table_OperatingSystem::Key &key2) const;
	};	

	map<Table_OperatingSystem::Key, class Row_OperatingSystem*, Table_OperatingSystem::Key_Less> cachedRows;
	map<Table_OperatingSystem::Key, class Row_OperatingSystem*, Table_OperatingSystem::Key_Less> deleted_cachedRows;
	vector<class Row_OperatingSystem*> addedRows;
	vector<class Row_OperatingSystem*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_OperatingSystem*> *rows);
	class Row_OperatingSystem* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_OperatingSystem* GetRow(long int in_PK_OperatingSystem);
	

private:	
	
		
	class Row_OperatingSystem* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_OperatingSystem : public TableRow, public SerializeClass
	{
		friend struct Table_OperatingSystem::Key;
		friend class Table_OperatingSystem;
	private:
		Table_OperatingSystem *table;
		
		long int m_PK_OperatingSystem;
string m_Description;
string m_Define;

		bool is_null[3];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_OperatingSystem_get();
string Description_get();
string Define_get();

		
		void PK_OperatingSystem_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_OperatingSystem(Table_OperatingSystem *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_OperatingSystem *Table_OperatingSystem_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Distro_FK_OperatingSystem_getrows(vector <class Row_Distro*> *rows);
void InstallWizard_Distro_FK_OperatingSystem_getrows(vector <class Row_InstallWizard_Distro*> *rows);
void Package_Directory_FK_OperatingSystem_getrows(vector <class Row_Package_Directory*> *rows);
void Package_Directory_File_FK_OperatingSystem_getrows(vector <class Row_Package_Directory_File*> *rows);
void Package_Source_Compat_FK_OperatingSystem_getrows(vector <class Row_Package_Source_Compat*> *rows);
void RepositorySource_FK_OperatingSystem_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_OperatingSystem+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_OperatingSystem_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

