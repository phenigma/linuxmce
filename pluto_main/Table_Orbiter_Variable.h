#ifndef __Table_Orbiter_Variable_H__
#define __Table_Orbiter_Variable_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter_Variable.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Orbiter_Variable
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Orbiter_Variable(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Orbiter_Variable")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Orbiter_Variable();

private:		
	friend class Row_Orbiter_Variable;
	struct Key
	{
		friend class Row_Orbiter_Variable;
		long int pk_FK_Orbiter;
long int pk_FK_Variable;

		
		Key(long int in_FK_Orbiter, long int in_FK_Variable);
	
		Key(class Row_Orbiter_Variable *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter_Variable::Key &key1, const Table_Orbiter_Variable::Key &key2) const;
	};	

	map<Table_Orbiter_Variable::Key, class Row_Orbiter_Variable*, Table_Orbiter_Variable::Key_Less> cachedRows;
	map<Table_Orbiter_Variable::Key, class Row_Orbiter_Variable*, Table_Orbiter_Variable::Key_Less> deleted_cachedRows;
	vector<class Row_Orbiter_Variable*> addedRows;
	vector<class Row_Orbiter_Variable*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Orbiter_Variable*> *rows);
	class Row_Orbiter_Variable* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter_Variable* GetRow(long int in_FK_Orbiter, long int in_FK_Variable);
	

private:	
	
		
	class Row_Orbiter_Variable* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Orbiter_Variable : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter_Variable::Key;
		friend class Table_Orbiter_Variable;
	private:
		Table_Orbiter_Variable *table;
		
		long int m_FK_Orbiter;
long int m_FK_Variable;
string m_Value;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Orbiter_get();
long int FK_Variable_get();
string Value_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Orbiter_set(long int val);
void FK_Variable_set(long int val);
void Value_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Value_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Value_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Orbiter_Variable(Table_Orbiter_Variable *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter_Variable *Table_Orbiter_Variable_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Orbiter* FK_Orbiter_getrow();
class Row_Variable* FK_Variable_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Orbiter+ m_FK_Variable+ m_Value+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Orbiter_asSQL();
string FK_Variable_asSQL();
string Value_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

