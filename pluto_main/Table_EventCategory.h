#ifndef __Table_EventCategory_H__
#define __Table_EventCategory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EventCategory.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_EventCategory
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_EventCategory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("EventCategory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_EventCategory();

private:		
	friend class Row_EventCategory;
	struct Key
	{
		friend class Row_EventCategory;
		long int pk_PK_EventCategory;

		
		Key(long int in_PK_EventCategory);
	
		Key(class Row_EventCategory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EventCategory::Key &key1, const Table_EventCategory::Key &key2) const;
	};	

	map<Table_EventCategory::Key, class Row_EventCategory*, Table_EventCategory::Key_Less> cachedRows;
	map<Table_EventCategory::Key, class Row_EventCategory*, Table_EventCategory::Key_Less> deleted_cachedRows;
	vector<class Row_EventCategory*> addedRows;
	vector<class Row_EventCategory*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_EventCategory*> *rows);
	class Row_EventCategory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventCategory* GetRow(long int in_PK_EventCategory);
	

private:	
	
		
	class Row_EventCategory* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_EventCategory : public TableRow, public SerializeClass
	{
		friend struct Table_EventCategory::Key;
		friend class Table_EventCategory;
	private:
		Table_EventCategory *table;
		
		long int m_PK_EventCategory;
long int m_FK_EventCategory_Parent;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_EventCategory_get();
long int FK_EventCategory_Parent_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_EventCategory_set(long int val);
void FK_EventCategory_Parent_set(long int val);
void Description_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_EventCategory_Parent_isNull();
bool Description_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_EventCategory_Parent_setNull(bool val);
void Description_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EventCategory(Table_EventCategory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EventCategory *Table_EventCategory_get() { return table; };

		// Return the rows for foreign keys 
		class Row_EventCategory* FK_EventCategory_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Event_FK_EventCategory_getrows(vector <class Row_Event*> *rows);
void EventCategory_FK_EventCategory_Parent_getrows(vector <class Row_EventCategory*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_EventCategory+ m_FK_EventCategory_Parent+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_EventCategory_asSQL();
string FK_EventCategory_Parent_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

