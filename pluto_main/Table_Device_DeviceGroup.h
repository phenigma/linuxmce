#ifndef __Table_Device_DeviceGroup_H__
#define __Table_Device_DeviceGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_DeviceGroup.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Device_DeviceGroup
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Device_DeviceGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Device_DeviceGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Device_DeviceGroup();

private:		
	friend class Row_Device_DeviceGroup;
	struct Key
	{
		friend class Row_Device_DeviceGroup;
		long int pk_FK_Device;
long int pk_FK_DeviceGroup;

		
		Key(long int in_FK_Device, long int in_FK_DeviceGroup);
	
		Key(class Row_Device_DeviceGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_DeviceGroup::Key &key1, const Table_Device_DeviceGroup::Key &key2) const;
	};	

	map<Table_Device_DeviceGroup::Key, class Row_Device_DeviceGroup*, Table_Device_DeviceGroup::Key_Less> cachedRows;
	map<Table_Device_DeviceGroup::Key, class Row_Device_DeviceGroup*, Table_Device_DeviceGroup::Key_Less> deleted_cachedRows;
	vector<class Row_Device_DeviceGroup*> addedRows;
	vector<class Row_Device_DeviceGroup*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Device_DeviceGroup*> *rows);
	class Row_Device_DeviceGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_DeviceGroup* GetRow(long int in_FK_Device, long int in_FK_DeviceGroup);
	

private:	
	
		
	class Row_Device_DeviceGroup* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Device_DeviceGroup : public TableRow, public SerializeClass
	{
		friend struct Table_Device_DeviceGroup::Key;
		friend class Table_Device_DeviceGroup;
	private:
		Table_Device_DeviceGroup *table;
		
		long int m_FK_Device;
long int m_FK_DeviceGroup;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Device_get();
long int FK_DeviceGroup_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Device_set(long int val);
void FK_DeviceGroup_set(long int val);
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
	
		Row_Device_DeviceGroup(Table_Device_DeviceGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_DeviceGroup *Table_Device_DeviceGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_DeviceGroup* FK_DeviceGroup_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Device+ m_FK_DeviceGroup+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Device_asSQL();
string FK_DeviceGroup_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

