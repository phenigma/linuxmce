#ifndef __Table_MediaType_H__
#define __Table_MediaType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_MediaType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_MediaType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_MediaType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("MediaType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_MediaType();

private:		
	friend class Row_MediaType;
	struct Key
	{
		friend class Row_MediaType;
		long int pk_PK_MediaType;

		
		Key(long int in_PK_MediaType);
	
		Key(class Row_MediaType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_MediaType::Key &key1, const Table_MediaType::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_MediaType*> *rows);
	class Row_MediaType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_MediaType* GetRow(long int in_PK_MediaType);
	

private:	
	
		
	class Row_MediaType* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_MediaType : public TableRow, public SerializeClass
	{
		friend struct Table_MediaType::Key;
		friend class Table_MediaType;
	private:
		Table_MediaType *table;
		
		long int m_PK_MediaType;
string m_Define;
string m_Description;
long int m_FK_DesignObj;
short int m_DCEAware;
string m_Extensions;
string m_Subdirectory;
short int m_IsExternalTransmission;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[13];
	
	public:
		long int PK_MediaType_get();
string Define_get();
string Description_get();
long int FK_DesignObj_get();
short int DCEAware_get();
string Extensions_get();
string Subdirectory_get();
short int IsExternalTransmission_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_MediaType_set(long int val);
void Define_set(string val);
void Description_set(string val);
void FK_DesignObj_set(long int val);
void DCEAware_set(short int val);
void Extensions_set(string val);
void Subdirectory_set(string val);
void IsExternalTransmission_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Define_isNull();
bool Description_isNull();
bool FK_DesignObj_isNull();
bool Extensions_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Define_setNull(bool val);
void Description_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void Extensions_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_MediaType(Table_MediaType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_MediaType *Table_MediaType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObj* FK_DesignObj_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_MediaType_FK_MediaType_getrows(vector <class Row_DeviceTemplate_MediaType*> *rows);
void MediaType_Broadcast_FK_MediaType_getrows(vector <class Row_MediaType_Broadcast*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_MediaType+ m_Define+ m_Description+ m_FK_DesignObj+ m_DCEAware+ m_Extensions+ m_Subdirectory+ m_IsExternalTransmission+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_MediaType_asSQL();
string Define_asSQL();
string Description_asSQL();
string FK_DesignObj_asSQL();
string DCEAware_asSQL();
string Extensions_asSQL();
string Subdirectory_asSQL();
string IsExternalTransmission_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

