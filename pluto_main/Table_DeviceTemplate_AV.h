#ifndef __Table_DeviceTemplate_AV_H__
#define __Table_DeviceTemplate_AV_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_AV.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceTemplate_AV : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_AV(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_AV")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_AV();

private:		
	friend class Row_DeviceTemplate_AV;
	struct Key
	{
		friend class Row_DeviceTemplate_AV;
		long int pk_FK_DeviceTemplate;

		
		Key(long int in_FK_DeviceTemplate);
	
		Key(class Row_DeviceTemplate_AV *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_AV::Key &key1, const Table_DeviceTemplate_AV::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_AV*> *rows);
	class Row_DeviceTemplate_AV* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_AV* GetRow(long int in_FK_DeviceTemplate);
	

private:	
	
		
	class Row_DeviceTemplate_AV* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_AV : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_AV::Key;
		friend class Table_DeviceTemplate_AV;
	private:
		Table_DeviceTemplate_AV *table;
		
		long int m_FK_DeviceTemplate;
long int m_IR_PowerDelay;
long int m_IR_ModeDelay;
long int m_DigitDelay;
long int m_TogglePower;
long int m_ToggleDSP;
long int m_ToggleInput;
long int m_ToggleOutput;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[13];
	
	public:
		long int FK_DeviceTemplate_get();
long int IR_PowerDelay_get();
long int IR_ModeDelay_get();
long int DigitDelay_get();
long int TogglePower_get();
long int ToggleDSP_get();
long int ToggleInput_get();
long int ToggleOutput_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DeviceTemplate_set(long int val);
void IR_PowerDelay_set(long int val);
void IR_ModeDelay_set(long int val);
void DigitDelay_set(long int val);
void TogglePower_set(long int val);
void ToggleDSP_set(long int val);
void ToggleInput_set(long int val);
void ToggleOutput_set(long int val);
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
	
		Row_DeviceTemplate_AV(Table_DeviceTemplate_AV *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_AV *Table_DeviceTemplate_AV_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_IR_PowerDelay+ m_IR_ModeDelay+ m_DigitDelay+ m_TogglePower+ m_ToggleDSP+ m_ToggleInput+ m_ToggleOutput+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string IR_PowerDelay_asSQL();
string IR_ModeDelay_asSQL();
string DigitDelay_asSQL();
string TogglePower_asSQL();
string ToggleDSP_asSQL();
string ToggleInput_asSQL();
string ToggleOutput_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

