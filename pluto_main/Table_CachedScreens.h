#ifndef __Table_CachedScreens_H__
#define __Table_CachedScreens_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CachedScreens.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CachedScreens
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CachedScreens(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CachedScreens")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CachedScreens();

private:		
	friend class Row_CachedScreens;
	struct Key
	{
		friend class Row_CachedScreens;
		long int pk_FK_Orbiter;
long int pk_FK_DesignObj;
long int pk_Version;

		
		Key(long int in_FK_Orbiter, long int in_FK_DesignObj, long int in_Version);
	
		Key(class Row_CachedScreens *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CachedScreens::Key &key1, const Table_CachedScreens::Key &key2) const;
	};	

	map<Table_CachedScreens::Key, class Row_CachedScreens*, Table_CachedScreens::Key_Less> cachedRows;
	map<Table_CachedScreens::Key, class Row_CachedScreens*, Table_CachedScreens::Key_Less> deleted_cachedRows;
	vector<class Row_CachedScreens*> addedRows;
	vector<class Row_CachedScreens*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CachedScreens*> *rows);
	class Row_CachedScreens* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CachedScreens* GetRow(long int in_FK_Orbiter, long int in_FK_DesignObj, long int in_Version);
	

private:	
	
		
	class Row_CachedScreens* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CachedScreens : public TableRow, public SerializeClass
	{
		friend struct Table_CachedScreens::Key;
		friend class Table_CachedScreens;
	private:
		Table_CachedScreens *table;
		
		long int m_FK_Orbiter;
long int m_FK_DesignObj;
long int m_Version;
string m_Modification_LastGen;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Orbiter_get();
long int FK_DesignObj_get();
long int Version_get();
string Modification_LastGen_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Orbiter_set(long int val);
void FK_DesignObj_set(long int val);
void Version_set(long int val);
void Modification_LastGen_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CachedScreens(Table_CachedScreens *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CachedScreens *Table_CachedScreens_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Orbiter* FK_Orbiter_getrow();
class Row_DesignObj* FK_DesignObj_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Orbiter+ m_FK_DesignObj+ m_Version+ m_Modification_LastGen+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Orbiter_asSQL();
string FK_DesignObj_asSQL();
string Version_asSQL();
string Modification_LastGen_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

