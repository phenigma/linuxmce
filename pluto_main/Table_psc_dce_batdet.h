#ifndef __Table_psc_dce_batdet_H__
#define __Table_psc_dce_batdet_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_dce_batdet.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_dce_batdet : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_dce_batdet(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("psc_dce_batdet")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_dce_batdet();

private:		
	friend class Row_psc_dce_batdet;
	struct Key
	{
		friend class Row_psc_dce_batdet;
		long int pk_PK_psc_dce_batdet;

		
		Key(long int in_PK_psc_dce_batdet);
	
		Key(class Row_psc_dce_batdet *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_dce_batdet::Key &key1, const Table_psc_dce_batdet::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_psc_dce_batdet*> *rows);
	class Row_psc_dce_batdet* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_dce_batdet* GetRow(long int in_PK_psc_dce_batdet);
	

private:	
	
		
	class Row_psc_dce_batdet* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_dce_batdet : public TableRow, public SerializeClass
	{
		friend struct Table_psc_dce_batdet::Key;
		friend class Table_psc_dce_batdet;
	private:
		Table_psc_dce_batdet *table;
		
		long int m_PK_psc_dce_batdet;
string m_Value;

		bool is_null[2];
	
	public:
		long int PK_psc_dce_batdet_get();
string Value_get();

		
		void PK_psc_dce_batdet_set(long int val);
void Value_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_dce_batdet(Table_psc_dce_batdet *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_dce_batdet *Table_psc_dce_batdet_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_psc_dce_batdet+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_dce_batdet_asSQL();
string Value_asSQL();

	};

#endif

