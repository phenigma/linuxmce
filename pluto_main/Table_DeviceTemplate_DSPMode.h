#ifndef __Table_DeviceTemplate_DSPMode_H__
#define __Table_DeviceTemplate_DSPMode_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DSPMode.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_DSPMode
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_DSPMode(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_DSPMode")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_DSPMode();

private:		
	friend class Row_DeviceTemplate_DSPMode;
	struct Key
	{
		friend class Row_DeviceTemplate_DSPMode;
		long int pk_FK_DeviceTemplate;
long int pk_FK_DSPMode;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_DSPMode);
	
		Key(class Row_DeviceTemplate_DSPMode *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DSPMode::Key &key1, const Table_DeviceTemplate_DSPMode::Key &key2) const;
	};	

	map<Table_DeviceTemplate_DSPMode::Key, class Row_DeviceTemplate_DSPMode*, Table_DeviceTemplate_DSPMode::Key_Less> cachedRows;
	map<Table_DeviceTemplate_DSPMode::Key, class Row_DeviceTemplate_DSPMode*, Table_DeviceTemplate_DSPMode::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_DSPMode*> addedRows;
	vector<class Row_DeviceTemplate_DSPMode*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DSPMode*> *rows);
	class Row_DeviceTemplate_DSPMode* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DSPMode* GetRow(long int in_FK_DeviceTemplate, long int in_FK_DSPMode);
	

private:	
	
		
	class Row_DeviceTemplate_DSPMode* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_DSPMode : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DSPMode::Key;
		friend class Table_DeviceTemplate_DSPMode;
	private:
		Table_DeviceTemplate_DSPMode *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_DSPMode;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_DSPMode_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_DSPMode_set(long int val);
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
	
		Row_DeviceTemplate_DSPMode(Table_DeviceTemplate_DSPMode *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DSPMode *Table_DeviceTemplate_DSPMode_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DSPMode* FK_DSPMode_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_DSPMode+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_DSPMode_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

