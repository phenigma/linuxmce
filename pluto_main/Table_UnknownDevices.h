#ifndef __Table_UnknownDevices_H__
#define __Table_UnknownDevices_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_UnknownDevices.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_UnknownDevices : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_UnknownDevices(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("UnknownDevices")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_UnknownDevices();

private:		
	friend class Row_UnknownDevices;
	struct Key
	{
		friend class Row_UnknownDevices;
		long int pk_PK_UnknownDevices;

		
		Key(long int in_PK_UnknownDevices);
	
		Key(class Row_UnknownDevices *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_UnknownDevices::Key &key1, const Table_UnknownDevices::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_UnknownDevices*> *rows);
	class Row_UnknownDevices* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_UnknownDevices* GetRow(long int in_PK_UnknownDevices);
	

private:	
	
		
	class Row_UnknownDevices* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_UnknownDevices : public TableRow, public SerializeClass
	{
		friend struct Table_UnknownDevices::Key;
		friend class Table_UnknownDevices;
	private:
		Table_UnknownDevices *table;
		
		long int m_PK_UnknownDevices;
string m_Description;
string m_MacAddress;
string m_IPAddress;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_UnknownDevices_get();
string Description_get();
string MacAddress_get();
string IPAddress_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_UnknownDevices_set(long int val);
void Description_set(string val);
void MacAddress_set(string val);
void IPAddress_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Description_isNull();
bool MacAddress_isNull();
bool IPAddress_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Description_setNull(bool val);
void MacAddress_setNull(bool val);
void IPAddress_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_UnknownDevices(Table_UnknownDevices *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_UnknownDevices *Table_UnknownDevices_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_UnknownDevices+ m_Description+ m_MacAddress+ m_IPAddress+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_UnknownDevices_asSQL();
string Description_asSQL();
string MacAddress_asSQL();
string IPAddress_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

