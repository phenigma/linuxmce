#ifndef __Table_Manufacturer_H__
#define __Table_Manufacturer_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Manufacturer.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Manufacturer : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Manufacturer(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Manufacturer")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Manufacturer();

private:		
	friend class Row_Manufacturer;
	struct Key
	{
		friend class Row_Manufacturer;
		long int pk_PK_Manufacturer;

		
		Key(long int in_PK_Manufacturer);
	
		Key(class Row_Manufacturer *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Manufacturer::Key &key1, const Table_Manufacturer::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Manufacturer*> *rows);
	class Row_Manufacturer* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Manufacturer* GetRow(long int in_PK_Manufacturer);
	

private:	
	
		
	class Row_Manufacturer* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Manufacturer : public TableRow, public SerializeClass
	{
		friend struct Table_Manufacturer::Key;
		friend class Table_Manufacturer;
	private:
		Table_Manufacturer *table;
		
		long int m_PK_Manufacturer;
string m_Description;
long int m_IRFrequency;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_Manufacturer_get();
string Description_get();
long int IRFrequency_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Manufacturer_set(long int val);
void Description_set(string val);
void IRFrequency_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool IRFrequency_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void IRFrequency_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Manufacturer(Table_Manufacturer *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Manufacturer *Table_Manufacturer_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_Manufacturer_getrows(vector <class Row_DeviceTemplate*> *rows);
void InfraredGroup_FK_Manufacturer_getrows(vector <class Row_InfraredGroup*> *rows);
void Package_FK_Manufacturer_getrows(vector <class Row_Package*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Manufacturer+ m_Description+ m_IRFrequency+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Manufacturer_asSQL();
string Description_asSQL();
string IRFrequency_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

