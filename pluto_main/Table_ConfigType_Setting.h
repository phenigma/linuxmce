#ifndef __Table_ConfigType_Setting_H__
#define __Table_ConfigType_Setting_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ConfigType_Setting.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_ConfigType_Setting : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_ConfigType_Setting(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("ConfigType_Setting")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_ConfigType_Setting();

private:		
	friend class Row_ConfigType_Setting;
	struct Key
	{
		friend class Row_ConfigType_Setting;
		long int pk_PK_ConfigType_Setting;

		
		Key(long int in_PK_ConfigType_Setting);
	
		Key(class Row_ConfigType_Setting *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ConfigType_Setting::Key &key1, const Table_ConfigType_Setting::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_ConfigType_Setting*> *rows);
	class Row_ConfigType_Setting* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ConfigType_Setting* GetRow(long int in_PK_ConfigType_Setting);
	

private:	
	
		
	class Row_ConfigType_Setting* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_ConfigType_Setting : public TableRow, public SerializeClass
	{
		friend struct Table_ConfigType_Setting::Key;
		friend class Table_ConfigType_Setting;
	private:
		Table_ConfigType_Setting *table;
		
		long int m_PK_ConfigType_Setting;
long int m_FK_ConfigType;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_ConfigType_Setting_get();
long int FK_ConfigType_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_ConfigType_Setting_set(long int val);
void FK_ConfigType_set(long int val);
void Description_set(string val);
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
	
		Row_ConfigType_Setting(Table_ConfigType_Setting *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ConfigType_Setting *Table_ConfigType_Setting_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ConfigType* FK_ConfigType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void ConfigType_Token_FK_ConfigType_Setting_getrows(vector <class Row_ConfigType_Token*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_ConfigType_Setting+ m_FK_ConfigType+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_ConfigType_Setting_asSQL();
string FK_ConfigType_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

