#ifndef __Table_ParameterType_H__
#define __Table_ParameterType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ParameterType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_ParameterType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_ParameterType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("ParameterType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_ParameterType();

private:		
	friend class Row_ParameterType;
	struct Key
	{
		friend class Row_ParameterType;
		long int pk_PK_ParameterType;

		
		Key(long int in_PK_ParameterType);
	
		Key(class Row_ParameterType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ParameterType::Key &key1, const Table_ParameterType::Key &key2) const;
	};	

	map<Table_ParameterType::Key, class Row_ParameterType*, Table_ParameterType::Key_Less> cachedRows;
	map<Table_ParameterType::Key, class Row_ParameterType*, Table_ParameterType::Key_Less> deleted_cachedRows;
	vector<class Row_ParameterType*> addedRows;
	vector<class Row_ParameterType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_ParameterType*> *rows);
	class Row_ParameterType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ParameterType* GetRow(long int in_PK_ParameterType);
	

private:	
	
		
	class Row_ParameterType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_ParameterType : public TableRow, public SerializeClass
	{
		friend struct Table_ParameterType::Key;
		friend class Table_ParameterType;
	private:
		Table_ParameterType *table;
		
		long int m_PK_ParameterType;
string m_Description;
string m_Define;
string m_CPPType;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_ParameterType_get();
string Description_get();
string Define_get();
string CPPType_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_ParameterType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void CPPType_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool CPPType_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void CPPType_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_ParameterType(Table_ParameterType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ParameterType *Table_ParameterType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CommandParameter_FK_ParameterType_getrows(vector <class Row_CommandParameter*> *rows);
void CriteriaParmList_FK_ParameterType_getrows(vector <class Row_CriteriaParmList*> *rows);
void DesignObjParameter_FK_ParameterType_getrows(vector <class Row_DesignObjParameter*> *rows);
void DeviceData_FK_ParameterType_getrows(vector <class Row_DeviceData*> *rows);
void EventParameter_FK_ParameterType_getrows(vector <class Row_EventParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_ParameterType+ m_Description+ m_Define+ m_CPPType+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_ParameterType_asSQL();
string Description_asSQL();
string Define_asSQL();
string CPPType_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

