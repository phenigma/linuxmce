#ifndef __Table_Orbiter_Users_PasswordReq_H__
#define __Table_Orbiter_Users_PasswordReq_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter_Users_PasswordReq.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Orbiter_Users_PasswordReq : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Orbiter_Users_PasswordReq(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Orbiter_Users_PasswordReq")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Orbiter_Users_PasswordReq();

private:		
	friend class Row_Orbiter_Users_PasswordReq;
	struct Key
	{
		friend class Row_Orbiter_Users_PasswordReq;
		long int pk_FK_Orbiter;
long int pk_FK_Users;

		
		Key(long int in_FK_Orbiter, long int in_FK_Users);
	
		Key(class Row_Orbiter_Users_PasswordReq *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter_Users_PasswordReq::Key &key1, const Table_Orbiter_Users_PasswordReq::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Orbiter_Users_PasswordReq*> *rows);
	class Row_Orbiter_Users_PasswordReq* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter_Users_PasswordReq* GetRow(long int in_FK_Orbiter, long int in_FK_Users);
	

private:	
	
		
	class Row_Orbiter_Users_PasswordReq* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_Orbiter_Users_PasswordReq : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter_Users_PasswordReq::Key;
		friend class Table_Orbiter_Users_PasswordReq;
	private:
		Table_Orbiter_Users_PasswordReq *table;
		
		long int m_FK_Orbiter;
long int m_FK_Users;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int FK_Orbiter_get();
long int FK_Users_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Orbiter_set(long int val);
void FK_Users_set(long int val);
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
	
		Row_Orbiter_Users_PasswordReq(Table_Orbiter_Users_PasswordReq *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter_Users_PasswordReq *Table_Orbiter_Users_PasswordReq_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Orbiter* FK_Orbiter_getrow();
class Row_Users* FK_Users_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Orbiter+ m_FK_Users+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Orbiter_asSQL();
string FK_Users_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

