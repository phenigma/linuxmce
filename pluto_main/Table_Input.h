#ifndef __Table_Input_H__
#define __Table_Input_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Input.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Input
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Input(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Input")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Input();

private:		
	friend class Row_Input;
	struct Key
	{
		friend class Row_Input;
		long int pk_PK_Input;

		
		Key(long int in_PK_Input);
	
		Key(class Row_Input *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Input::Key &key1, const Table_Input::Key &key2) const;
	};	

	map<Table_Input::Key, class Row_Input*, Table_Input::Key_Less> cachedRows;
	map<Table_Input::Key, class Row_Input*, Table_Input::Key_Less> deleted_cachedRows;
	vector<class Row_Input*> addedRows;
	vector<class Row_Input*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Input*> *rows);
	class Row_Input* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Input* GetRow(long int in_PK_Input);
	

private:	
	
		
	class Row_Input* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Input : public TableRow, public SerializeClass
	{
		friend struct Table_Input::Key;
		friend class Table_Input;
	private:
		Table_Input *table;
		
		long int m_PK_Input;
string m_Description;
string m_Define;
long int m_FK_Command;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Input_get();
string Description_get();
string Define_get();
long int FK_Command_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Input_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_Command_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Command_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Command_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Input(Table_Input *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Input *Table_Input_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Command* FK_Command_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Device_FK_Input_Audio_getrows(vector <class Row_Device*> *rows);
void Device_FK_Input_Video_getrows(vector <class Row_Device*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_FK_Input_Audio_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_FK_Input_Video_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Input_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_FK_Input_Audio_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_FK_Input_Video_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Input_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);
void DeviceTemplate_Input_FK_Input_getrows(vector <class Row_DeviceTemplate_Input*> *rows);
void Device_Device_Pipe_FK_Input_getrows(vector <class Row_Device_Device_Pipe*> *rows);
void InfraredGroup_Input_FK_Input_getrows(vector <class Row_InfraredGroup_Input*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Input+ m_Description+ m_Define+ m_FK_Command+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Input_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_Command_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

