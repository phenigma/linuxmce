#ifndef __Table_Household_Installation_H__
#define __Table_Household_Installation_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Household_Installation.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Household_Installation
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Household_Installation(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Household_Installation")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Household_Installation();

private:		
	friend class Row_Household_Installation;
	struct Key
	{
		friend class Row_Household_Installation;
		long int pk_FK_Household;
long int pk_FK_Installation;

		
		Key(long int in_FK_Household, long int in_FK_Installation);
	
		Key(class Row_Household_Installation *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Household_Installation::Key &key1, const Table_Household_Installation::Key &key2) const;
	};	

	map<Table_Household_Installation::Key, class Row_Household_Installation*, Table_Household_Installation::Key_Less> cachedRows;
	map<Table_Household_Installation::Key, class Row_Household_Installation*, Table_Household_Installation::Key_Less> deleted_cachedRows;
	vector<class Row_Household_Installation*> addedRows;
	vector<class Row_Household_Installation*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Household_Installation*> *rows);
	class Row_Household_Installation* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Household_Installation* GetRow(long int in_FK_Household, long int in_FK_Installation);
	

private:	
	
		
	class Row_Household_Installation* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Household_Installation : public TableRow, public SerializeClass
	{
		friend struct Table_Household_Installation::Key;
		friend class Table_Household_Installation;
	private:
		Table_Household_Installation *table;
		
		long int m_FK_Household;
long int m_FK_Installation;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Household_get();
long int FK_Installation_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Household_set(long int val);
void FK_Installation_set(long int val);
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
	
		Row_Household_Installation(Table_Household_Installation *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Household_Installation *Table_Household_Installation_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Household* FK_Household_getrow();
class Row_Installation* FK_Installation_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Household+ m_FK_Installation+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Household_asSQL();
string FK_Installation_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

