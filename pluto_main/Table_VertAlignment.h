#ifndef __Table_VertAlignment_H__
#define __Table_VertAlignment_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_VertAlignment.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_VertAlignment
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_VertAlignment(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("VertAlignment")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_VertAlignment();

private:		
	friend class Row_VertAlignment;
	struct Key
	{
		friend class Row_VertAlignment;
		long int pk_PK_VertAlignment;

		
		Key(long int in_PK_VertAlignment);
	
		Key(class Row_VertAlignment *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_VertAlignment::Key &key1, const Table_VertAlignment::Key &key2) const;
	};	

	map<Table_VertAlignment::Key, class Row_VertAlignment*, Table_VertAlignment::Key_Less> cachedRows;
	map<Table_VertAlignment::Key, class Row_VertAlignment*, Table_VertAlignment::Key_Less> deleted_cachedRows;
	vector<class Row_VertAlignment*> addedRows;
	vector<class Row_VertAlignment*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_VertAlignment*> *rows);
	class Row_VertAlignment* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_VertAlignment* GetRow(long int in_PK_VertAlignment);
	

private:	
	
		
	class Row_VertAlignment* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_VertAlignment : public TableRow, public SerializeClass
	{
		friend struct Table_VertAlignment::Key;
		friend class Table_VertAlignment;
	private:
		Table_VertAlignment *table;
		
		long int m_PK_VertAlignment;
string m_Description;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_VertAlignment_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_VertAlignment_set(long int val);
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
	
		Row_VertAlignment(Table_VertAlignment *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_VertAlignment *Table_VertAlignment_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_VertAlignment_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);
void StyleVariation_FK_VertAlignment_getrows(vector <class Row_StyleVariation*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_VertAlignment+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_VertAlignment_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

