#ifndef __Table_Orbiter_Users_PasswordReq_H__
#define __Table_Orbiter_Users_PasswordReq_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter_Users_PasswordReq.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Orbiter_Users_PasswordReq
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Orbiter_Users_PasswordReq(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Orbiter_Users_PasswordReq")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Orbiter_Users_PasswordReq();

private:		
	friend class Row_Orbiter_Users_PasswordReq;
	struct Key
	{
		friend class Row_Orbiter_Users_PasswordReq;
		long int pk_FK_Orbiter;
long int pk_FK_Users;

		
		Key(long int in_FK_Orbiter, long int in_FK_Users);
	
		Key(class Row_Orbiter_Users_PasswordReq *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter_Users_PasswordReq::Key &key1, const Table_Orbiter_Users_PasswordReq::Key &key2) const;
	};	

	map<Table_Orbiter_Users_PasswordReq::Key, class Row_Orbiter_Users_PasswordReq*, Table_Orbiter_Users_PasswordReq::Key_Less> cachedRows;
	map<Table_Orbiter_Users_PasswordReq::Key, class Row_Orbiter_Users_PasswordReq*, Table_Orbiter_Users_PasswordReq::Key_Less> deleted_cachedRows;
	vector<class Row_Orbiter_Users_PasswordReq*> addedRows;
	vector<class Row_Orbiter_Users_PasswordReq*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Orbiter_Users_PasswordReq*> *rows);
	class Row_Orbiter_Users_PasswordReq* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter_Users_PasswordReq* GetRow(long int in_FK_Orbiter, long int in_FK_Users);
	

private:	
	
		
	class Row_Orbiter_Users_PasswordReq* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Orbiter_Users_PasswordReq : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter_Users_PasswordReq::Key;
		friend class Table_Orbiter_Users_PasswordReq;
	private:
		Table_Orbiter_Users_PasswordReq *table;
		
		long int m_FK_Orbiter;
long int m_FK_Users;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Orbiter_get();
long int FK_Users_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Orbiter_set(long int val);
void FK_Users_set(long int val);
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
	
		Row_Orbiter_Users_PasswordReq(Table_Orbiter_Users_PasswordReq *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter_Users_PasswordReq *Table_Orbiter_Users_PasswordReq_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Orbiter* FK_Orbiter_getrow();
class Row_Users* FK_Users_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Orbiter+ m_FK_Users+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Orbiter_asSQL();
string FK_Users_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

