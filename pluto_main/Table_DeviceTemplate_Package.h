#ifndef __Table_DeviceTemplate_Package_H__
#define __Table_DeviceTemplate_Package_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_Package.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_Package
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_Package(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_Package")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_Package();

private:		
	friend class Row_DeviceTemplate_Package;
	struct Key
	{
		friend class Row_DeviceTemplate_Package;
		long int pk_FK_DeviceTemplate;
long int pk_FK_Package;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_Package);
	
		Key(class Row_DeviceTemplate_Package *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_Package::Key &key1, const Table_DeviceTemplate_Package::Key &key2) const;
	};	

	map<Table_DeviceTemplate_Package::Key, class Row_DeviceTemplate_Package*, Table_DeviceTemplate_Package::Key_Less> cachedRows;
	map<Table_DeviceTemplate_Package::Key, class Row_DeviceTemplate_Package*, Table_DeviceTemplate_Package::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_Package*> addedRows;
	vector<class Row_DeviceTemplate_Package*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_Package*> *rows);
	class Row_DeviceTemplate_Package* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_Package* GetRow(long int in_FK_DeviceTemplate, long int in_FK_Package);
	

private:	
	
		
	class Row_DeviceTemplate_Package* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_Package : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_Package::Key;
		friend class Table_DeviceTemplate_Package;
	private:
		Table_DeviceTemplate_Package *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_Package;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
short int m_DevelopmentOnly;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_Package_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
short int DevelopmentOnly_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_Package_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void DevelopmentOnly_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_Package(Table_DeviceTemplate_Package *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_Package *Table_DeviceTemplate_Package_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Package* FK_Package_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_Package+ m_FK_OperatingSystem+ m_FK_Distro+ m_DevelopmentOnly+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_Package_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string DevelopmentOnly_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

