#ifndef __Table_psc_security_batdet_H__
#define __Table_psc_security_batdet_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_security_batdet.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_security_batdet : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_security_batdet(Database_pluto_security *pDatabase):database(pDatabase), m_Mutex("psc_security_batdet")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_security_batdet();

private:		
	friend class Row_psc_security_batdet;
	struct Key
	{
		friend class Row_psc_security_batdet;
		long int pk_PK_psc_security_batdet;

		
		Key(long int in_PK_psc_security_batdet);
	
		Key(class Row_psc_security_batdet *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_security_batdet::Key &key1, const Table_psc_security_batdet::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_psc_security_batdet*> *rows);
	class Row_psc_security_batdet* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_psc_security_batdet* GetRow(long int in_PK_psc_security_batdet);
	

private:	
	
		
	class Row_psc_security_batdet* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_security_batdet : public TableRow, public SerializeClass
	{
		friend struct Table_psc_security_batdet::Key;
		friend class Table_psc_security_batdet;
	private:
		Table_psc_security_batdet *table;
		
		long int m_PK_psc_security_batdet;
long int m_FK_psc_security_bathdr;
string m_Tablename;
long int m_New;
long int m_Deleted;
long int m_Modified;
long int m_FK_psc_security_bathdr_orig;
long int m_FK_psc_security_bathdr_auth;
long int m_FK_psc_security_bathdr_unauth;

		bool is_null[9];
	
	public:
		long int PK_psc_security_batdet_get();
long int FK_psc_security_bathdr_get();
string Tablename_get();
long int New_get();
long int Deleted_get();
long int Modified_get();
long int FK_psc_security_bathdr_orig_get();
long int FK_psc_security_bathdr_auth_get();
long int FK_psc_security_bathdr_unauth_get();

		
		void PK_psc_security_batdet_set(long int val);
void FK_psc_security_bathdr_set(long int val);
void Tablename_set(string val);
void New_set(long int val);
void Deleted_set(long int val);
void Modified_set(long int val);
void FK_psc_security_bathdr_orig_set(long int val);
void FK_psc_security_bathdr_auth_set(long int val);
void FK_psc_security_bathdr_unauth_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_security_batdet(Table_psc_security_batdet *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_security_batdet *Table_psc_security_batdet_get() { return table; };

		// Return the rows for foreign keys 
		class Row_psc_security_bathdr* FK_psc_security_bathdr_getrow();
class Row_psc_security_bathdr* FK_psc_security_bathdr_orig_getrow();
class Row_psc_security_bathdr* FK_psc_security_bathdr_auth_getrow();
class Row_psc_security_bathdr* FK_psc_security_bathdr_unauth_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_security_batdet+ m_FK_psc_security_bathdr+ m_Tablename+ m_New+ m_Deleted+ m_Modified+ m_FK_psc_security_bathdr_orig+ m_FK_psc_security_bathdr_auth+ m_FK_psc_security_bathdr_unauth;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_security_batdet_asSQL();
string FK_psc_security_bathdr_asSQL();
string Tablename_asSQL();
string New_asSQL();
string Deleted_asSQL();
string Modified_asSQL();
string FK_psc_security_bathdr_orig_asSQL();
string FK_psc_security_bathdr_auth_asSQL();
string FK_psc_security_bathdr_unauth_asSQL();

	};

#endif

