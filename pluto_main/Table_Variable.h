#ifndef __Table_Variable_H__
#define __Table_Variable_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Variable.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Variable : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Variable(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Variable")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Variable();

private:		
	friend class Row_Variable;
	struct Key
	{
		friend class Row_Variable;
		long int pk_PK_Variable;

		
		Key(long int in_PK_Variable);
	
		Key(class Row_Variable *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Variable::Key &key1, const Table_Variable::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Variable*> *rows);
	class Row_Variable* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Variable* GetRow(long int in_PK_Variable);
	

private:	
	
		
	class Row_Variable* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Variable : public TableRow, public SerializeClass
	{
		friend struct Table_Variable::Key;
		friend class Table_Variable;
	private:
		Table_Variable *table;
		
		long int m_PK_Variable;
string m_Description;
string m_Define;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Variable_get();
string Description_get();
string Define_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Variable_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Comments_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Comments_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Variable(Table_Variable *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Variable *Table_Variable_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Orbiter_Variable_FK_Variable_getrows(vector <class Row_Orbiter_Variable*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Variable+ m_Description+ m_Define+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Variable_asSQL();
string Description_asSQL();
string Define_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

