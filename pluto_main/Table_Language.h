#ifndef __Table_Language_H__
#define __Table_Language_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Language.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Language : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Language(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Language")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Language();

private:		
	friend class Row_Language;
	struct Key
	{
		friend class Row_Language;
		long int pk_PK_Language;

		
		Key(long int in_PK_Language);
	
		Key(class Row_Language *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Language::Key &key1, const Table_Language::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Language*> *rows);
	class Row_Language* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Language* GetRow(long int in_PK_Language);
	

private:	
	
		
	class Row_Language* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Language : public TableRow, public SerializeClass
	{
		friend struct Table_Language::Key;
		friend class Table_Language;
	private:
		Table_Language *table;
		
		long int m_PK_Language;
string m_Description;
long int m_FK_Language_TextPlacement;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_Language_get();
string Description_get();
long int FK_Language_TextPlacement_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Language_set(long int val);
void Description_set(string val);
void FK_Language_TextPlacement_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Language_TextPlacement_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Language_TextPlacement_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Language(Table_Language *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Language *Table_Language_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Language* FK_Language_TextPlacement_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_Language_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);
void Language_FK_Language_TextPlacement_getrows(vector <class Row_Language*> *rows);
void Text_LS_FK_Language_getrows(vector <class Row_Text_LS*> *rows);
void Text_LS_AltVersions_FK_Language_getrows(vector <class Row_Text_LS_AltVersions*> *rows);
void Users_FK_Language_getrows(vector <class Row_Users*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Language+ m_Description+ m_FK_Language_TextPlacement+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Language_asSQL();
string Description_asSQL();
string FK_Language_TextPlacement_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

