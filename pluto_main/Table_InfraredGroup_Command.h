#ifndef __Table_InfraredGroup_Command_H__
#define __Table_InfraredGroup_Command_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup_Command.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_InfraredGroup_Command : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InfraredGroup_Command(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InfraredGroup_Command")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InfraredGroup_Command();

private:		
	friend class Row_InfraredGroup_Command;
	struct Key
	{
		friend class Row_InfraredGroup_Command;
		long int pk_PK_InfraredGroup_Command;

		
		Key(long int in_PK_InfraredGroup_Command);
	
		Key(class Row_InfraredGroup_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup_Command::Key &key1, const Table_InfraredGroup_Command::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredGroup_Command*> *rows);
	class Row_InfraredGroup_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup_Command* GetRow(long int in_PK_InfraredGroup_Command);
	

private:	
	
		
	class Row_InfraredGroup_Command* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_InfraredGroup_Command : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup_Command::Key;
		friend class Table_InfraredGroup_Command;
	private:
		Table_InfraredGroup_Command *table;
		
		long int m_PK_InfraredGroup_Command;
long int m_FK_InfraredGroup;
long int m_FK_Command;
long int m_FK_DeviceTemplate;
long int m_FK_Device;
string m_IRData;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[11];
	
	public:
		long int PK_InfraredGroup_Command_get();
long int FK_InfraredGroup_get();
long int FK_Command_get();
long int FK_DeviceTemplate_get();
long int FK_Device_get();
string IRData_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_InfraredGroup_Command_set(long int val);
void FK_InfraredGroup_set(long int val);
void FK_Command_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_Device_set(long int val);
void IRData_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_InfraredGroup_isNull();
bool FK_DeviceTemplate_isNull();
bool FK_Device_isNull();
bool IRData_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_InfraredGroup_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void FK_Device_setNull(bool val);
void IRData_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InfraredGroup_Command(Table_InfraredGroup_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup_Command *Table_InfraredGroup_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_InfraredGroup* FK_InfraredGroup_getrow();
class Row_Command* FK_Command_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Device* FK_Device_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_InfraredGroup_Command+ m_FK_InfraredGroup+ m_FK_Command+ m_FK_DeviceTemplate+ m_FK_Device+ m_IRData+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_InfraredGroup_Command_asSQL();
string FK_InfraredGroup_asSQL();
string FK_Command_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_Device_asSQL();
string IRData_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

