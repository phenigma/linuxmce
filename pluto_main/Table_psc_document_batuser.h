#ifndef __Table_psc_document_batuser_H__
#define __Table_psc_document_batuser_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_document_batuser.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_document_batuser : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_document_batuser(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("psc_document_batuser")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_document_batuser();

private:		
	friend class Row_psc_document_batuser;
	struct Key
	{
		friend class Row_psc_document_batuser;
		long int pk_PK_psc_document_batuser;

		
		Key(long int in_PK_psc_document_batuser);
	
		Key(class Row_psc_document_batuser *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_document_batuser::Key &key1, const Table_psc_document_batuser::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_psc_document_batuser*> *rows);
	class Row_psc_document_batuser* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_document_batuser* GetRow(long int in_PK_psc_document_batuser);
	

private:	
	
		
	class Row_psc_document_batuser* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_document_batuser : public TableRow, public SerializeClass
	{
		friend struct Table_psc_document_batuser::Key;
		friend class Table_psc_document_batuser;
	private:
		Table_psc_document_batuser *table;
		
		long int m_PK_psc_document_batuser;
long int m_FK_psc_document_bathdr;
long int m_psc_user;
short int m_is_sup;

		bool is_null[4];
	
	public:
		long int PK_psc_document_batuser_get();
long int FK_psc_document_bathdr_get();
long int psc_user_get();
short int is_sup_get();

		
		void PK_psc_document_batuser_set(long int val);
void FK_psc_document_bathdr_set(long int val);
void psc_user_set(long int val);
void is_sup_set(short int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_document_batuser(Table_psc_document_batuser *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_document_batuser *Table_psc_document_batuser_get() { return table; };

		// Return the rows for foreign keys 
		class Row_psc_document_bathdr* FK_psc_document_bathdr_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_document_batuser+ m_FK_psc_document_bathdr+ m_psc_user+ m_is_sup;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_document_batuser_asSQL();
string FK_psc_document_bathdr_asSQL();
string psc_user_asSQL();
string is_sup_asSQL();

	};

#endif

