#ifndef __Table_CriteriaParmList_H__
#define __Table_CriteriaParmList_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaParmList.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CriteriaParmList
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CriteriaParmList(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CriteriaParmList")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CriteriaParmList();

private:		
	friend class Row_CriteriaParmList;
	struct Key
	{
		friend class Row_CriteriaParmList;
		long int pk_PK_CriteriaParmList;

		
		Key(long int in_PK_CriteriaParmList);
	
		Key(class Row_CriteriaParmList *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaParmList::Key &key1, const Table_CriteriaParmList::Key &key2) const;
	};	

	map<Table_CriteriaParmList::Key, class Row_CriteriaParmList*, Table_CriteriaParmList::Key_Less> cachedRows;
	map<Table_CriteriaParmList::Key, class Row_CriteriaParmList*, Table_CriteriaParmList::Key_Less> deleted_cachedRows;
	vector<class Row_CriteriaParmList*> addedRows;
	vector<class Row_CriteriaParmList*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CriteriaParmList*> *rows);
	class Row_CriteriaParmList* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaParmList* GetRow(long int in_PK_CriteriaParmList);
	

private:	
	
		
	class Row_CriteriaParmList* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CriteriaParmList : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaParmList::Key;
		friend class Table_CriteriaParmList;
	private:
		Table_CriteriaParmList *table;
		
		long int m_PK_CriteriaParmList;
long int m_FK_ParameterType;
string m_Description;
string m_Define;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_CriteriaParmList_get();
long int FK_ParameterType_get();
string Description_get();
string Define_get();

		
		void PK_CriteriaParmList_set(long int val);
void FK_ParameterType_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_CriteriaParmList(Table_CriteriaParmList *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaParmList *Table_CriteriaParmList_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ParameterType* FK_ParameterType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CannedEvents_CriteriaParmList_FK_CriteriaParmList_getrows(vector <class Row_CannedEvents_CriteriaParmList*> *rows);
void CriteriaList_CriteriaParmList_FK_CriteriaParmList_getrows(vector <class Row_CriteriaList_CriteriaParmList*> *rows);
void CriteriaParm_FK_CriteriaParmList_getrows(vector <class Row_CriteriaParm*> *rows);
void CriteriaParm_D_FK_CriteriaParmList_getrows(vector <class Row_CriteriaParm_D*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CriteriaParmList+ m_FK_ParameterType+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_CriteriaParmList_asSQL();
string FK_ParameterType_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

