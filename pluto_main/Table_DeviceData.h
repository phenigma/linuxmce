#ifndef __Table_DeviceData_H__
#define __Table_DeviceData_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceData
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceData(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceData")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceData();

private:		
	friend class Row_DeviceData;
	struct Key
	{
		friend class Row_DeviceData;
		long int pk_PK_DeviceData;

		
		Key(long int in_PK_DeviceData);
	
		Key(class Row_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceData::Key &key1, const Table_DeviceData::Key &key2) const;
	};	

	map<Table_DeviceData::Key, class Row_DeviceData*, Table_DeviceData::Key_Less> cachedRows;
	map<Table_DeviceData::Key, class Row_DeviceData*, Table_DeviceData::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceData*> addedRows;
	vector<class Row_DeviceData*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceData*> *rows);
	class Row_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceData* GetRow(long int in_PK_DeviceData);
	

private:	
	
		
	class Row_DeviceData* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceData::Key;
		friend class Table_DeviceData;
	private:
		Table_DeviceData *table;
		
		long int m_PK_DeviceData;
string m_Description;
string m_Define;
long int m_FK_ParameterType;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DeviceData_get();
string Description_get();
string Define_get();
long int FK_ParameterType_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DeviceData_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_ParameterType_set(long int val);
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
	
		Row_DeviceData(Table_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceData *Table_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ParameterType* FK_ParameterType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceCategory_DeviceData_FK_DeviceData_getrows(vector <class Row_DeviceCategory_DeviceData*> *rows);
void DeviceTemplate_DeviceData_FK_DeviceData_getrows(vector <class Row_DeviceTemplate_DeviceData*> *rows);
void Device_DeviceData_FK_DeviceData_getrows(vector <class Row_Device_DeviceData*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DeviceData+ m_Description+ m_Define+ m_FK_ParameterType+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceData_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_ParameterType_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

