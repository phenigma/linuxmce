#ifndef __Table_Output_H__
#define __Table_Output_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Output.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Output : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Output(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Output")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Output();

private:		
	friend class Row_Output;
	struct Key
	{
		friend class Row_Output;
		long int pk_PK_Output;

		
		Key(long int in_PK_Output);
	
		Key(class Row_Output *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Output::Key &key1, const Table_Output::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Output*> *rows);
	class Row_Output* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Output* GetRow(long int in_PK_Output);
	

private:	
	
		
	class Row_Output* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Output : public TableRow, public SerializeClass
	{
		friend struct Table_Output::Key;
		friend class Table_Output;
	private:
		Table_Output *table;
		
		long int m_PK_Output;
string m_Description;
string m_Define;
long int m_FK_Command;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Output_get();
string Description_get();
string Define_get();
long int FK_Command_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Output_set(long int val);
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
	
		Row_Output(Table_Output *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Output *Table_Output_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Command* FK_Command_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Output_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Output_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);
void DeviceTemplate_Output_FK_Output_getrows(vector <class Row_DeviceTemplate_Output*> *rows);
void Device_Device_Pipe_FK_Output_getrows(vector <class Row_Device_Device_Pipe*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Output+ m_Description+ m_Define+ m_FK_Command+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Output_asSQL();
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

