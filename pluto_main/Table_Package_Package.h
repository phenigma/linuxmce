#ifndef __Table_Package_Package_H__
#define __Table_Package_Package_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Package.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Package_Package : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Package_Package(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Package_Package")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Package_Package();

private:		
	friend class Row_Package_Package;
	struct Key
	{
		friend class Row_Package_Package;
		long int pk_FK_Package;
long int pk_FK_Package_DependsOn;

		
		Key(long int in_FK_Package, long int in_FK_Package_DependsOn);
	
		Key(class Row_Package_Package *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Package::Key &key1, const Table_Package_Package::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Package_Package*> *rows);
	class Row_Package_Package* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Package* GetRow(long int in_FK_Package, long int in_FK_Package_DependsOn);
	

private:	
	
		
	class Row_Package_Package* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_Package_Package : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Package::Key;
		friend class Table_Package_Package;
	private:
		Table_Package_Package *table;
		
		long int m_FK_Package;
long int m_FK_Package_DependsOn;
short int m_OnlyToBuild;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int FK_Package_get();
long int FK_Package_DependsOn_get();
short int OnlyToBuild_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Package_set(long int val);
void FK_Package_DependsOn_set(long int val);
void OnlyToBuild_set(short int val);
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
	
		Row_Package_Package(Table_Package_Package *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Package *Table_Package_Package_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();
class Row_Package* FK_Package_DependsOn_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Package+ m_FK_Package_DependsOn+ m_OnlyToBuild+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Package_asSQL();
string FK_Package_DependsOn_asSQL();
string OnlyToBuild_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

