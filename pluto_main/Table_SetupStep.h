#ifndef __Table_SetupStep_H__
#define __Table_SetupStep_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_SetupStep.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_SetupStep : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_SetupStep(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("SetupStep")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_SetupStep();

private:		
	friend class Row_SetupStep;
	struct Key
	{
		friend class Row_SetupStep;
		long int pk_PK_SetupStep;

		
		Key(long int in_PK_SetupStep);
	
		Key(class Row_SetupStep *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_SetupStep::Key &key1, const Table_SetupStep::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_SetupStep*> *rows);
	class Row_SetupStep* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_SetupStep* GetRow(long int in_PK_SetupStep);
	

private:	
	
		
	class Row_SetupStep* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_SetupStep : public TableRow, public SerializeClass
	{
		friend struct Table_SetupStep::Key;
		friend class Table_SetupStep;
	private:
		Table_SetupStep *table;
		
		long int m_PK_SetupStep;
long int m_FK_SetupStep_Next;
long int m_FK_SetupStep_Parent;
string m_Address;
string m_Name;
string m_Description;
short int m_FirstStep;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[13];
	
	public:
		long int PK_SetupStep_get();
long int FK_SetupStep_Next_get();
long int FK_SetupStep_Parent_get();
string Address_get();
string Name_get();
string Description_get();
short int FirstStep_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_SetupStep_set(long int val);
void FK_SetupStep_Next_set(long int val);
void FK_SetupStep_Parent_set(long int val);
void Address_set(string val);
void Name_set(string val);
void Description_set(string val);
void FirstStep_set(short int val);
void Define_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_SetupStep_Next_isNull();
bool FK_SetupStep_Parent_isNull();
bool Description_isNull();
bool Define_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_SetupStep_Next_setNull(bool val);
void FK_SetupStep_Parent_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_SetupStep(Table_SetupStep *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_SetupStep *Table_SetupStep_get() { return table; };

		// Return the rows for foreign keys 
		class Row_SetupStep* FK_SetupStep_Next_getrow();
class Row_SetupStep* FK_SetupStep_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void SetupStep_FK_SetupStep_Next_getrows(vector <class Row_SetupStep*> *rows);
void SetupStep_FK_SetupStep_Parent_getrows(vector <class Row_SetupStep*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_SetupStep+ m_FK_SetupStep_Next+ m_FK_SetupStep_Parent+ m_Address+ m_Name+ m_Description+ m_FirstStep+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_SetupStep_asSQL();
string FK_SetupStep_Next_asSQL();
string FK_SetupStep_Parent_asSQL();
string Address_asSQL();
string Name_asSQL();
string Description_asSQL();
string FirstStep_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

