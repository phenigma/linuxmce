#ifndef __Table_InfraredGroup_Input_H__
#define __Table_InfraredGroup_Input_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup_Input.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_InfraredGroup_Input
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InfraredGroup_Input(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InfraredGroup_Input")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InfraredGroup_Input();

private:		
	friend class Row_InfraredGroup_Input;
	struct Key
	{
		friend class Row_InfraredGroup_Input;
		long int pk_FK_InfraredGroup;
long int pk_FK_Input;

		
		Key(long int in_FK_InfraredGroup, long int in_FK_Input);
	
		Key(class Row_InfraredGroup_Input *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup_Input::Key &key1, const Table_InfraredGroup_Input::Key &key2) const;
	};	

	map<Table_InfraredGroup_Input::Key, class Row_InfraredGroup_Input*, Table_InfraredGroup_Input::Key_Less> cachedRows;
	map<Table_InfraredGroup_Input::Key, class Row_InfraredGroup_Input*, Table_InfraredGroup_Input::Key_Less> deleted_cachedRows;
	vector<class Row_InfraredGroup_Input*> addedRows;
	vector<class Row_InfraredGroup_Input*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredGroup_Input*> *rows);
	class Row_InfraredGroup_Input* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup_Input* GetRow(long int in_FK_InfraredGroup, long int in_FK_Input);
	

private:	
	
		
	class Row_InfraredGroup_Input* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_InfraredGroup_Input : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup_Input::Key;
		friend class Table_InfraredGroup_Input;
	private:
		Table_InfraredGroup_Input *table;
		
		long int m_FK_InfraredGroup;
long int m_FK_Input;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_InfraredGroup_get();
long int FK_Input_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_InfraredGroup_set(long int val);
void FK_Input_set(long int val);
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
	
		Row_InfraredGroup_Input(Table_InfraredGroup_Input *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup_Input *Table_InfraredGroup_Input_get() { return table; };

		// Return the rows for foreign keys 
		class Row_InfraredGroup* FK_InfraredGroup_getrow();
class Row_Input* FK_Input_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_InfraredGroup+ m_FK_Input+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_InfraredGroup_asSQL();
string FK_Input_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

