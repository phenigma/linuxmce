#ifndef __Table_ConfigType_H__
#define __Table_ConfigType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ConfigType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_ConfigType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_ConfigType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("ConfigType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_ConfigType();

private:		
	friend class Row_ConfigType;
	struct Key
	{
		friend class Row_ConfigType;
		long int pk_PK_ConfigType;

		
		Key(long int in_PK_ConfigType);
	
		Key(class Row_ConfigType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ConfigType::Key &key1, const Table_ConfigType::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_ConfigType*> *rows);
	class Row_ConfigType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ConfigType* GetRow(long int in_PK_ConfigType);
	

private:	
	
		
	class Row_ConfigType* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_ConfigType : public TableRow, public SerializeClass
	{
		friend struct Table_ConfigType::Key;
		friend class Table_ConfigType;
	private:
		Table_ConfigType *table;
		
		long int m_PK_ConfigType;
string m_Description;
string m_Define;
long int m_FK_DeviceTemplate;
long int m_FK_DeviceCategory;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_ConfigType_get();
string Description_get();
string Define_get();
long int FK_DeviceTemplate_get();
long int FK_DeviceCategory_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_ConfigType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_DeviceTemplate_set(long int val);
void FK_DeviceCategory_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DeviceTemplate_isNull();
bool FK_DeviceCategory_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DeviceTemplate_setNull(bool val);
void FK_DeviceCategory_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_ConfigType(Table_ConfigType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ConfigType *Table_ConfigType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceCategory* FK_DeviceCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void ConfigType_File_FK_ConfigType_getrows(vector <class Row_ConfigType_File*> *rows);
void ConfigType_Setting_FK_ConfigType_getrows(vector <class Row_ConfigType_Setting*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_ConfigType+ m_Description+ m_Define+ m_FK_DeviceTemplate+ m_FK_DeviceCategory+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_ConfigType_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_DeviceCategory_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

