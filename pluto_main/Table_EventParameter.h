#ifndef __Table_EventParameter_H__
#define __Table_EventParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EventParameter.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_EventParameter : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_EventParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("EventParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_EventParameter();

private:		
	friend class Row_EventParameter;
	struct Key
	{
		friend class Row_EventParameter;
		long int pk_PK_EventParameter;

		
		Key(long int in_PK_EventParameter);
	
		Key(class Row_EventParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EventParameter::Key &key1, const Table_EventParameter::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_EventParameter*> *rows);
	class Row_EventParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventParameter* GetRow(long int in_PK_EventParameter);
	

private:	
	
		
	class Row_EventParameter* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_EventParameter : public TableRow, public SerializeClass
	{
		friend struct Table_EventParameter::Key;
		friend class Table_EventParameter;
	private:
		Table_EventParameter *table;
		
		long int m_PK_EventParameter;
string m_Description;
string m_Define;
string m_Comments;
long int m_FK_ParameterType;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_EventParameter_get();
string Description_get();
string Define_get();
string Comments_get();
long int FK_ParameterType_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_EventParameter_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Comments_set(string val);
void FK_ParameterType_set(long int val);
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
	
		Row_EventParameter(Table_EventParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EventParameter *Table_EventParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ParameterType* FK_ParameterType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Event_EventParameter_FK_EventParameter_getrows(vector <class Row_Event_EventParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_EventParameter+ m_Description+ m_Define+ m_Comments+ m_FK_ParameterType+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_EventParameter_asSQL();
string Description_asSQL();
string Define_asSQL();
string Comments_asSQL();
string FK_ParameterType_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

