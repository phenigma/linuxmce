#ifndef __Table_RepositorySource_H__
#define __Table_RepositorySource_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RepositorySource.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_RepositorySource
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_RepositorySource(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("RepositorySource")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_RepositorySource();

private:		
	friend class Row_RepositorySource;
	struct Key
	{
		friend class Row_RepositorySource;
		long int pk_PK_RepositorySource;

		
		Key(long int in_PK_RepositorySource);
	
		Key(class Row_RepositorySource *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RepositorySource::Key &key1, const Table_RepositorySource::Key &key2) const;
	};	

	map<Table_RepositorySource::Key, class Row_RepositorySource*, Table_RepositorySource::Key_Less> cachedRows;
	map<Table_RepositorySource::Key, class Row_RepositorySource*, Table_RepositorySource::Key_Less> deleted_cachedRows;
	vector<class Row_RepositorySource*> addedRows;
	vector<class Row_RepositorySource*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_RepositorySource*> *rows);
	class Row_RepositorySource* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RepositorySource* GetRow(long int in_PK_RepositorySource);
	

private:	
	
		
	class Row_RepositorySource* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_RepositorySource : public TableRow, public SerializeClass
	{
		friend struct Table_RepositorySource::Key;
		friend class Table_RepositorySource;
	private:
		Table_RepositorySource *table;
		
		long int m_PK_RepositorySource;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
long int m_FK_RepositoryType;
string m_Description;
string m_Define;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_RepositorySource_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
long int FK_RepositoryType_get();
string Description_get();
string Define_get();

		
		void PK_RepositorySource_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void FK_RepositoryType_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool Define_isNull();

			
		void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RepositorySource(Table_RepositorySource *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RepositorySource *Table_RepositorySource_get() { return table; };

		// Return the rows for foreign keys 
		class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();
class Row_RepositoryType* FK_RepositoryType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Package_Source_FK_RepositorySource_getrows(vector <class Row_Package_Source*> *rows);
void RepositorySource_URL_FK_RepositorySource_getrows(vector <class Row_RepositorySource_URL*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_RepositorySource+ m_FK_OperatingSystem+ m_FK_Distro+ m_FK_RepositoryType+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_RepositorySource_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string FK_RepositoryType_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

