#ifndef __Table_Pipe_H__
#define __Table_Pipe_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Pipe.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Pipe : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Pipe(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Pipe")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Pipe();

private:		
	friend class Row_Pipe;
	struct Key
	{
		friend class Row_Pipe;
		long int pk_PK_Pipe;

		
		Key(long int in_PK_Pipe);
	
		Key(class Row_Pipe *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Pipe::Key &key1, const Table_Pipe::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Pipe*> *rows);
	class Row_Pipe* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Pipe* GetRow(long int in_PK_Pipe);
	

private:	
	
		
	class Row_Pipe* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Pipe : public TableRow, public SerializeClass
	{
		friend struct Table_Pipe::Key;
		friend class Table_Pipe;
	private:
		Table_Pipe *table;
		
		long int m_PK_Pipe;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int PK_Pipe_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Pipe_set(long int val);
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
	
		Row_Pipe(Table_Pipe *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Pipe *Table_Pipe_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Command_Pipe_FK_Pipe_getrows(vector <class Row_Command_Pipe*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Pipe_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Pipe_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);
void Device_Device_Pipe_FK_Pipe_getrows(vector <class Row_Device_Device_Pipe*> *rows);
void MediaType_FK_Pipe_getrows(vector <class Row_MediaType*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Pipe+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Pipe_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

