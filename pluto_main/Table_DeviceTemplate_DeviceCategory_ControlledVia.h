#ifndef __Table_DeviceTemplate_DeviceCategory_ControlledVia_H__
#define __Table_DeviceTemplate_DeviceCategory_ControlledVia_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DeviceCategory_ControlledVia.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_DeviceCategory_ControlledVia
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_DeviceCategory_ControlledVia(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_DeviceCategory_ControlledVia")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_DeviceCategory_ControlledVia();

private:		
	friend class Row_DeviceTemplate_DeviceCategory_ControlledVia;
	struct Key
	{
		friend class Row_DeviceTemplate_DeviceCategory_ControlledVia;
		long int pk_PK_DeviceTemplate_DeviceCategory_ControlledVia;

		
		Key(long int in_PK_DeviceTemplate_DeviceCategory_ControlledVia);
	
		Key(class Row_DeviceTemplate_DeviceCategory_ControlledVia *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DeviceCategory_ControlledVia::Key &key1, const Table_DeviceTemplate_DeviceCategory_ControlledVia::Key &key2) const;
	};	

	map<Table_DeviceTemplate_DeviceCategory_ControlledVia::Key, class Row_DeviceTemplate_DeviceCategory_ControlledVia*, Table_DeviceTemplate_DeviceCategory_ControlledVia::Key_Less> cachedRows;
	map<Table_DeviceTemplate_DeviceCategory_ControlledVia::Key, class Row_DeviceTemplate_DeviceCategory_ControlledVia*, Table_DeviceTemplate_DeviceCategory_ControlledVia::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia*> addedRows;
	vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows);
	class Row_DeviceTemplate_DeviceCategory_ControlledVia* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DeviceCategory_ControlledVia* GetRow(long int in_PK_DeviceTemplate_DeviceCategory_ControlledVia);
	

private:	
	
		
	class Row_DeviceTemplate_DeviceCategory_ControlledVia* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_DeviceCategory_ControlledVia : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DeviceCategory_ControlledVia::Key;
		friend class Table_DeviceTemplate_DeviceCategory_ControlledVia;
	private:
		Table_DeviceTemplate_DeviceCategory_ControlledVia *table;
		
		long int m_PK_DeviceTemplate_DeviceCategory_ControlledVia;
long int m_FK_DeviceTemplate;
long int m_FK_DeviceCategory;
string m_Description;
short int m_RerouteMessagesToParent;
short int m_AutoCreateChildren;
long int m_FK_Input_Audio;
long int m_FK_Input_Video;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[13];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DeviceTemplate_DeviceCategory_ControlledVia_get();
long int FK_DeviceTemplate_get();
long int FK_DeviceCategory_get();
string Description_get();
short int RerouteMessagesToParent_get();
short int AutoCreateChildren_get();
long int FK_Input_Audio_get();
long int FK_Input_Video_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DeviceTemplate_DeviceCategory_ControlledVia_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_DeviceCategory_set(long int val);
void Description_set(string val);
void RerouteMessagesToParent_set(short int val);
void AutoCreateChildren_set(short int val);
void FK_Input_Audio_set(long int val);
void FK_Input_Video_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Description_isNull();
bool FK_Input_Audio_isNull();
bool FK_Input_Video_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Description_setNull(bool val);
void FK_Input_Audio_setNull(bool val);
void FK_Input_Video_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_DeviceCategory_ControlledVia(Table_DeviceTemplate_DeviceCategory_ControlledVia *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DeviceCategory_ControlledVia *Table_DeviceTemplate_DeviceCategory_ControlledVia_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Input* FK_Input_Audio_getrow();
class Row_Input* FK_Input_Video_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_DeviceTemplate_DeviceCategory_ControlledVia_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DeviceTemplate_DeviceCategory_ControlledVia+ m_FK_DeviceTemplate+ m_FK_DeviceCategory+ m_Description+ m_RerouteMessagesToParent+ m_AutoCreateChildren+ m_FK_Input_Audio+ m_FK_Input_Video+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_DeviceCategory_asSQL();
string Description_asSQL();
string RerouteMessagesToParent_asSQL();
string AutoCreateChildren_asSQL();
string FK_Input_Audio_asSQL();
string FK_Input_Video_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

