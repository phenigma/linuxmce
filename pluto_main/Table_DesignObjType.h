#ifndef __Table_DesignObjType_H__
#define __Table_DesignObjType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DesignObjType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjType();

private:		
	friend class Row_DesignObjType;
	struct Key
	{
		friend class Row_DesignObjType;
		long int pk_PK_DesignObjType;

		
		Key(long int in_PK_DesignObjType);
	
		Key(class Row_DesignObjType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjType::Key &key1, const Table_DesignObjType::Key &key2) const;
	};	

	map<Table_DesignObjType::Key, class Row_DesignObjType*, Table_DesignObjType::Key_Less> cachedRows;
	map<Table_DesignObjType::Key, class Row_DesignObjType*, Table_DesignObjType::Key_Less> deleted_cachedRows;
	vector<class Row_DesignObjType*> addedRows;
	vector<class Row_DesignObjType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjType*> *rows);
	class Row_DesignObjType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjType* GetRow(long int in_PK_DesignObjType);
	

private:	
	
		
	class Row_DesignObjType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DesignObjType : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjType::Key;
		friend class Table_DesignObjType;
	private:
		Table_DesignObjType *table;
		
		long int m_PK_DesignObjType;
string m_Description;
string m_Define;
short int m_CanClick;
short int m_ContainsText;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DesignObjType_get();
string Description_get();
string Define_get();
short int CanClick_get();
short int ContainsText_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DesignObjType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void CanClick_set(short int val);
void ContainsText_set(short int val);
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
	
		Row_DesignObjType(Table_DesignObjType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjType *Table_DesignObjType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DesignObj_FK_DesignObjType_getrows(vector <class Row_DesignObj*> *rows);
void DesignObjType_DesignObjParameter_FK_DesignObjType_getrows(vector <class Row_DesignObjType_DesignObjParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DesignObjType+ m_Description+ m_Define+ m_CanClick+ m_ContainsText+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjType_asSQL();
string Description_asSQL();
string Define_asSQL();
string CanClick_asSQL();
string ContainsText_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

