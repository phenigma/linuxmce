#ifndef __Table_CriteriaParm_D_H__
#define __Table_CriteriaParm_D_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaParm_D.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_CriteriaParm_D : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CriteriaParm_D(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CriteriaParm_D")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CriteriaParm_D();

private:		
	friend class Row_CriteriaParm_D;
	struct Key
	{
		friend class Row_CriteriaParm_D;
		long int pk_PK_CriteriaParm;

		
		Key(long int in_PK_CriteriaParm);
	
		Key(class Row_CriteriaParm_D *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaParm_D::Key &key1, const Table_CriteriaParm_D::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CriteriaParm_D*> *rows);
	class Row_CriteriaParm_D* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaParm_D* GetRow(long int in_PK_CriteriaParm);
	

private:	
	
		
	class Row_CriteriaParm_D* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_CriteriaParm_D : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaParm_D::Key;
		friend class Table_CriteriaParm_D;
	private:
		Table_CriteriaParm_D *table;
		
		long int m_PK_CriteriaParm;
long int m_FK_CriteriaParmNesting_D;
long int m_FK_CriteriaParmList;
long int m_Operator;
string m_Value;
long int m_FK_CannedEvents_CriteriaParmList;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[11];
	
	public:
		long int PK_CriteriaParm_get();
long int FK_CriteriaParmNesting_D_get();
long int FK_CriteriaParmList_get();
long int Operator_get();
string Value_get();
long int FK_CannedEvents_CriteriaParmList_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_CriteriaParm_set(long int val);
void FK_CriteriaParmNesting_D_set(long int val);
void FK_CriteriaParmList_set(long int val);
void Operator_set(long int val);
void Value_set(string val);
void FK_CannedEvents_CriteriaParmList_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_CannedEvents_CriteriaParmList_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_CannedEvents_CriteriaParmList_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CriteriaParm_D(Table_CriteriaParm_D *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaParm_D *Table_CriteriaParm_D_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaParmNesting_D* FK_CriteriaParmNesting_D_getrow();
class Row_CriteriaParmList* FK_CriteriaParmList_getrow();
class Row_CannedEvents_CriteriaParmList* FK_CannedEvents_CriteriaParmList_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CriteriaParm+ m_FK_CriteriaParmNesting_D+ m_FK_CriteriaParmList+ m_Operator+ m_Value+ m_FK_CannedEvents_CriteriaParmList+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_CriteriaParm_asSQL();
string FK_CriteriaParmNesting_D_asSQL();
string FK_CriteriaParmList_asSQL();
string Operator_asSQL();
string Value_asSQL();
string FK_CannedEvents_CriteriaParmList_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

