#ifndef __Table_UserMode_H__
#define __Table_UserMode_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_UserMode.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_UserMode
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_UserMode(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("UserMode")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_UserMode();

private:		
	friend class Row_UserMode;
	struct Key
	{
		friend class Row_UserMode;
		long int pk_PK_UserMode;

		
		Key(long int in_PK_UserMode);
	
		Key(class Row_UserMode *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_UserMode::Key &key1, const Table_UserMode::Key &key2) const;
	};	

	map<Table_UserMode::Key, class Row_UserMode*, Table_UserMode::Key_Less> cachedRows;
	map<Table_UserMode::Key, class Row_UserMode*, Table_UserMode::Key_Less> deleted_cachedRows;
	vector<class Row_UserMode*> addedRows;
	vector<class Row_UserMode*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_UserMode*> *rows);
	class Row_UserMode* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_UserMode* GetRow(long int in_PK_UserMode);
	

private:	
	
		
	class Row_UserMode* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_UserMode : public TableRow, public SerializeClass
	{
		friend struct Table_UserMode::Key;
		friend class Table_UserMode;
	private:
		Table_UserMode *table;
		
		long int m_PK_UserMode;
string m_Description;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_UserMode_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_UserMode_set(long int val);
void Description_set(string val);
void Define_set(string val);
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
	
		Row_UserMode(Table_UserMode *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_UserMode *Table_UserMode_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_UserMode+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_UserMode_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

