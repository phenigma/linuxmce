#ifndef __Table_FloorplanObjectType_Color_H__
#define __Table_FloorplanObjectType_Color_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_FloorplanObjectType_Color.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_FloorplanObjectType_Color
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_FloorplanObjectType_Color(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("FloorplanObjectType_Color")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_FloorplanObjectType_Color();

private:		
	friend class Row_FloorplanObjectType_Color;
	struct Key
	{
		friend class Row_FloorplanObjectType_Color;
		long int pk_PK_FloorplanDesignObjType_Color;

		
		Key(long int in_PK_FloorplanDesignObjType_Color);
	
		Key(class Row_FloorplanObjectType_Color *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_FloorplanObjectType_Color::Key &key1, const Table_FloorplanObjectType_Color::Key &key2) const;
	};	

	map<Table_FloorplanObjectType_Color::Key, class Row_FloorplanObjectType_Color*, Table_FloorplanObjectType_Color::Key_Less> cachedRows;
	map<Table_FloorplanObjectType_Color::Key, class Row_FloorplanObjectType_Color*, Table_FloorplanObjectType_Color::Key_Less> deleted_cachedRows;
	vector<class Row_FloorplanObjectType_Color*> addedRows;
	vector<class Row_FloorplanObjectType_Color*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_FloorplanObjectType_Color*> *rows);
	class Row_FloorplanObjectType_Color* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_FloorplanObjectType_Color* GetRow(long int in_PK_FloorplanDesignObjType_Color);
	

private:	
	
		
	class Row_FloorplanObjectType_Color* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_FloorplanObjectType_Color : public TableRow, public SerializeClass
	{
		friend struct Table_FloorplanObjectType_Color::Key;
		friend class Table_FloorplanObjectType_Color;
	private:
		Table_FloorplanObjectType_Color *table;
		
		long int m_PK_FloorplanDesignObjType_Color;
long int m_FK_DesignObjType_Floorplan;
string m_Description;
string m_Define;
long int m_Color;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_FloorplanDesignObjType_Color_get();
long int FK_DesignObjType_Floorplan_get();
string Description_get();
string Define_get();
long int Color_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_FloorplanDesignObjType_Color_set(long int val);
void FK_DesignObjType_Floorplan_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Color_set(long int val);
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
	
		Row_FloorplanObjectType_Color(Table_FloorplanObjectType_Color *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FloorplanObjectType_Color *Table_FloorplanObjectType_Color_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjType* FK_DesignObjType_Floorplan_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_FloorplanDesignObjType_Color+ m_FK_DesignObjType_Floorplan+ m_Description+ m_Define+ m_Color+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_FloorplanDesignObjType_Color_asSQL();
string FK_DesignObjType_Floorplan_asSQL();
string Description_asSQL();
string Define_asSQL();
string Color_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

