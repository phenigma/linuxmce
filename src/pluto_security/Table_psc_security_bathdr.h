#ifndef __Table_psc_security_bathdr_H__
#define __Table_psc_security_bathdr_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_security_bathdr.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_security_bathdr : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_security_bathdr(Database_pluto_security *pDatabase):database(pDatabase), m_Mutex("psc_security_bathdr")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_security_bathdr();

private:		
	friend class Row_psc_security_bathdr;
	struct Key
	{
		friend class Row_psc_security_bathdr;
		long int pk_PK_psc_security_bathdr;

		
		Key(long int in_PK_psc_security_bathdr);
	
		Key(class Row_psc_security_bathdr *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_security_bathdr::Key &key1, const Table_psc_security_bathdr::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_psc_security_bathdr*> *rows);
	class Row_psc_security_bathdr* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_psc_security_bathdr* GetRow(long int in_PK_psc_security_bathdr);
	

private:	
	
		
	class Row_psc_security_bathdr* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_security_bathdr : public TableRow, public SerializeClass
	{
		friend struct Table_psc_security_bathdr::Key;
		friend class Table_psc_security_bathdr;
	private:
		Table_psc_security_bathdr *table;
		
		long int m_PK_psc_security_bathdr;
string m_IPAddress;
string m_date;
string m_comments;

		bool is_null[4];
	
	public:
		long int PK_psc_security_bathdr_get();
string IPAddress_get();
string date_get();
string comments_get();

		
		void PK_psc_security_bathdr_set(long int val);
void IPAddress_set(string val);
void date_set(string val);
void comments_set(string val);

		
		bool IPAddress_isNull();
bool date_isNull();
bool comments_isNull();

			
		void IPAddress_setNull(bool val);
void date_setNull(bool val);
void comments_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_security_bathdr(Table_psc_security_bathdr *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_security_bathdr *Table_psc_security_bathdr_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void psc_security_batdet_FK_psc_security_bathdr_getrows(vector <class Row_psc_security_batdet*> *rows);
void psc_security_batdet_FK_psc_security_bathdr_orig_getrows(vector <class Row_psc_security_batdet*> *rows);
void psc_security_batdet_FK_psc_security_bathdr_auth_getrows(vector <class Row_psc_security_batdet*> *rows);
void psc_security_batdet_FK_psc_security_bathdr_unauth_getrows(vector <class Row_psc_security_batdet*> *rows);
void psc_security_batuser_FK_psc_security_bathdr_getrows(vector <class Row_psc_security_batuser*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_security_bathdr+ m_IPAddress+ m_date+ m_comments;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_security_bathdr_asSQL();
string IPAddress_asSQL();
string date_asSQL();
string comments_asSQL();

	};

#endif

