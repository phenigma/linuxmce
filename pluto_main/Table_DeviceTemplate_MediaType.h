#ifndef __Table_DeviceTemplate_MediaType_H__
#define __Table_DeviceTemplate_MediaType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_MediaType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceTemplate_MediaType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_MediaType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_MediaType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_MediaType();

private:		
	friend class Row_DeviceTemplate_MediaType;
	struct Key
	{
		friend class Row_DeviceTemplate_MediaType;
		long int pk_PK_DeviceTemplate_MediaType;

		
		Key(long int in_PK_DeviceTemplate_MediaType);
	
		Key(class Row_DeviceTemplate_MediaType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_MediaType::Key &key1, const Table_DeviceTemplate_MediaType::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_MediaType*> *rows);
	class Row_DeviceTemplate_MediaType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_MediaType* GetRow(long int in_PK_DeviceTemplate_MediaType);
	

private:	
	
		
	class Row_DeviceTemplate_MediaType* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_MediaType : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_MediaType::Key;
		friend class Table_DeviceTemplate_MediaType;
	private:
		Table_DeviceTemplate_MediaType *table;
		
		long int m_PK_DeviceTemplate_MediaType;
long int m_FK_DeviceTemplate;
long int m_FK_MediaType;
short int m_CanPlayFromDiskDrive;
short int m_CanStoreOnServer;
short int m_CanSetPosition;
short int m_StopOtherMediaInEntArea;
string m_Extensions;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[13];
	
	public:
		long int PK_DeviceTemplate_MediaType_get();
long int FK_DeviceTemplate_get();
long int FK_MediaType_get();
short int CanPlayFromDiskDrive_get();
short int CanStoreOnServer_get();
short int CanSetPosition_get();
short int StopOtherMediaInEntArea_get();
string Extensions_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DeviceTemplate_MediaType_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_MediaType_set(long int val);
void CanPlayFromDiskDrive_set(short int val);
void CanStoreOnServer_set(short int val);
void CanSetPosition_set(short int val);
void StopOtherMediaInEntArea_set(short int val);
void Extensions_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Extensions_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Extensions_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_MediaType(Table_DeviceTemplate_MediaType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_MediaType *Table_DeviceTemplate_MediaType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_MediaType* FK_MediaType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_MediaType_DesignObj_FK_DeviceTemplate_MediaType_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceTemplate_MediaType+ m_FK_DeviceTemplate+ m_FK_MediaType+ m_CanPlayFromDiskDrive+ m_CanStoreOnServer+ m_CanSetPosition+ m_StopOtherMediaInEntArea+ m_Extensions+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceTemplate_MediaType_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_MediaType_asSQL();
string CanPlayFromDiskDrive_asSQL();
string CanStoreOnServer_asSQL();
string CanSetPosition_asSQL();
string StopOtherMediaInEntArea_asSQL();
string Extensions_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

