#ifndef __Table_Event_H__
#define __Table_Event_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Event.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Event : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Event(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Event")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Event();

private:		
	friend class Row_Event;
	struct Key
	{
		friend class Row_Event;
		long int pk_PK_Event;

		
		Key(long int in_PK_Event);
	
		Key(class Row_Event *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Event::Key &key1, const Table_Event::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Event*> *rows);
	class Row_Event* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Event* GetRow(long int in_PK_Event);
	

private:	
	
		
	class Row_Event* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Event : public TableRow, public SerializeClass
	{
		friend struct Table_Event::Key;
		friend class Table_Event;
	private:
		Table_Event *table;
		
		long int m_PK_Event;
long int m_FK_EventCategory;
string m_Description;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Event_get();
long int FK_EventCategory_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Event_set(long int val);
void FK_EventCategory_set(long int val);
void Description_set(string val);
void Define_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Description_isNull();
bool Define_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Description_setNull(bool val);
void Define_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Event(Table_Event *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Event *Table_Event_get() { return table; };

		// Return the rows for foreign keys 
		class Row_EventCategory* FK_EventCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CannedEvents_FK_Event_getrows(vector <class Row_CannedEvents*> *rows);
void DeviceCategory_Event_FK_Event_getrows(vector <class Row_DeviceCategory_Event*> *rows);
void DeviceTemplate_Event_FK_Event_getrows(vector <class Row_DeviceTemplate_Event*> *rows);
void Device_HouseMode_FK_Event_getrows(vector <class Row_Device_HouseMode*> *rows);
void EventHandler_FK_Event_getrows(vector <class Row_EventHandler*> *rows);
void Event_EventParameter_FK_Event_getrows(vector <class Row_Event_EventParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Event+ m_FK_EventCategory+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Event_asSQL();
string FK_EventCategory_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

