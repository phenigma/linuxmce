#ifndef __Table_DeviceGroup_H__
#define __Table_DeviceGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceGroup.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceGroup
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceGroup();

private:		
	friend class Row_DeviceGroup;
	struct Key
	{
		friend class Row_DeviceGroup;
		long int pk_PK_DeviceGroup;

		
		Key(long int in_PK_DeviceGroup);
	
		Key(class Row_DeviceGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceGroup::Key &key1, const Table_DeviceGroup::Key &key2) const;
	};	

	map<Table_DeviceGroup::Key, class Row_DeviceGroup*, Table_DeviceGroup::Key_Less> cachedRows;
	map<Table_DeviceGroup::Key, class Row_DeviceGroup*, Table_DeviceGroup::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceGroup*> addedRows;
	vector<class Row_DeviceGroup*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceGroup*> *rows);
	class Row_DeviceGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceGroup* GetRow(long int in_PK_DeviceGroup);
	

private:	
	
		
	class Row_DeviceGroup* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceGroup : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceGroup::Key;
		friend class Table_DeviceGroup;
	private:
		Table_DeviceGroup *table;
		
		long int m_PK_DeviceGroup;
long int m_FK_DeviceGroup_Parent;
long int m_FK_Installation;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DeviceGroup_get();
long int FK_DeviceGroup_Parent_get();
long int FK_Installation_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DeviceGroup_set(long int val);
void FK_DeviceGroup_Parent_set(long int val);
void FK_Installation_set(long int val);
void Description_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_DeviceGroup_Parent_isNull();
bool FK_Installation_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_DeviceGroup_Parent_setNull(bool val);
void FK_Installation_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceGroup(Table_DeviceGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceGroup *Table_DeviceGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceGroup* FK_DeviceGroup_Parent_getrow();
class Row_Installation* FK_Installation_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_DeviceGroup_getrows(vector <class Row_CommandGroup_Command*> *rows);
void DeviceGroup_FK_DeviceGroup_Parent_getrows(vector <class Row_DeviceGroup*> *rows);
void Device_DeviceGroup_FK_DeviceGroup_getrows(vector <class Row_Device_DeviceGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DeviceGroup+ m_FK_DeviceGroup_Parent+ m_FK_Installation+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceGroup_asSQL();
string FK_DeviceGroup_Parent_asSQL();
string FK_Installation_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

