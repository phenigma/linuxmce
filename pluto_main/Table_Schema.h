#ifndef __Table_Schema_H__
#define __Table_Schema_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Schema.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Schema : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Schema(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Schema")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Schema();

private:		
	friend class Row_Schema;
	struct Key
	{
		friend class Row_Schema;
		long int pk_PK_Schema;

		
		Key(long int in_PK_Schema);
	
		Key(class Row_Schema *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Schema::Key &key1, const Table_Schema::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Schema*> *rows);
	class Row_Schema* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Schema* GetRow(long int in_PK_Schema);
	

private:	
	
		
	class Row_Schema* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Schema : public TableRow, public SerializeClass
	{
		friend struct Table_Schema::Key;
		friend class Table_Schema;
	private:
		Table_Schema *table;
		
		long int m_PK_Schema;
long int m_FK_Version;
string m_SQLCommands;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_Schema_get();
long int FK_Version_get();
string SQLCommands_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Schema_set(long int val);
void FK_Version_set(long int val);
void SQLCommands_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Version_isNull();
bool SQLCommands_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Version_setNull(bool val);
void SQLCommands_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Schema(Table_Schema *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Schema *Table_Schema_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Version* FK_Version_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Schema+ m_FK_Version+ m_SQLCommands+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Schema_asSQL();
string FK_Version_asSQL();
string SQLCommands_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

