#ifndef __Table_EventHandler_H__
#define __Table_EventHandler_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EventHandler.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_EventHandler
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_EventHandler(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("EventHandler")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_EventHandler();

private:		
	friend class Row_EventHandler;
	struct Key
	{
		friend class Row_EventHandler;
		long int pk_PK_EventHandler;

		
		Key(long int in_PK_EventHandler);
	
		Key(class Row_EventHandler *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EventHandler::Key &key1, const Table_EventHandler::Key &key2) const;
	};	

	map<Table_EventHandler::Key, class Row_EventHandler*, Table_EventHandler::Key_Less> cachedRows;
	map<Table_EventHandler::Key, class Row_EventHandler*, Table_EventHandler::Key_Less> deleted_cachedRows;
	vector<class Row_EventHandler*> addedRows;
	vector<class Row_EventHandler*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_EventHandler*> *rows);
	class Row_EventHandler* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventHandler* GetRow(long int in_PK_EventHandler);
	

private:	
	
		
	class Row_EventHandler* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_EventHandler : public TableRow, public SerializeClass
	{
		friend struct Table_EventHandler::Key;
		friend class Table_EventHandler;
	private:
		Table_EventHandler *table;
		
		long int m_PK_EventHandler;
long int m_FK_Event;
long int m_FK_EventType;
string m_Description;
long int m_FK_Criteria;
long int m_FK_Installation;
long int m_FK_CommandGroup;
short int m_UserCreated;
long int m_FK_CannedEvents;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[13];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_EventHandler_get();
long int FK_Event_get();
long int FK_EventType_get();
string Description_get();
long int FK_Criteria_get();
long int FK_Installation_get();
long int FK_CommandGroup_get();
short int UserCreated_get();
long int FK_CannedEvents_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_EventHandler_set(long int val);
void FK_Event_set(long int val);
void FK_EventType_set(long int val);
void Description_set(string val);
void FK_Criteria_set(long int val);
void FK_Installation_set(long int val);
void FK_CommandGroup_set(long int val);
void UserCreated_set(short int val);
void FK_CannedEvents_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_EventType_isNull();
bool FK_Criteria_isNull();
bool FK_Installation_isNull();
bool FK_CommandGroup_isNull();
bool FK_CannedEvents_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_EventType_setNull(bool val);
void FK_Criteria_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_CommandGroup_setNull(bool val);
void FK_CannedEvents_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EventHandler(Table_EventHandler *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EventHandler *Table_EventHandler_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Event* FK_Event_getrow();
class Row_Criteria* FK_Criteria_getrow();
class Row_Installation* FK_Installation_getrow();
class Row_CommandGroup* FK_CommandGroup_getrow();
class Row_CannedEvents* FK_CannedEvents_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_EventHandler+ m_FK_Event+ m_FK_EventType+ m_Description+ m_FK_Criteria+ m_FK_Installation+ m_FK_CommandGroup+ m_UserCreated+ m_FK_CannedEvents+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_EventHandler_asSQL();
string FK_Event_asSQL();
string FK_EventType_asSQL();
string Description_asSQL();
string FK_Criteria_asSQL();
string FK_Installation_asSQL();
string FK_CommandGroup_asSQL();
string UserCreated_asSQL();
string FK_CannedEvents_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

