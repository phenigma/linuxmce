#ifndef __Table_Household_H__
#define __Table_Household_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Household.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Household
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Household(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Household")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Household();

private:		
	friend class Row_Household;
	struct Key
	{
		friend class Row_Household;
		long int pk_PK_Household;

		
		Key(long int in_PK_Household);
	
		Key(class Row_Household *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Household::Key &key1, const Table_Household::Key &key2) const;
	};	

	map<Table_Household::Key, class Row_Household*, Table_Household::Key_Less> cachedRows;
	map<Table_Household::Key, class Row_Household*, Table_Household::Key_Less> deleted_cachedRows;
	vector<class Row_Household*> addedRows;
	vector<class Row_Household*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Household*> *rows);
	class Row_Household* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Household* GetRow(long int in_PK_Household);
	

private:	
	
		
	class Row_Household* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Household : public TableRow, public SerializeClass
	{
		friend struct Table_Household::Key;
		friend class Table_Household;
	private:
		Table_Household *table;
		
		long int m_PK_Household;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[5];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Household_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Household_set(long int val);
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
	
		Row_Household(Table_Household *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Household *Table_Household_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Household_Installation_FK_Household_getrows(vector <class Row_Household_Installation*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Household+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Household_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

