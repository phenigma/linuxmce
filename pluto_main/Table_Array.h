#ifndef __Table_Array_H__
#define __Table_Array_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Array.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Array
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Array(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Array")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Array();

private:		
	friend class Row_Array;
	struct Key
	{
		friend class Row_Array;
		long int pk_PK_Array;

		
		Key(long int in_PK_Array);
	
		Key(class Row_Array *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Array::Key &key1, const Table_Array::Key &key2) const;
	};	

	map<Table_Array::Key, class Row_Array*, Table_Array::Key_Less> cachedRows;
	map<Table_Array::Key, class Row_Array*, Table_Array::Key_Less> deleted_cachedRows;
	vector<class Row_Array*> addedRows;
	vector<class Row_Array*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Array*> *rows);
	class Row_Array* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Array* GetRow(long int in_PK_Array);
	

private:	
	
		
	class Row_Array* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Array : public TableRow, public SerializeClass
	{
		friend struct Table_Array::Key;
		friend class Table_Array;
	private:
		Table_Array *table;
		
		long int m_PK_Array;
string m_Description;
string m_Define;
short int m_CommandGroup;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Array_get();
string Description_get();
string Define_get();
short int CommandGroup_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Array_set(long int val);
void Description_set(string val);
void Define_set(string val);
void CommandGroup_set(short int val);
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
	
		Row_Array(Table_Array *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Array *Table_Array_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Array_getrows(vector <class Row_CommandGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Array+ m_Description+ m_Define+ m_CommandGroup+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Array_asSQL();
string Description_asSQL();
string Define_asSQL();
string CommandGroup_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

