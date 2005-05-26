#ifndef __Table_CannedEvents_H__
#define __Table_CannedEvents_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CannedEvents.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CannedEvents : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CannedEvents(Database_pluto_main *pDatabase):database(pDatabase)
	{
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

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_CannedEvents*> *rows);
	class Row_CannedEvents* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CannedEvents* GetRow(long int in_PK_CannedEvents);
	

private:	
	
		
	class Row_CannedEvents* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CannedEvents : public TableRow, public SerializeClass
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
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_CannedEvents_get();
string Description_get();
long int FK_Event_get();
short int bIsAnd_get();
short int bIsNot_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_CannedEvents_set(long int val);
void Description_set(string val);
void FK_Event_set(long int val);
void bIsAnd_set(short int val);
void bIsNot_set(short int val);
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
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CannedEvents+ m_Description+ m_FK_Event+ m_bIsAnd+ m_bIsNot+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_CannedEvents_asSQL();
string Description_asSQL();
string FK_Event_asSQL();
string bIsAnd_asSQL();
string bIsNot_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

