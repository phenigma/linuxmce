#ifndef __Table_CriteriaList_CriteriaParmList_H__
#define __Table_CriteriaList_CriteriaParmList_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaList_CriteriaParmList.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CriteriaList_CriteriaParmList
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CriteriaList_CriteriaParmList(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CriteriaList_CriteriaParmList")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CriteriaList_CriteriaParmList();

private:		
	friend class Row_CriteriaList_CriteriaParmList;
	struct Key
	{
		friend class Row_CriteriaList_CriteriaParmList;
		long int pk_FK_CriteriaList;
long int pk_FK_CriteriaParmList;

		
		Key(long int in_FK_CriteriaList, long int in_FK_CriteriaParmList);
	
		Key(class Row_CriteriaList_CriteriaParmList *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaList_CriteriaParmList::Key &key1, const Table_CriteriaList_CriteriaParmList::Key &key2) const;
	};	

	map<Table_CriteriaList_CriteriaParmList::Key, class Row_CriteriaList_CriteriaParmList*, Table_CriteriaList_CriteriaParmList::Key_Less> cachedRows;
	map<Table_CriteriaList_CriteriaParmList::Key, class Row_CriteriaList_CriteriaParmList*, Table_CriteriaList_CriteriaParmList::Key_Less> deleted_cachedRows;
	vector<class Row_CriteriaList_CriteriaParmList*> addedRows;
	vector<class Row_CriteriaList_CriteriaParmList*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CriteriaList_CriteriaParmList*> *rows);
	class Row_CriteriaList_CriteriaParmList* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaList_CriteriaParmList* GetRow(long int in_FK_CriteriaList, long int in_FK_CriteriaParmList);
	

private:	
	
		
	class Row_CriteriaList_CriteriaParmList* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CriteriaList_CriteriaParmList : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaList_CriteriaParmList::Key;
		friend class Table_CriteriaList_CriteriaParmList;
	private:
		Table_CriteriaList_CriteriaParmList *table;
		
		long int m_FK_CriteriaList;
long int m_FK_CriteriaParmList;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_CriteriaList_get();
long int FK_CriteriaParmList_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_CriteriaList_set(long int val);
void FK_CriteriaParmList_set(long int val);
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
	
		Row_CriteriaList_CriteriaParmList(Table_CriteriaList_CriteriaParmList *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaList_CriteriaParmList *Table_CriteriaList_CriteriaParmList_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaList* FK_CriteriaList_getrow();
class Row_CriteriaParmList* FK_CriteriaParmList_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_CriteriaList+ m_FK_CriteriaParmList+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_CriteriaList_asSQL();
string FK_CriteriaParmList_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

