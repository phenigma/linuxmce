#ifndef __Table_Document_Comment_H__
#define __Table_Document_Comment_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Document_Comment.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Document_Comment : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Document_Comment(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Document_Comment")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Document_Comment();

private:		
	friend class Row_Document_Comment;
	struct Key
	{
		friend class Row_Document_Comment;
		long int pk_PK_Document_Comment;

		
		Key(long int in_PK_Document_Comment);
	
		Key(class Row_Document_Comment *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Document_Comment::Key &key1, const Table_Document_Comment::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Document_Comment*> *rows);
	class Row_Document_Comment* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Document_Comment* GetRow(long int in_PK_Document_Comment);
	

private:	
	
		
	class Row_Document_Comment* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Document_Comment : public TableRow, public SerializeClass
	{
		friend struct Table_Document_Comment::Key;
		friend class Table_Document_Comment;
	private:
		Table_Document_Comment *table;
		
		long int m_PK_Document_Comment;
long int m_FK_Document;
long int m_FK_Users;
string m_Comment;
string m_Date;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_Document_Comment_get();
long int FK_Document_get();
long int FK_Users_get();
string Comment_get();
string Date_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Document_Comment_set(long int val);
void FK_Document_set(long int val);
void FK_Users_set(long int val);
void Comment_set(string val);
void Date_set(string val);
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
	
		Row_Document_Comment(Table_Document_Comment *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Document_Comment *Table_Document_Comment_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Document* FK_Document_getrow();
class Row_Users* FK_Users_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Document_Comment+ m_FK_Document+ m_FK_Users+ m_Comment+ m_Date+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Document_Comment_asSQL();
string FK_Document_asSQL();
string FK_Users_asSQL();
string Comment_asSQL();
string Date_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

