#ifndef __Table_Orbiter_H__
#define __Table_Orbiter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Orbiter : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Orbiter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Orbiter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Orbiter();

private:		
	friend class Row_Orbiter;
	struct Key
	{
		friend class Row_Orbiter;
		long int pk_PK_Orbiter;

		
		Key(long int in_PK_Orbiter);
	
		Key(class Row_Orbiter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter::Key &key1, const Table_Orbiter::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Orbiter*> *rows);
	class Row_Orbiter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter* GetRow(long int in_PK_Orbiter);
	

private:	
	
		
	class Row_Orbiter* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Orbiter : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter::Key;
		friend class Table_Orbiter;
	private:
		Table_Orbiter *table;
		
		long int m_PK_Orbiter;
string m_FloorplanInfo;
string m_Modification_LastGen;
short int m_Regen;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Orbiter_get();
string FloorplanInfo_get();
string Modification_LastGen_get();
short int Regen_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Orbiter_set(long int val);
void FloorplanInfo_set(string val);
void Modification_LastGen_set(string val);
void Regen_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FloorplanInfo_isNull();
bool Modification_LastGen_isNull();
bool Regen_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FloorplanInfo_setNull(bool val);
void Modification_LastGen_setNull(bool val);
void Regen_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Orbiter(Table_Orbiter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter *Table_Orbiter_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CachedScreens_FK_Orbiter_getrows(vector <class Row_CachedScreens*> *rows);
void Device_Orbiter_FK_Orbiter_getrows(vector <class Row_Device_Orbiter*> *rows);
void Orbiter_Users_PasswordReq_FK_Orbiter_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows);
void Orbiter_Variable_FK_Orbiter_getrows(vector <class Row_Orbiter_Variable*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Orbiter+ m_FloorplanInfo+ m_Modification_LastGen+ m_Regen+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Orbiter_asSQL();
string FloorplanInfo_asSQL();
string Modification_LastGen_asSQL();
string Regen_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

