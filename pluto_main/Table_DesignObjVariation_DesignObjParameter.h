#ifndef __Table_DesignObjVariation_DesignObjParameter_H__
#define __Table_DesignObjVariation_DesignObjParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_DesignObjParameter.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DesignObjVariation_DesignObjParameter : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjVariation_DesignObjParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjVariation_DesignObjParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjVariation_DesignObjParameter();

private:		
	friend class Row_DesignObjVariation_DesignObjParameter;
	struct Key
	{
		friend class Row_DesignObjVariation_DesignObjParameter;
		long int pk_FK_DesignObjVariation;
long int pk_FK_DesignObjParameter;

		
		Key(long int in_FK_DesignObjVariation, long int in_FK_DesignObjParameter);
	
		Key(class Row_DesignObjVariation_DesignObjParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_DesignObjParameter::Key &key1, const Table_DesignObjVariation_DesignObjParameter::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_DesignObjParameter*> *rows);
	class Row_DesignObjVariation_DesignObjParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_DesignObjParameter* GetRow(long int in_FK_DesignObjVariation, long int in_FK_DesignObjParameter);
	

private:	
	
		
	class Row_DesignObjVariation_DesignObjParameter* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation_DesignObjParameter : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_DesignObjParameter::Key;
		friend class Table_DesignObjVariation_DesignObjParameter;
	private:
		Table_DesignObjVariation_DesignObjParameter *table;
		
		long int m_FK_DesignObjVariation;
long int m_FK_DesignObjParameter;
string m_Value;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int FK_DesignObjVariation_get();
long int FK_DesignObjParameter_get();
string Value_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DesignObjVariation_set(long int val);
void FK_DesignObjParameter_set(long int val);
void Value_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Value_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Value_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_DesignObjParameter(Table_DesignObjVariation_DesignObjParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_DesignObjParameter *Table_DesignObjVariation_DesignObjParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_DesignObjParameter* FK_DesignObjParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DesignObjVariation+ m_FK_DesignObjParameter+ m_Value+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DesignObjVariation_asSQL();
string FK_DesignObjParameter_asSQL();
string Value_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

