#ifndef __Table_DeviceCategory_H__
#define __Table_DeviceCategory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCategory.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceCategory : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCategory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCategory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCategory();

private:		
	friend class Row_DeviceCategory;
	struct Key
	{
		friend class Row_DeviceCategory;
		long int pk_PK_DeviceCategory;

		
		Key(long int in_PK_DeviceCategory);
	
		Key(class Row_DeviceCategory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCategory::Key &key1, const Table_DeviceCategory::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCategory*> *rows);
	class Row_DeviceCategory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCategory* GetRow(long int in_PK_DeviceCategory);
	

private:	
	
		
	class Row_DeviceCategory* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceCategory : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCategory::Key;
		friend class Table_DeviceCategory;
	private:
		Table_DeviceCategory *table;
		
		long int m_PK_DeviceCategory;
long int m_FK_DeviceCategory_Parent;
string m_Description;
string m_Define;
short int m_IsAV;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_DeviceCategory_get();
long int FK_DeviceCategory_Parent_get();
string Description_get();
string Define_get();
short int IsAV_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DeviceCategory_set(long int val);
void FK_DeviceCategory_Parent_set(long int val);
void Description_set(string val);
void Define_set(string val);
void IsAV_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DeviceCategory_Parent_isNull();
bool Description_isNull();
bool Define_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DeviceCategory_Parent_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceCategory(Table_DeviceCategory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCategory *Table_DeviceCategory_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_D_Command_FK_DeviceCategory_getrows(vector <class Row_CommandGroup_D_Command*> *rows);
void ConfigType_FK_DeviceCategory_getrows(vector <class Row_ConfigType*> *rows);
void DeviceCategory_FK_DeviceCategory_Parent_getrows(vector <class Row_DeviceCategory*> *rows);
void DeviceCategory_DeviceData_FK_DeviceCategory_getrows(vector <class Row_DeviceCategory_DeviceData*> *rows);
void DeviceCategory_Event_FK_DeviceCategory_getrows(vector <class Row_DeviceCategory_Event*> *rows);
void DeviceCommandGroup_FK_DeviceCategory_getrows(vector <class Row_DeviceCommandGroup*> *rows);
void DeviceTemplate_FK_DeviceCategory_getrows(vector <class Row_DeviceTemplate*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_FK_DeviceCategory_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows);
void InfraredGroup_FK_DeviceCategory_getrows(vector <class Row_InfraredGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceCategory+ m_FK_DeviceCategory_Parent+ m_Description+ m_Define+ m_IsAV+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceCategory_asSQL();
string FK_DeviceCategory_Parent_asSQL();
string Description_asSQL();
string Define_asSQL();
string IsAV_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

