#ifndef __Table_Installation_RepositorySource_URL_H__
#define __Table_Installation_RepositorySource_URL_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Installation_RepositorySource_URL.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Installation_RepositorySource_URL
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Installation_RepositorySource_URL(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Installation_RepositorySource_URL")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Installation_RepositorySource_URL();

private:		
	friend class Row_Installation_RepositorySource_URL;
	struct Key
	{
		friend class Row_Installation_RepositorySource_URL;
		long int pk_FK_Installation;
long int pk_FK_RepositorySource_URL;

		
		Key(long int in_FK_Installation, long int in_FK_RepositorySource_URL);
	
		Key(class Row_Installation_RepositorySource_URL *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Installation_RepositorySource_URL::Key &key1, const Table_Installation_RepositorySource_URL::Key &key2) const;
	};	

	map<Table_Installation_RepositorySource_URL::Key, class Row_Installation_RepositorySource_URL*, Table_Installation_RepositorySource_URL::Key_Less> cachedRows;
	map<Table_Installation_RepositorySource_URL::Key, class Row_Installation_RepositorySource_URL*, Table_Installation_RepositorySource_URL::Key_Less> deleted_cachedRows;
	vector<class Row_Installation_RepositorySource_URL*> addedRows;
	vector<class Row_Installation_RepositorySource_URL*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Installation_RepositorySource_URL*> *rows);
	class Row_Installation_RepositorySource_URL* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Installation_RepositorySource_URL* GetRow(long int in_FK_Installation, long int in_FK_RepositorySource_URL);
	

private:	
	
		
	class Row_Installation_RepositorySource_URL* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Installation_RepositorySource_URL : public TableRow, public SerializeClass
	{
		friend struct Table_Installation_RepositorySource_URL::Key;
		friend class Table_Installation_RepositorySource_URL;
	private:
		Table_Installation_RepositorySource_URL *table;
		
		long int m_FK_Installation;
long int m_FK_RepositorySource_URL;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Installation_get();
long int FK_RepositorySource_URL_get();

		
		void FK_Installation_set(long int val);
void FK_RepositorySource_URL_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Installation_RepositorySource_URL(Table_Installation_RepositorySource_URL *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Installation_RepositorySource_URL *Table_Installation_RepositorySource_URL_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Installation* FK_Installation_getrow();
class Row_RepositorySource_URL* FK_RepositorySource_URL_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Installation+ m_FK_RepositorySource_URL;
		}
	private:
		void SetDefaultValues();
		
		string FK_Installation_asSQL();
string FK_RepositorySource_URL_asSQL();

	};

#endif

