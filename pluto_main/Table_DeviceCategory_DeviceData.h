#ifndef __Table_DeviceCategory_DeviceData_H__
#define __Table_DeviceCategory_DeviceData_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCategory_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceCategory_DeviceData
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCategory_DeviceData(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCategory_DeviceData")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCategory_DeviceData();

private:		
	friend class Row_DeviceCategory_DeviceData;
	struct Key
	{
		friend class Row_DeviceCategory_DeviceData;
		long int pk_FK_DeviceCategory;
long int pk_FK_DeviceData;

		
		Key(long int in_FK_DeviceCategory, long int in_FK_DeviceData);
	
		Key(class Row_DeviceCategory_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCategory_DeviceData::Key &key1, const Table_DeviceCategory_DeviceData::Key &key2) const;
	};	

	map<Table_DeviceCategory_DeviceData::Key, class Row_DeviceCategory_DeviceData*, Table_DeviceCategory_DeviceData::Key_Less> cachedRows;
	map<Table_DeviceCategory_DeviceData::Key, class Row_DeviceCategory_DeviceData*, Table_DeviceCategory_DeviceData::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceCategory_DeviceData*> addedRows;
	vector<class Row_DeviceCategory_DeviceData*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCategory_DeviceData*> *rows);
	class Row_DeviceCategory_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCategory_DeviceData* GetRow(long int in_FK_DeviceCategory, long int in_FK_DeviceData);
	

private:	
	
		
	class Row_DeviceCategory_DeviceData* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceCategory_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCategory_DeviceData::Key;
		friend class Table_DeviceCategory_DeviceData;
	private:
		Table_DeviceCategory_DeviceData *table;
		
		long int m_FK_DeviceCategory;
long int m_FK_DeviceData;
string m_DefaultValue;
short int m_Required;
short int m_SetByDevice;
short int m_AllowedToModify;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[11];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceCategory_get();
long int FK_DeviceData_get();
string DefaultValue_get();
short int Required_get();
short int SetByDevice_get();
short int AllowedToModify_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceCategory_set(long int val);
void FK_DeviceData_set(long int val);
void DefaultValue_set(string val);
void Required_set(short int val);
void SetByDevice_set(short int val);
void AllowedToModify_set(short int val);
void Description_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool DefaultValue_isNull();
bool Description_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void DefaultValue_setNull(bool val);
void Description_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceCategory_DeviceData(Table_DeviceCategory_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCategory_DeviceData *Table_DeviceCategory_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_DeviceData* FK_DeviceData_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceCategory+ m_FK_DeviceData+ m_DefaultValue+ m_Required+ m_SetByDevice+ m_AllowedToModify+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceCategory_asSQL();
string FK_DeviceData_asSQL();
string DefaultValue_asSQL();
string Required_asSQL();
string SetByDevice_asSQL();
string AllowedToModify_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

