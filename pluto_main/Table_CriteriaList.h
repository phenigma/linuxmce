#ifndef __Table_CriteriaList_H__
#define __Table_CriteriaList_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaList.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CriteriaList
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CriteriaList(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CriteriaList")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CriteriaList();

private:		
	friend class Row_CriteriaList;
	struct Key
	{
		friend class Row_CriteriaList;
		long int pk_PK_CriteriaList;

		
		Key(long int in_PK_CriteriaList);
	
		Key(class Row_CriteriaList *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaList::Key &key1, const Table_CriteriaList::Key &key2) const;
	};	

	map<Table_CriteriaList::Key, class Row_CriteriaList*, Table_CriteriaList::Key_Less> cachedRows;
	map<Table_CriteriaList::Key, class Row_CriteriaList*, Table_CriteriaList::Key_Less> deleted_cachedRows;
	vector<class Row_CriteriaList*> addedRows;
	vector<class Row_CriteriaList*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CriteriaList*> *rows);
	class Row_CriteriaList* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaList* GetRow(long int in_PK_CriteriaList);
	

private:	
	
		
	class Row_CriteriaList* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CriteriaList : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaList::Key;
		friend class Table_CriteriaList;
	private:
		Table_CriteriaList *table;
		
		long int m_PK_CriteriaList;
string m_Description;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_CriteriaList_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_CriteriaList_set(long int val);
void Description_set(string val);
void Define_set(string val);
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
	
		Row_CriteriaList(Table_CriteriaList *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaList *Table_CriteriaList_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Criteria_FK_CriteriaList_getrows(vector <class Row_Criteria*> *rows);
void CriteriaList_CriteriaParmList_FK_CriteriaList_getrows(vector <class Row_CriteriaList_CriteriaParmList*> *rows);
void Criteria_D_FK_CriteriaList_getrows(vector <class Row_Criteria_D*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CriteriaList+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_CriteriaList_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

