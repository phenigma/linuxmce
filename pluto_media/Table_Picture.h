#ifndef __Table_Picture_H__
#define __Table_Picture_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Picture.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Picture : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Picture(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Picture")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Picture();

private:		
	friend class Row_Picture;
	struct Key
	{
		friend class Row_Picture;
		long int pk_PK_Picture;

		
		Key(long int in_PK_Picture);
	
		Key(class Row_Picture *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Picture::Key &key1, const Table_Picture::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Picture*> *rows);
	class Row_Picture* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Picture* GetRow(long int in_PK_Picture);
	

private:	
	
		
	class Row_Picture* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Picture : public TableRow, public SerializeClass
	{
		friend struct Table_Picture::Key;
		friend class Table_Picture;
	private:
		Table_Picture *table;
		
		long int m_PK_Picture;
string m_Extension;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int PK_Picture_get();
string Extension_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Picture_set(long int val);
void Extension_set(string val);
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
	
		Row_Picture(Table_Picture *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Picture *Table_Picture_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Picture_Attribute_FK_Picture_getrows(vector <class Row_Picture_Attribute*> *rows);
void Picture_File_FK_Picture_getrows(vector <class Row_Picture_File*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Picture+ m_Extension+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Picture_asSQL();
string Extension_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

