#ifndef __Table_DesignObjType_H__
#define __Table_DesignObjType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DesignObjType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjType();

private:		
	friend class Row_DesignObjType;
	struct Key
	{
		friend class Row_DesignObjType;
		long int pk_PK_DesignObjType;

		
		Key(long int in_PK_DesignObjType);
	
		Key(class Row_DesignObjType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjType::Key &key1, const Table_DesignObjType::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjType*> *rows);
	class Row_DesignObjType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjType* GetRow(long int in_PK_DesignObjType);
	

private:	
	
		
	class Row_DesignObjType* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DesignObjType : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjType::Key;
		friend class Table_DesignObjType;
	private:
		Table_DesignObjType *table;
		
		long int m_PK_DesignObjType;
string m_Description;
string m_Define;
short int m_CanClick;
short int m_ContainsText;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_DesignObjType_get();
string Description_get();
string Define_get();
short int CanClick_get();
short int ContainsText_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DesignObjType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void CanClick_set(short int val);
void ContainsText_set(short int val);
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
	
		Row_DesignObjType(Table_DesignObjType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjType *Table_DesignObjType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DesignObj_FK_DesignObjType_getrows(vector <class Row_DesignObj*> *rows);
void DesignObjType_DesignObjParameter_FK_DesignObjType_getrows(vector <class Row_DesignObjType_DesignObjParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DesignObjType+ m_Description+ m_Define+ m_CanClick+ m_ContainsText+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjType_asSQL();
string Description_asSQL();
string Define_asSQL();
string CanClick_asSQL();
string ContainsText_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

