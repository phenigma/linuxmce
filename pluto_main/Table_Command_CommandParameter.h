#ifndef __Table_Command_CommandParameter_H__
#define __Table_Command_CommandParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Command_CommandParameter.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Command_CommandParameter : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Command_CommandParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Command_CommandParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Command_CommandParameter();

private:		
	friend class Row_Command_CommandParameter;
	struct Key
	{
		friend class Row_Command_CommandParameter;
		long int pk_FK_Command;
long int pk_FK_CommandParameter;

		
		Key(long int in_FK_Command, long int in_FK_CommandParameter);
	
		Key(class Row_Command_CommandParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Command_CommandParameter::Key &key1, const Table_Command_CommandParameter::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Command_CommandParameter*> *rows);
	class Row_Command_CommandParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Command_CommandParameter* GetRow(long int in_FK_Command, long int in_FK_CommandParameter);
	

private:	
	
		
	class Row_Command_CommandParameter* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_Command_CommandParameter : public TableRow, public SerializeClass
	{
		friend struct Table_Command_CommandParameter::Key;
		friend class Table_Command_CommandParameter;
	private:
		Table_Command_CommandParameter *table;
		
		long int m_FK_Command;
long int m_FK_CommandParameter;
string m_Description;
short int m_IsOut;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int FK_Command_get();
long int FK_CommandParameter_get();
string Description_get();
short int IsOut_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Command_set(long int val);
void FK_CommandParameter_set(long int val);
void Description_set(string val);
void IsOut_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Command_CommandParameter(Table_Command_CommandParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Command_CommandParameter *Table_Command_CommandParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Command* FK_Command_getrow();
class Row_CommandParameter* FK_CommandParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Command+ m_FK_CommandParameter+ m_Description+ m_IsOut+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Command_asSQL();
string FK_CommandParameter_asSQL();
string Description_asSQL();
string IsOut_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

