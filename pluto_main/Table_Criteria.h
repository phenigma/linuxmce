#ifndef __Table_Criteria_H__
#define __Table_Criteria_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Criteria.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Criteria
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Criteria(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Criteria")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Criteria();

private:		
	friend class Row_Criteria;
	struct Key
	{
		friend class Row_Criteria;
		long int pk_PK_Criteria;

		
		Key(long int in_PK_Criteria);
	
		Key(class Row_Criteria *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Criteria::Key &key1, const Table_Criteria::Key &key2) const;
	};	

	map<Table_Criteria::Key, class Row_Criteria*, Table_Criteria::Key_Less> cachedRows;
	map<Table_Criteria::Key, class Row_Criteria*, Table_Criteria::Key_Less> deleted_cachedRows;
	vector<class Row_Criteria*> addedRows;
	vector<class Row_Criteria*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Criteria*> *rows);
	class Row_Criteria* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Criteria* GetRow(long int in_PK_Criteria);
	

private:	
	
		
	class Row_Criteria* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Criteria : public TableRow, public SerializeClass
	{
		friend struct Table_Criteria::Key;
		friend class Table_Criteria;
	private:
		Table_Criteria *table;
		
		long int m_PK_Criteria;
long int m_FK_CriteriaParmNesting;
long int m_FK_CriteriaList;
string m_Description;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Criteria_get();
long int FK_CriteriaParmNesting_get();
long int FK_CriteriaList_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Criteria_set(long int val);
void FK_CriteriaParmNesting_set(long int val);
void FK_CriteriaList_set(long int val);
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
	
		Row_Criteria(Table_Criteria *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Criteria *Table_Criteria_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaParmNesting* FK_CriteriaParmNesting_getrow();
class Row_CriteriaList* FK_CriteriaList_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Criteria_Orbiter_getrows(vector <class Row_CommandGroup*> *rows);
void CommandGroup_D_FK_Criteria_Orbiter_getrows(vector <class Row_CommandGroup_D*> *rows);
void DesignObjVariation_FK_Criteria_Orbiter_getrows(vector <class Row_DesignObjVariation*> *rows);
void EventHandler_FK_Criteria_getrows(vector <class Row_EventHandler*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Criteria+ m_FK_CriteriaParmNesting+ m_FK_CriteriaList+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Criteria_asSQL();
string FK_CriteriaParmNesting_asSQL();
string FK_CriteriaList_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

