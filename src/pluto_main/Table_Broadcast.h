#ifndef __Table_Broadcast_H__
#define __Table_Broadcast_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Broadcast.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Broadcast : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Broadcast(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Broadcast")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Broadcast();

private:		
	friend class Row_Broadcast;
	struct Key
	{
		friend class Row_Broadcast;
		long int pk_PK_Broadcast;

		
		Key(long int in_PK_Broadcast);
	
		Key(class Row_Broadcast *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Broadcast::Key &key1, const Table_Broadcast::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Broadcast*> *rows);
	class Row_Broadcast* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Broadcast* GetRow(long int in_PK_Broadcast);
	

private:	
	
		
	class Row_Broadcast* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Broadcast : public TableRow, public SerializeClass
	{
		friend struct Table_Broadcast::Key;
		friend class Table_Broadcast;
	private:
		Table_Broadcast *table;
		
		long int m_PK_Broadcast;
string m_Description;
string m_Extensions;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_Broadcast_get();
string Description_get();
string Extensions_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Broadcast_set(long int val);
void Description_set(string val);
void Extensions_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Extensions_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Extensions_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Broadcast(Table_Broadcast *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Broadcast *Table_Broadcast_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void MediaType_Broadcast_FK_Broadcast_getrows(vector <class Row_MediaType_Broadcast*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Broadcast+ m_Description+ m_Extensions+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Broadcast_asSQL();
string Description_asSQL();
string Extensions_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

