#ifndef __Table_DesignObjCategory_H__
#define __Table_DesignObjCategory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjCategory.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DesignObjCategory
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjCategory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjCategory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjCategory();

private:		
	friend class Row_DesignObjCategory;
	struct Key
	{
		friend class Row_DesignObjCategory;
		long int pk_PK_DesignObjCategory;

		
		Key(long int in_PK_DesignObjCategory);
	
		Key(class Row_DesignObjCategory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjCategory::Key &key1, const Table_DesignObjCategory::Key &key2) const;
	};	

	map<Table_DesignObjCategory::Key, class Row_DesignObjCategory*, Table_DesignObjCategory::Key_Less> cachedRows;
	map<Table_DesignObjCategory::Key, class Row_DesignObjCategory*, Table_DesignObjCategory::Key_Less> deleted_cachedRows;
	vector<class Row_DesignObjCategory*> addedRows;
	vector<class Row_DesignObjCategory*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjCategory*> *rows);
	class Row_DesignObjCategory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjCategory* GetRow(long int in_PK_DesignObjCategory);
	

private:	
	
		
	class Row_DesignObjCategory* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DesignObjCategory : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjCategory::Key;
		friend class Table_DesignObjCategory;
	private:
		Table_DesignObjCategory *table;
		
		long int m_PK_DesignObjCategory;
long int m_FK_DesignObjCategory_Parent;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DesignObjCategory_get();
long int FK_DesignObjCategory_Parent_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DesignObjCategory_set(long int val);
void FK_DesignObjCategory_Parent_set(long int val);
void Description_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_DesignObjCategory_Parent_isNull();
bool Description_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_DesignObjCategory_Parent_setNull(bool val);
void Description_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjCategory(Table_DesignObjCategory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjCategory *Table_DesignObjCategory_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjCategory* FK_DesignObjCategory_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObj_FK_DesignObjCategory_getrows(vector <class Row_DesignObj*> *rows);
void DesignObjCategory_FK_DesignObjCategory_Parent_getrows(vector <class Row_DesignObjCategory*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DesignObjCategory+ m_FK_DesignObjCategory_Parent+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjCategory_asSQL();
string FK_DesignObjCategory_Parent_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

