#ifndef __Table_DeviceCategory_Event_H__
#define __Table_DeviceCategory_Event_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCategory_Event.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceCategory_Event : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCategory_Event(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCategory_Event")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCategory_Event();

private:		
	friend class Row_DeviceCategory_Event;
	struct Key
	{
		friend class Row_DeviceCategory_Event;
		long int pk_FK_DeviceCategory;
long int pk_FK_Event;

		
		Key(long int in_FK_DeviceCategory, long int in_FK_Event);
	
		Key(class Row_DeviceCategory_Event *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCategory_Event::Key &key1, const Table_DeviceCategory_Event::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCategory_Event*> *rows);
	class Row_DeviceCategory_Event* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCategory_Event* GetRow(long int in_FK_DeviceCategory, long int in_FK_Event);
	

private:	
	
		
	class Row_DeviceCategory_Event* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceCategory_Event : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCategory_Event::Key;
		friend class Table_DeviceCategory_Event;
	private:
		Table_DeviceCategory_Event *table;
		
		long int m_FK_DeviceCategory;
long int m_FK_Event;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int FK_DeviceCategory_get();
long int FK_Event_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DeviceCategory_set(long int val);
void FK_Event_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceCategory_Event(Table_DeviceCategory_Event *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCategory_Event *Table_DeviceCategory_Event_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Event* FK_Event_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceCategory+ m_FK_Event+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceCategory_asSQL();
string FK_Event_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

