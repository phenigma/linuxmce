#ifndef __Table_File_H__
#define __Table_File_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_File.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_File : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_File(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("File")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_File();

private:		
	friend class Row_File;
	struct Key
	{
		friend class Row_File;
		long int pk_PK_File;

		
		Key(long int in_PK_File);
	
		Key(class Row_File *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_File::Key &key1, const Table_File::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_File*> *rows);
	class Row_File* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_File* GetRow(long int in_PK_File);
	

private:	
	
		
	class Row_File* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_File : public TableRow, public SerializeClass
	{
		friend struct Table_File::Key;
		friend class Table_File;
	private:
		Table_File *table;
		
		long int m_PK_File;
long int m_FK_Type;
string m_Path;
long int m_Missing;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_File_get();
long int FK_Type_get();
string Path_get();
long int Missing_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_File_set(long int val);
void FK_Type_set(long int val);
void Path_set(string val);
void Missing_set(long int val);
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
	
		Row_File(Table_File *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_File *Table_File_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Type* FK_Type_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void File_Attribute_FK_File_getrows(vector <class Row_File_Attribute*> *rows);
void Picture_File_FK_File_getrows(vector <class Row_Picture_File*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_File+ m_FK_Type+ m_Path+ m_Missing+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_File_asSQL();
string FK_Type_asSQL();
string Path_asSQL();
string Missing_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

