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
long int m_FK_FloorplanDesignObjType;
string m_Description;
string m_Define;
long int m_Color;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_FloorplanDesignObjType_Color_get();
long int FK_FloorplanDesignObjType_get();
string Description_get();
string Define_get();
long int Color_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_FloorplanDesignObjType_Color_set(long int val);
void FK_FloorplanDesignObjType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Color_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_FloorplanObjectType_Color(Table_FloorplanObjectType_Color *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FloorplanObjectType_Color *Table_FloorplanObjectType_Color_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_FloorplanDesignObjType_Color+ m_FK_FloorplanDesignObjType+ m_Description+ m_Define+ m_Color+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_FloorplanDesignObjType_Color_asSQL();
string FK_FloorplanDesignObjType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Color_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

