#ifndef __Table_FloorplanType_H__
#define __Table_FloorplanType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_FloorplanType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_FloorplanType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_FloorplanType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("FloorplanType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_FloorplanType();

private:		
	friend class Row_FloorplanType;
	struct Key
	{
		friend class Row_FloorplanType;
		long int pk_PK_FloorplanType;

		
		Key(long int in_PK_FloorplanType);
	
		Key(class Row_FloorplanType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_FloorplanType::Key &key1, const Table_FloorplanType::Key &key2) const;
	};	

	map<Table_FloorplanType::Key, class Row_FloorplanType*, Table_FloorplanType::Key_Less> cachedRows;
	map<Table_FloorplanType::Key, class Row_FloorplanType*, Table_FloorplanType::Key_Less> deleted_cachedRows;
	vector<class Row_FloorplanType*> addedRows;
	vector<class Row_FloorplanType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_FloorplanType*> *rows);
	class Row_FloorplanType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_FloorplanType* GetRow(long int in_PK_FloorplanType);
	

private:	
	
		
	class Row_FloorplanType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_FloorplanType : public TableRow, public SerializeClass
	{
		friend struct Table_FloorplanType::Key;
		friend class Table_FloorplanType;
	private:
		Table_FloorplanType *table;
		
		long int m_PK_FloorplanType;
string m_Description;
string m_Define;
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
		long int PK_FloorplanType_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_FloorplanType_set(long int val);
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
	
		Row_FloorplanType(Table_FloorplanType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FloorplanType *Table_FloorplanType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void FloorplanObjectType_FK_FloorplanType_getrows(vector <class Row_FloorplanObjectType*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_FloorplanType+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_FloorplanType_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

