#ifndef __Table_DesignObjVariation_Text_H__
#define __Table_DesignObjVariation_Text_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_Text.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DesignObjVariation_Text : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjVariation_Text(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjVariation_Text")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjVariation_Text();

private:		
	friend class Row_DesignObjVariation_Text;
	struct Key
	{
		friend class Row_DesignObjVariation_Text;
		long int pk_PK_DesignObjVariation_Text;

		
		Key(long int in_PK_DesignObjVariation_Text);
	
		Key(class Row_DesignObjVariation_Text *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_Text::Key &key1, const Table_DesignObjVariation_Text::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_Text*> *rows);
	class Row_DesignObjVariation_Text* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_Text* GetRow(long int in_PK_DesignObjVariation_Text);
	

private:	
	
		
	class Row_DesignObjVariation_Text* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation_Text : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_Text::Key;
		friend class Table_DesignObjVariation_Text;
	private:
		Table_DesignObjVariation_Text *table;
		
		long int m_PK_DesignObjVariation_Text;
long int m_FK_DesignObjVariation;
long int m_FK_Text;
long int m_FK_Text_OverrideFromHeader;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_DesignObjVariation_Text_get();
long int FK_DesignObjVariation_get();
long int FK_Text_get();
long int FK_Text_OverrideFromHeader_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DesignObjVariation_Text_set(long int val);
void FK_DesignObjVariation_set(long int val);
void FK_Text_set(long int val);
void FK_Text_OverrideFromHeader_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Text_OverrideFromHeader_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Text_OverrideFromHeader_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_Text(Table_DesignObjVariation_Text *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_Text *Table_DesignObjVariation_Text_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_Text* FK_Text_getrow();
class Row_Text* FK_Text_OverrideFromHeader_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObjVariation_Text+ m_FK_DesignObjVariation+ m_FK_Text+ m_FK_Text_OverrideFromHeader+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_Text_asSQL();
string FK_DesignObjVariation_asSQL();
string FK_Text_asSQL();
string FK_Text_OverrideFromHeader_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

