#ifndef __Table_System_H__
#define __Table_System_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_System.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_System
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_System(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("System")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_System();

private:		
	friend class Row_System;
	struct Key
	{
		friend class Row_System;
		long int pk_PK_System;

		
		Key(long int in_PK_System);
	
		Key(class Row_System *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_System::Key &key1, const Table_System::Key &key2) const;
	};	

	map<Table_System::Key, class Row_System*, Table_System::Key_Less> cachedRows;
	map<Table_System::Key, class Row_System*, Table_System::Key_Less> deleted_cachedRows;
	vector<class Row_System*> addedRows;
	vector<class Row_System*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_System*> *rows);
	class Row_System* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_System* GetRow(long int in_PK_System);
	

private:	
	
		
	class Row_System* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_System : public TableRow, public SerializeClass
	{
		friend struct Table_System::Key;
		friend class Table_System;
	private:
		Table_System *table;
		
		long int m_PK_System;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_System_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_System_set(long int val);
void Description_set(string val);
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
	
		Row_System(Table_System *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_System *Table_System_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Orbiter_FK_System_getrows(vector <class Row_Orbiter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_System+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_System_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

