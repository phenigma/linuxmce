#ifndef __Table_RepositoryType_H__
#define __Table_RepositoryType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RepositoryType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_RepositoryType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_RepositoryType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("RepositoryType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_RepositoryType();

private:		
	friend class Row_RepositoryType;
	struct Key
	{
		friend class Row_RepositoryType;
		long int pk_PK_RepositoryType;

		
		Key(long int in_PK_RepositoryType);
	
		Key(class Row_RepositoryType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RepositoryType::Key &key1, const Table_RepositoryType::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_RepositoryType*> *rows);
	class Row_RepositoryType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RepositoryType* GetRow(long int in_PK_RepositoryType);
	

private:	
	
		
	class Row_RepositoryType* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_RepositoryType : public TableRow, public SerializeClass
	{
		friend struct Table_RepositoryType::Key;
		friend class Table_RepositoryType;
	private:
		Table_RepositoryType *table;
		
		long int m_PK_RepositoryType;
string m_Description;
string m_Define;
short int m_SourceOnly;
string m_PathToFile;
string m_Instructions;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[11];
	
	public:
		long int PK_RepositoryType_get();
string Description_get();
string Define_get();
short int SourceOnly_get();
string PathToFile_get();
string Instructions_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_RepositoryType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void SourceOnly_set(short int val);
void PathToFile_set(string val);
void Instructions_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Define_isNull();
bool SourceOnly_isNull();
bool PathToFile_isNull();
bool Instructions_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Define_setNull(bool val);
void SourceOnly_setNull(bool val);
void PathToFile_setNull(bool val);
void Instructions_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RepositoryType(Table_RepositoryType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RepositoryType *Table_RepositoryType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Installation_FK_RepositoryType_Source_getrows(vector <class Row_Installation*> *rows);
void Installation_FK_RepositoryType_Binaries_getrows(vector <class Row_Installation*> *rows);
void RepositorySource_FK_RepositoryType_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_RepositoryType+ m_Description+ m_Define+ m_SourceOnly+ m_PathToFile+ m_Instructions+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_RepositoryType_asSQL();
string Description_asSQL();
string Define_asSQL();
string SourceOnly_asSQL();
string PathToFile_asSQL();
string Instructions_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

