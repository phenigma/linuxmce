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

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_EventHandler : public TableBase , SingleLongKeyBase
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

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_EventHandler*> *rows);
	class Row_EventHandler* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventHandler* GetRow(long int in_PK_EventHandler);
	

private:	
	
		
	class Row_EventHandler* FetchRow(SingleLongKey &key);
		
			
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
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[14];
	
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
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_EventHandler_set(long int val);
void FK_Event_set(long int val);
void FK_EventType_set(long int val);
void Description_set(string val);
void FK_Criteria_set(long int val);
void FK_Installation_set(long int val);
void FK_CommandGroup_set(long int val);
void UserCreated_set(short int val);
void FK_CannedEvents_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_EventType_isNull();
bool FK_Criteria_isNull();
bool FK_Installation_isNull();
bool FK_CommandGroup_isNull();
bool FK_CannedEvents_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_EventType_setNull(bool val);
void FK_Criteria_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_CommandGroup_setNull(bool val);
void FK_CannedEvents_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
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
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_EventHandler+ m_FK_Event+ m_FK_EventType+ m_Description+ m_FK_Criteria+ m_FK_Installation+ m_FK_CommandGroup+ m_UserCreated+ m_FK_CannedEvents+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
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
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

