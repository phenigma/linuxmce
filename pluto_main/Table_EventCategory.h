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

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_EventCategory : public TableBase , SingleLongKeyBase
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

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_EventCategory*> *rows);
	class Row_EventCategory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventCategory* GetRow(long int in_PK_EventCategory);
	

private:	
	
		
	class Row_EventCategory* FetchRow(SingleLongKey &key);
		
			
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
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_EventCategory_get();
long int FK_EventCategory_Parent_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_EventCategory_set(long int val);
void FK_EventCategory_Parent_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_EventCategory_Parent_isNull();
bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_EventCategory_Parent_setNull(bool val);
void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
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
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_EventCategory+ m_FK_EventCategory_Parent+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_EventCategory_asSQL();
string FK_EventCategory_Parent_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

