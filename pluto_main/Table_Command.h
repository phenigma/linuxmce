#ifndef __Table_Command_H__
#define __Table_Command_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Command.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Command : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Command(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Command")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Command();

private:		
	friend class Row_Command;
	struct Key
	{
		friend class Row_Command;
		long int pk_PK_Command;

		
		Key(long int in_PK_Command);
	
		Key(class Row_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Command::Key &key1, const Table_Command::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Command*> *rows);
	class Row_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Command* GetRow(long int in_PK_Command);
	

private:	
	
		
	class Row_Command* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Command : public TableRow, public SerializeClass
	{
		friend struct Table_Command::Key;
		friend class Table_Command;
	private:
		Table_Command *table;
		
		long int m_PK_Command;
string m_Description;
string m_Define;
long int m_FK_CommandCategory;
short int m_AVCommand;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_Command_get();
string Description_get();
string Define_get();
long int FK_CommandCategory_get();
short int AVCommand_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Command_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_CommandCategory_set(long int val);
void AVCommand_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Define_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Define_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Command(Table_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Command *Table_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandCategory* FK_CommandCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_Command_getrows(vector <class Row_CommandGroup_Command*> *rows);
void CommandGroup_D_Command_FK_Command_getrows(vector <class Row_CommandGroup_D_Command*> *rows);
void Command_CommandParameter_FK_Command_getrows(vector <class Row_Command_CommandParameter*> *rows);
void Command_Pipe_FK_Command_getrows(vector <class Row_Command_Pipe*> *rows);
void DSPMode_FK_Command_getrows(vector <class Row_DSPMode*> *rows);
void DeviceCommandGroup_Command_FK_Command_getrows(vector <class Row_DeviceCommandGroup_Command*> *rows);
void Device_Command_FK_Command_getrows(vector <class Row_Device_Command*> *rows);
void InfraredGroup_Command_FK_Command_getrows(vector <class Row_InfraredGroup_Command*> *rows);
void Input_FK_Command_getrows(vector <class Row_Input*> *rows);
void Output_FK_Command_getrows(vector <class Row_Output*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Command+ m_Description+ m_Define+ m_FK_CommandCategory+ m_AVCommand+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Command_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_CommandCategory_asSQL();
string AVCommand_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

