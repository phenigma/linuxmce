#ifndef __Table_Device_DeviceData_H__
#define __Table_Device_DeviceData_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Device_DeviceData
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Device_DeviceData(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Device_DeviceData")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Device_DeviceData();

private:		
	friend class Row_Device_DeviceData;
	struct Key
	{
		friend class Row_Device_DeviceData;
		long int pk_FK_Device;
long int pk_FK_DeviceData;

		
		Key(long int in_FK_Device, long int in_FK_DeviceData);
	
		Key(class Row_Device_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_DeviceData::Key &key1, const Table_Device_DeviceData::Key &key2) const;
	};	

	map<Table_Device_DeviceData::Key, class Row_Device_DeviceData*, Table_Device_DeviceData::Key_Less> cachedRows;
	map<Table_Device_DeviceData::Key, class Row_Device_DeviceData*, Table_Device_DeviceData::Key_Less> deleted_cachedRows;
	vector<class Row_Device_DeviceData*> addedRows;
	vector<class Row_Device_DeviceData*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Device_DeviceData*> *rows);
	class Row_Device_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_DeviceData* GetRow(long int in_FK_Device, long int in_FK_DeviceData);
	

private:	
	
		
	class Row_Device_DeviceData* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Device_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_Device_DeviceData::Key;
		friend class Table_Device_DeviceData;
	private:
		Table_Device_DeviceData *table;
		
		long int m_FK_Device;
long int m_FK_DeviceData;
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
		long int FK_Device_get();
long int FK_DeviceData_get();
string Value_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Device_set(long int val);
void FK_DeviceData_set(long int val);
void Value_set(string val);
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
	
		Row_Device_DeviceData(Table_Device_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_DeviceData *Table_Device_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_DeviceData* FK_DeviceData_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Device+ m_FK_DeviceData+ m_Value+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Device_asSQL();
string FK_DeviceData_asSQL();
string Value_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

