#ifndef __Table_DeviceGroup_H__
#define __Table_DeviceGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceGroup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceGroup : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceGroup();

private:		
	friend class Row_DeviceGroup;
	struct Key
	{
		friend class Row_DeviceGroup;
		long int pk_PK_DeviceGroup;

		
		Key(long int in_PK_DeviceGroup);
	
		Key(class Row_DeviceGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceGroup::Key &key1, const Table_DeviceGroup::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceGroup*> *rows);
	class Row_DeviceGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceGroup* GetRow(long int in_PK_DeviceGroup);
	

private:	
	
		
	class Row_DeviceGroup* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceGroup : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceGroup::Key;
		friend class Table_DeviceGroup;
	private:
		Table_DeviceGroup *table;
		
		long int m_PK_DeviceGroup;
long int m_FK_DeviceGroup_Parent;
long int m_FK_Installation;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_DeviceGroup_get();
long int FK_DeviceGroup_Parent_get();
long int FK_Installation_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DeviceGroup_set(long int val);
void FK_DeviceGroup_Parent_set(long int val);
void FK_Installation_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DeviceGroup_Parent_isNull();
bool FK_Installation_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DeviceGroup_Parent_setNull(bool val);
void FK_Installation_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceGroup(Table_DeviceGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceGroup *Table_DeviceGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceGroup* FK_DeviceGroup_Parent_getrow();
class Row_Installation* FK_Installation_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_DeviceGroup_getrows(vector <class Row_CommandGroup_Command*> *rows);
void DeviceGroup_FK_DeviceGroup_Parent_getrows(vector <class Row_DeviceGroup*> *rows);
void Device_DeviceGroup_FK_DeviceGroup_getrows(vector <class Row_Device_DeviceGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceGroup+ m_FK_DeviceGroup_Parent+ m_FK_Installation+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceGroup_asSQL();
string FK_DeviceGroup_Parent_asSQL();
string FK_Installation_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

