#ifndef __Table_Version_H__
#define __Table_Version_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Version.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Version
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Version(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Version")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Version();

private:		
	friend class Row_Version;
	struct Key
	{
		friend class Row_Version;
		long int pk_PK_Version;

		
		Key(long int in_PK_Version);
	
		Key(class Row_Version *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Version::Key &key1, const Table_Version::Key &key2) const;
	};	

	map<Table_Version::Key, class Row_Version*, Table_Version::Key_Less> cachedRows;
	map<Table_Version::Key, class Row_Version*, Table_Version::Key_Less> deleted_cachedRows;
	vector<class Row_Version*> addedRows;
	vector<class Row_Version*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Version*> *rows);
	class Row_Version* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Version* GetRow(long int in_PK_Version);
	

private:	
	
		
	class Row_Version* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Version : public TableRow, public SerializeClass
	{
		friend struct Table_Version::Key;
		friend class Table_Version;
	private:
		Table_Version *table;
		
		long int m_PK_Version;
string m_Description;
string m_Define;
string m_Models;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Version_get();
string Description_get();
string Define_get();
string Models_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Version_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Models_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Description_isNull();
bool Define_isNull();
bool Models_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Description_setNull(bool val);
void Define_setNull(bool val);
void Models_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Version(Table_Version *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Version *Table_Version_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Installation_FK_Version_getrows(vector <class Row_Installation*> *rows);
void Orbiter_FK_Version_getrows(vector <class Row_Orbiter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Version+ m_Description+ m_Define+ m_Models+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Version_asSQL();
string Description_asSQL();
string Define_asSQL();
string Models_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

