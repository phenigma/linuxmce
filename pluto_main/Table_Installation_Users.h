#ifndef __Table_Installation_Users_H__
#define __Table_Installation_Users_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Installation_Users.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Installation_Users
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Installation_Users(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Installation_Users")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Installation_Users();

private:		
	friend class Row_Installation_Users;
	struct Key
	{
		friend class Row_Installation_Users;
		long int pk_FK_Installation;
long int pk_FK_Users;

		
		Key(long int in_FK_Installation, long int in_FK_Users);
	
		Key(class Row_Installation_Users *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Installation_Users::Key &key1, const Table_Installation_Users::Key &key2) const;
	};	

	map<Table_Installation_Users::Key, class Row_Installation_Users*, Table_Installation_Users::Key_Less> cachedRows;
	map<Table_Installation_Users::Key, class Row_Installation_Users*, Table_Installation_Users::Key_Less> deleted_cachedRows;
	vector<class Row_Installation_Users*> addedRows;
	vector<class Row_Installation_Users*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Installation_Users*> *rows);
	class Row_Installation_Users* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Installation_Users* GetRow(long int in_FK_Installation, long int in_FK_Users);
	

private:	
	
		
	class Row_Installation_Users* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Installation_Users : public TableRow, public SerializeClass
	{
		friend struct Table_Installation_Users::Key;
		friend class Table_Installation_Users;
	private:
		Table_Installation_Users *table;
		
		long int m_FK_Installation;
long int m_FK_Users;
short int m_userCanModifyInstallation;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Installation_get();
long int FK_Users_get();
short int userCanModifyInstallation_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Installation_set(long int val);
void FK_Users_set(long int val);
void userCanModifyInstallation_set(short int val);
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
	
		Row_Installation_Users(Table_Installation_Users *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Installation_Users *Table_Installation_Users_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Installation* FK_Installation_getrow();
class Row_Users* FK_Users_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Installation+ m_FK_Users+ m_userCanModifyInstallation+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Installation_asSQL();
string FK_Users_asSQL();
string userCanModifyInstallation_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

