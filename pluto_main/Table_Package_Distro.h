#ifndef __Table_Package_Distro_H__
#define __Table_Package_Distro_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Distro.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Package_Distro
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Package_Distro(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Package_Distro")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Package_Distro();

private:		
	friend class Row_Package_Distro;
	struct Key
	{
		friend class Row_Package_Distro;
		long int pk_FK_Package;
long int pk_FK_Distro;

		
		Key(long int in_FK_Package, long int in_FK_Distro);
	
		Key(class Row_Package_Distro *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Distro::Key &key1, const Table_Package_Distro::Key &key2) const;
	};	

	map<Table_Package_Distro::Key, class Row_Package_Distro*, Table_Package_Distro::Key_Less> cachedRows;
	map<Table_Package_Distro::Key, class Row_Package_Distro*, Table_Package_Distro::Key_Less> deleted_cachedRows;
	vector<class Row_Package_Distro*> addedRows;
	vector<class Row_Package_Distro*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Package_Distro*> *rows);
	class Row_Package_Distro* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Distro* GetRow(long int in_FK_Package, long int in_FK_Distro);
	

private:	
	
		
	class Row_Package_Distro* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Package_Distro : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Distro::Key;
		friend class Table_Package_Distro;
	private:
		Table_Package_Distro *table;
		
		long int m_FK_Package;
long int m_FK_Distro;
string m_Name;
long int m_FK_RepositorySource;
long int m_FK_RepositoryType;
string m_Repository;
string m_Version;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Package_get();
long int FK_Distro_get();
string Name_get();
long int FK_RepositorySource_get();
long int FK_RepositoryType_get();
string Repository_get();
string Version_get();

		
		void FK_Package_set(long int val);
void FK_Distro_set(long int val);
void Name_set(string val);
void FK_RepositorySource_set(long int val);
void FK_RepositoryType_set(long int val);
void Repository_set(string val);
void Version_set(string val);

		
		bool Name_isNull();
bool FK_RepositorySource_isNull();
bool FK_RepositoryType_isNull();
bool Repository_isNull();
bool Version_isNull();

			
		void Name_setNull(bool val);
void FK_RepositorySource_setNull(bool val);
void FK_RepositoryType_setNull(bool val);
void Repository_setNull(bool val);
void Version_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_Distro(Table_Package_Distro *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Distro *Table_Package_Distro_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();
class Row_Distro* FK_Distro_getrow();
class Row_RepositorySource* FK_RepositorySource_getrow();
class Row_RepositoryType* FK_RepositoryType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Package+ m_FK_Distro+ m_Name+ m_FK_RepositorySource+ m_FK_RepositoryType+ m_Repository+ m_Version;
		}
	private:
		void SetDefaultValues();
		
		string FK_Package_asSQL();
string FK_Distro_asSQL();
string Name_asSQL();
string FK_RepositorySource_asSQL();
string FK_RepositoryType_asSQL();
string Repository_asSQL();
string Version_asSQL();

	};

#endif

