#ifndef __Table_RepositoryType_H__
#define __Table_RepositoryType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RepositoryType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_RepositoryType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_RepositoryType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("RepositoryType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_RepositoryType();

private:		
	friend class Row_RepositoryType;
	struct Key
	{
		friend class Row_RepositoryType;
		long int pk_PK_RepositoryType;

		
		Key(long int in_PK_RepositoryType);
	
		Key(class Row_RepositoryType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RepositoryType::Key &key1, const Table_RepositoryType::Key &key2) const;
	};	

	map<Table_RepositoryType::Key, class Row_RepositoryType*, Table_RepositoryType::Key_Less> cachedRows;
	map<Table_RepositoryType::Key, class Row_RepositoryType*, Table_RepositoryType::Key_Less> deleted_cachedRows;
	vector<class Row_RepositoryType*> addedRows;
	vector<class Row_RepositoryType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_RepositoryType*> *rows);
	class Row_RepositoryType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RepositoryType* GetRow(long int in_PK_RepositoryType);
	

private:	
	
		
	class Row_RepositoryType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_RepositoryType : public TableRow, public SerializeClass
	{
		friend struct Table_RepositoryType::Key;
		friend class Table_RepositoryType;
	private:
		Table_RepositoryType *table;
		
		long int m_PK_RepositoryType;
string m_Description;
string m_Define;

		bool is_null[3];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_RepositoryType_get();
string Description_get();
string Define_get();

		
		void PK_RepositoryType_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RepositoryType(Table_RepositoryType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RepositoryType *Table_RepositoryType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void RepositorySource_FK_RepositoryType_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_RepositoryType+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_RepositoryType_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

