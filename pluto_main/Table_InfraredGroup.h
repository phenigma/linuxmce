#ifndef __Table_InfraredGroup_H__
#define __Table_InfraredGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_InfraredGroup : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InfraredGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InfraredGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InfraredGroup();

private:		
	friend class Row_InfraredGroup;
	struct Key
	{
		friend class Row_InfraredGroup;
		long int pk_PK_InfraredGroup;

		
		Key(long int in_PK_InfraredGroup);
	
		Key(class Row_InfraredGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup::Key &key1, const Table_InfraredGroup::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredGroup*> *rows);
	class Row_InfraredGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup* GetRow(long int in_PK_InfraredGroup);
	

private:	
	
		
	class Row_InfraredGroup* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_InfraredGroup : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup::Key;
		friend class Table_InfraredGroup;
	private:
		Table_InfraredGroup *table;
		
		long int m_PK_InfraredGroup;
long int m_FK_DeviceCategory;
long int m_FK_Manufacturer;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_InfraredGroup_get();
long int FK_DeviceCategory_get();
long int FK_Manufacturer_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_InfraredGroup_set(long int val);
void FK_DeviceCategory_set(long int val);
void FK_Manufacturer_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InfraredGroup(Table_InfraredGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup *Table_InfraredGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_InfraredGroup_FK_InfraredGroup_getrows(vector <class Row_DeviceTemplate_InfraredGroup*> *rows);
void InfraredCode_FK_InfraredGroup_getrows(vector <class Row_InfraredCode*> *rows);
void InfraredGroup_DSPMode_FK_InfraredGroup_getrows(vector <class Row_InfraredGroup_DSPMode*> *rows);
void InfraredGroup_Input_FK_InfraredGroup_getrows(vector <class Row_InfraredGroup_Input*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_InfraredGroup+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_InfraredGroup_asSQL();
string FK_DeviceCategory_asSQL();
string FK_Manufacturer_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

