#ifndef __Table_psc_media_tables_H__
#define __Table_psc_media_tables_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_media_tables.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_media_tables : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_media_tables(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("psc_media_tables")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_media_tables();

private:		
	friend class Row_psc_media_tables;
	struct Key
	{
		friend class Row_psc_media_tables;
		long int pk_PK_psc_media_tables;

		
		Key(long int in_PK_psc_media_tables);
	
		Key(class Row_psc_media_tables *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_media_tables::Key &key1, const Table_psc_media_tables::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_psc_media_tables*> *rows);
	class Row_psc_media_tables* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_psc_media_tables* GetRow(long int in_PK_psc_media_tables);
	

private:	
	
		
	class Row_psc_media_tables* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_media_tables : public TableRow, public SerializeClass
	{
		friend struct Table_psc_media_tables::Key;
		friend class Table_psc_media_tables;
	private:
		Table_psc_media_tables *table;
		
		long int m_PK_psc_media_tables;
string m_Tablename;
string m_filter;
short int m_frozen;
long int m_last_psc_id;
long int m_last_psc_batch;

		bool is_null[6];
	
	public:
		long int PK_psc_media_tables_get();
string Tablename_get();
string filter_get();
short int frozen_get();
long int last_psc_id_get();
long int last_psc_batch_get();

		
		void PK_psc_media_tables_set(long int val);
void Tablename_set(string val);
void filter_set(string val);
void frozen_set(short int val);
void last_psc_id_set(long int val);
void last_psc_batch_set(long int val);

		
		bool filter_isNull();
bool frozen_isNull();
bool last_psc_id_isNull();
bool last_psc_batch_isNull();

			
		void filter_setNull(bool val);
void frozen_setNull(bool val);
void last_psc_id_setNull(bool val);
void last_psc_batch_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_media_tables(Table_psc_media_tables *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_media_tables *Table_psc_media_tables_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_media_tables+ m_Tablename+ m_filter+ m_frozen+ m_last_psc_id+ m_last_psc_batch;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_media_tables_asSQL();
string Tablename_asSQL();
string filter_asSQL();
string frozen_asSQL();
string last_psc_id_asSQL();
string last_psc_batch_asSQL();

	};

#endif

