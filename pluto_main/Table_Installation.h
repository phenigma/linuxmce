#ifndef __Table_Installation_H__
#define __Table_Installation_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Installation.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Installation : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Installation(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Installation")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Installation();

private:		
	friend class Row_Installation;
	struct Key
	{
		friend class Row_Installation;
		long int pk_PK_Installation;

		
		Key(long int in_PK_Installation);
	
		Key(class Row_Installation *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Installation::Key &key1, const Table_Installation::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Installation*> *rows);
	class Row_Installation* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Installation* GetRow(long int in_PK_Installation);
	

private:	
	
		
	class Row_Installation* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Installation : public TableRow, public SerializeClass
	{
		friend struct Table_Installation::Key;
		friend class Table_Installation;
	private:
		Table_Installation *table;
		
		long int m_PK_Installation;
string m_Description;
string m_Name;
string m_Address;
string m_City;
string m_State;
string m_Zip;
long int m_FK_Country;
string m_ActivationCode;
string m_LastStatus;
string m_LastAlive;
short int m_isActive;
long int m_FK_Version;
short int m_isMonitored;
long int m_FK_RepositoryType_Source;
long int m_FK_RepositoryType_Binaries;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[21];
	
	public:
		long int PK_Installation_get();
string Description_get();
string Name_get();
string Address_get();
string City_get();
string State_get();
string Zip_get();
long int FK_Country_get();
string ActivationCode_get();
string LastStatus_get();
string LastAlive_get();
short int isActive_get();
long int FK_Version_get();
short int isMonitored_get();
long int FK_RepositoryType_Source_get();
long int FK_RepositoryType_Binaries_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Installation_set(long int val);
void Description_set(string val);
void Name_set(string val);
void Address_set(string val);
void City_set(string val);
void State_set(string val);
void Zip_set(string val);
void FK_Country_set(long int val);
void ActivationCode_set(string val);
void LastStatus_set(string val);
void LastAlive_set(string val);
void isActive_set(short int val);
void FK_Version_set(long int val);
void isMonitored_set(short int val);
void FK_RepositoryType_Source_set(long int val);
void FK_RepositoryType_Binaries_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Name_isNull();
bool Address_isNull();
bool City_isNull();
bool State_isNull();
bool Zip_isNull();
bool ActivationCode_isNull();
bool LastStatus_isNull();
bool LastAlive_isNull();
bool FK_Version_isNull();
bool isMonitored_isNull();
bool FK_RepositoryType_Source_isNull();
bool FK_RepositoryType_Binaries_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Name_setNull(bool val);
void Address_setNull(bool val);
void City_setNull(bool val);
void State_setNull(bool val);
void Zip_setNull(bool val);
void ActivationCode_setNull(bool val);
void LastStatus_setNull(bool val);
void LastAlive_setNull(bool val);
void FK_Version_setNull(bool val);
void isMonitored_setNull(bool val);
void FK_RepositoryType_Source_setNull(bool val);
void FK_RepositoryType_Binaries_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Installation(Table_Installation *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Installation *Table_Installation_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Country* FK_Country_getrow();
class Row_Version* FK_Version_getrow();
class Row_RepositoryType* FK_RepositoryType_Source_getrow();
class Row_RepositoryType* FK_RepositoryType_Binaries_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Installation_getrows(vector <class Row_CommandGroup*> *rows);
void Criteria_FK_Installation_getrows(vector <class Row_Criteria*> *rows);
void Device_FK_Installation_getrows(vector <class Row_Device*> *rows);
void DeviceGroup_FK_Installation_getrows(vector <class Row_DeviceGroup*> *rows);
void EventHandler_FK_Installation_getrows(vector <class Row_EventHandler*> *rows);
void Floorplan_FK_Installation_getrows(vector <class Row_Floorplan*> *rows);
void Household_Installation_FK_Installation_getrows(vector <class Row_Household_Installation*> *rows);
void Installation_RepositorySource_URL_FK_Installation_getrows(vector <class Row_Installation_RepositorySource_URL*> *rows);
void Installation_Users_FK_Installation_getrows(vector <class Row_Installation_Users*> *rows);
void Room_FK_Installation_getrows(vector <class Row_Room*> *rows);
void SetupStep_FK_Installation_getrows(vector <class Row_SetupStep*> *rows);
void Users_FK_Installation_Main_getrows(vector <class Row_Users*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Installation+ m_Description+ m_Name+ m_Address+ m_City+ m_State+ m_Zip+ m_FK_Country+ m_ActivationCode+ m_LastStatus+ m_LastAlive+ m_isActive+ m_FK_Version+ m_isMonitored+ m_FK_RepositoryType_Source+ m_FK_RepositoryType_Binaries+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Installation_asSQL();
string Description_asSQL();
string Name_asSQL();
string Address_asSQL();
string City_asSQL();
string State_asSQL();
string Zip_asSQL();
string FK_Country_asSQL();
string ActivationCode_asSQL();
string LastStatus_asSQL();
string LastAlive_asSQL();
string isActive_asSQL();
string FK_Version_asSQL();
string isMonitored_asSQL();
string FK_RepositoryType_Source_asSQL();
string FK_RepositoryType_Binaries_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

