#ifndef __Table_Criteria_H__
#define __Table_Criteria_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Criteria.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Criteria : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Criteria(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Criteria")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Criteria();

private:		
	friend class Row_Criteria;
	struct Key
	{
		friend class Row_Criteria;
		long int pk_PK_Criteria;

		
		Key(long int in_PK_Criteria);
	
		Key(class Row_Criteria *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Criteria::Key &key1, const Table_Criteria::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Criteria*> *rows);
	class Row_Criteria* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Criteria* GetRow(long int in_PK_Criteria);
	

private:	
	
		
	class Row_Criteria* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Criteria : public TableRow, public SerializeClass
	{
		friend struct Table_Criteria::Key;
		friend class Table_Criteria;
	private:
		Table_Criteria *table;
		
		long int m_PK_Criteria;
long int m_FK_CriteriaParmNesting;
long int m_FK_CriteriaList;
long int m_FK_Installation;
string m_Description;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[11];
	
	public:
		long int PK_Criteria_get();
long int FK_CriteriaParmNesting_get();
long int FK_CriteriaList_get();
long int FK_Installation_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Criteria_set(long int val);
void FK_CriteriaParmNesting_set(long int val);
void FK_CriteriaList_set(long int val);
void FK_Installation_set(long int val);
void Description_set(string val);
void Define_set(string val);
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
	
		Row_Criteria(Table_Criteria *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Criteria *Table_Criteria_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaParmNesting* FK_CriteriaParmNesting_getrow();
class Row_CriteriaList* FK_CriteriaList_getrow();
class Row_Installation* FK_Installation_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Criteria_Orbiter_getrows(vector <class Row_CommandGroup*> *rows);
void CommandGroup_D_FK_Criteria_Orbiter_getrows(vector <class Row_CommandGroup_D*> *rows);
void DesignObjVariation_FK_Criteria_Orbiter_getrows(vector <class Row_DesignObjVariation*> *rows);
void EventHandler_FK_Criteria_getrows(vector <class Row_EventHandler*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Criteria+ m_FK_CriteriaParmNesting+ m_FK_CriteriaList+ m_FK_Installation+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Criteria_asSQL();
string FK_CriteriaParmNesting_asSQL();
string FK_CriteriaList_asSQL();
string FK_Installation_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

