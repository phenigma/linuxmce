#ifndef __Table_Text_H__
#define __Table_Text_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Text.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Text : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Text(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Text")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Text();

private:		
	friend class Row_Text;
	struct Key
	{
		friend class Row_Text;
		long int pk_PK_Text;

		
		Key(long int in_PK_Text);
	
		Key(class Row_Text *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Text::Key &key1, const Table_Text::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Text*> *rows);
	class Row_Text* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Text* GetRow(long int in_PK_Text);
	

private:	
	
		
	class Row_Text* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Text : public TableRow, public SerializeClass
	{
		friend struct Table_Text::Key;
		friend class Table_Text;
	private:
		Table_Text *table;
		
		long int m_PK_Text;
long int m_FK_TextCategory;
string m_Description;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Text_get();
long int FK_TextCategory_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Text_set(long int val);
void FK_TextCategory_set(long int val);
void Description_set(string val);
void Define_set(string val);
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
	
		Row_Text(Table_Text *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Text *Table_Text_get() { return table; };

		// Return the rows for foreign keys 
		class Row_TextCategory* FK_TextCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_FK_Text_getrows(vector <class Row_DesignObjVariation_Text*> *rows);
void DesignObjVariation_Text_FK_Text_OverrideFromHeader_getrows(vector <class Row_DesignObjVariation_Text*> *rows);
void Text_LS_FK_Text_getrows(vector <class Row_Text_LS*> *rows);
void Text_LS_AltVersions_FK_Text_getrows(vector <class Row_Text_LS_AltVersions*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Text+ m_FK_TextCategory+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Text_asSQL();
string FK_TextCategory_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

