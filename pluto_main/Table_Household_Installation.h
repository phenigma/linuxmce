#ifndef __Table_Household_Installation_H__
#define __Table_Household_Installation_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Household_Installation.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Household_Installation : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Household_Installation(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Household_Installation")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Household_Installation();

private:		
	friend class Row_Household_Installation;
	struct Key
	{
		friend class Row_Household_Installation;
		long int pk_FK_Household;
long int pk_FK_Installation;

		
		Key(long int in_FK_Household, long int in_FK_Installation);
	
		Key(class Row_Household_Installation *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Household_Installation::Key &key1, const Table_Household_Installation::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Household_Installation*> *rows);
	class Row_Household_Installation* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Household_Installation* GetRow(long int in_FK_Household, long int in_FK_Installation);
	

private:	
	
		
	class Row_Household_Installation* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_Household_Installation : public TableRow, public SerializeClass
	{
		friend struct Table_Household_Installation::Key;
		friend class Table_Household_Installation;
	private:
		Table_Household_Installation *table;
		
		long int m_FK_Household;
long int m_FK_Installation;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int FK_Household_get();
long int FK_Installation_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Household_set(long int val);
void FK_Installation_set(long int val);
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
	
		Row_Household_Installation(Table_Household_Installation *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Household_Installation *Table_Household_Installation_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Household* FK_Household_getrow();
class Row_Installation* FK_Installation_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Household+ m_FK_Installation+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Household_asSQL();
string FK_Installation_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

