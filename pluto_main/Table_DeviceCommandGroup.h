#ifndef __Table_DeviceCommandGroup_H__
#define __Table_DeviceCommandGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCommandGroup.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceCommandGroup
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCommandGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCommandGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCommandGroup();

private:		
	friend class Row_DeviceCommandGroup;
	struct Key
	{
		friend class Row_DeviceCommandGroup;
		long int pk_PK_DeviceCommandGroup;

		
		Key(long int in_PK_DeviceCommandGroup);
	
		Key(class Row_DeviceCommandGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCommandGroup::Key &key1, const Table_DeviceCommandGroup::Key &key2) const;
	};	

	map<Table_DeviceCommandGroup::Key, class Row_DeviceCommandGroup*, Table_DeviceCommandGroup::Key_Less> cachedRows;
	map<Table_DeviceCommandGroup::Key, class Row_DeviceCommandGroup*, Table_DeviceCommandGroup::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceCommandGroup*> addedRows;
	vector<class Row_DeviceCommandGroup*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCommandGroup*> *rows);
	class Row_DeviceCommandGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCommandGroup* GetRow(long int in_PK_DeviceCommandGroup);
	

private:	
	
		
	class Row_DeviceCommandGroup* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceCommandGroup : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCommandGroup::Key;
		friend class Table_DeviceCommandGroup;
	private:
		Table_DeviceCommandGroup *table;
		
		long int m_PK_DeviceCommandGroup;
long int m_FK_DeviceCategory;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DeviceCommandGroup_get();
long int FK_DeviceCategory_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DeviceCommandGroup_set(long int val);
void FK_DeviceCategory_set(long int val);
void Description_set(string val);
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
	
		Row_DeviceCommandGroup(Table_DeviceCommandGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCommandGroup *Table_DeviceCommandGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceCommandGroup_Command_FK_DeviceCommandGroup_getrows(vector <class Row_DeviceCommandGroup_Command*> *rows);
void DeviceCommandGroup_DeviceCommandGroup_Parent_FK_DeviceCommandGroup_getrows(vector <class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> *rows);
void DeviceCommandGroup_DeviceCommandGroup_Parent_FK_DeviceCommandGroup_Parent_getrows(vector <class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> *rows);
void DeviceTemplate_DeviceCommandGroup_FK_DeviceCommandGroup_getrows(vector <class Row_DeviceTemplate_DeviceCommandGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DeviceCommandGroup+ m_FK_DeviceCategory+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceCommandGroup_asSQL();
string FK_DeviceCategory_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

