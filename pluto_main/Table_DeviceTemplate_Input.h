#ifndef __Table_DeviceTemplate_Input_H__
#define __Table_DeviceTemplate_Input_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_Input.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_Input
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_Input(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_Input")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_Input();

private:		
	friend class Row_DeviceTemplate_Input;
	struct Key
	{
		friend class Row_DeviceTemplate_Input;
		long int pk_FK_DeviceTemplate;
long int pk_FK_Input;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_Input);
	
		Key(class Row_DeviceTemplate_Input *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_Input::Key &key1, const Table_DeviceTemplate_Input::Key &key2) const;
	};	

	map<Table_DeviceTemplate_Input::Key, class Row_DeviceTemplate_Input*, Table_DeviceTemplate_Input::Key_Less> cachedRows;
	map<Table_DeviceTemplate_Input::Key, class Row_DeviceTemplate_Input*, Table_DeviceTemplate_Input::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_Input*> addedRows;
	vector<class Row_DeviceTemplate_Input*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_Input*> *rows);
	class Row_DeviceTemplate_Input* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_Input* GetRow(long int in_FK_DeviceTemplate, long int in_FK_Input);
	

private:	
	
		
	class Row_DeviceTemplate_Input* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_Input : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_Input::Key;
		friend class Table_DeviceTemplate_Input;
	private:
		Table_DeviceTemplate_Input *table;
		
		long int m_PK_DeviceTemplate_Input;
long int m_FK_DeviceTemplate;
long int m_FK_Input;
long int m_FK_ConnectorType;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DeviceTemplate_Input_get();
long int FK_DeviceTemplate_get();
long int FK_Input_get();
long int FK_ConnectorType_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DeviceTemplate_Input_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_Input_set(long int val);
void FK_ConnectorType_set(long int val);
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
	
		Row_DeviceTemplate_Input(Table_DeviceTemplate_Input *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_Input *Table_DeviceTemplate_Input_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Input* FK_Input_getrow();
class Row_ConnectorType* FK_ConnectorType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DeviceTemplate_Input+ m_FK_DeviceTemplate+ m_FK_Input+ m_FK_ConnectorType+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceTemplate_Input_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_Input_asSQL();
string FK_ConnectorType_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

