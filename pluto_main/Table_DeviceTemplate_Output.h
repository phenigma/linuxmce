#ifndef __Table_DeviceTemplate_Output_H__
#define __Table_DeviceTemplate_Output_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_Output.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceTemplate_Output : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_Output(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_Output")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_Output();

private:		
	friend class Row_DeviceTemplate_Output;
	struct Key
	{
		friend class Row_DeviceTemplate_Output;
		long int pk_FK_DeviceTemplate;
long int pk_FK_Output;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_Output);
	
		Key(class Row_DeviceTemplate_Output *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_Output::Key &key1, const Table_DeviceTemplate_Output::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_Output*> *rows);
	class Row_DeviceTemplate_Output* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_Output* GetRow(long int in_FK_DeviceTemplate, long int in_FK_Output);
	

private:	
	
		
	class Row_DeviceTemplate_Output* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_Output : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_Output::Key;
		friend class Table_DeviceTemplate_Output;
	private:
		Table_DeviceTemplate_Output *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_Output;
short int m_OrderNo;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_Output_get();
short int OrderNo_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_Output_set(long int val);
void OrderNo_set(short int val);
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
	
		Row_DeviceTemplate_Output(Table_DeviceTemplate_Output *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_Output *Table_DeviceTemplate_Output_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Output* FK_Output_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_Output+ m_OrderNo+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_Output_asSQL();
string OrderNo_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

