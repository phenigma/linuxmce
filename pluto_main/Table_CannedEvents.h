#ifndef __Table_CannedEvents_H__
#define __Table_CannedEvents_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CannedEvents.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CannedEvents
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CannedEvents(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CannedEvents")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CannedEvents();

private:		
	friend class Row_CannedEvents;
	struct Key
	{
		friend class Row_CannedEvents;
		long int pk_PK_CannedEvents;

		
		Key(long int in_PK_CannedEvents);
	
		Key(class Row_CannedEvents *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CannedEvents::Key &key1, const Table_CannedEvents::Key &key2) const;
	};	

	map<Table_CannedEvents::Key, class Row_CannedEvents*, Table_CannedEvents::Key_Less> cachedRows;
	map<Table_CannedEvents::Key, class Row_CannedEvents*, Table_CannedEvents::Key_Less> deleted_cachedRows;
	vector<class Row_CannedEvents*> addedRows;
	vector<class Row_CannedEvents*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CannedEvents*> *rows);
	class Row_CannedEvents* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CannedEvents* GetRow(long int in_PK_CannedEvents);
	

private:	
	
		
	class Row_CannedEvents* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CannedEvents : public TableRow, public SerializeClass
	{
		friend struct Table_CannedEvents::Key;
		friend class Table_CannedEvents;
	private:
		Table_CannedEvents *table;
		
		long int m_PK_CannedEvents;
string m_Description;
long int m_FK_Event;
short int m_bIsAnd;
short int m_bIsNot;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_CannedEvents_get();
string Description_get();
long int FK_Event_get();
short int bIsAnd_get();
short int bIsNot_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_CannedEvents_set(long int val);
void Description_set(string val);
void FK_Event_set(long int val);
void bIsAnd_set(short int val);
void bIsNot_set(short int val);
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
	
		Row_CannedEvents(Table_CannedEvents *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CannedEvents *Table_CannedEvents_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Event* FK_Event_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CannedEvents_CriteriaParmList_FK_CannedEvents_getrows(vector <class Row_CannedEvents_CriteriaParmList*> *rows);
void EventHandler_FK_CannedEvents_getrows(vector <class Row_EventHandler*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CannedEvents+ m_Description+ m_FK_Event+ m_bIsAnd+ m_bIsNot+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_CannedEvents_asSQL();
string Description_asSQL();
string FK_Event_asSQL();
string bIsAnd_asSQL();
string bIsNot_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

