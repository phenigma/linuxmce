#ifndef __Table_DeviceCommandGroup_DeviceCommandGroup_Parent_H__
#define __Table_DeviceCommandGroup_DeviceCommandGroup_Parent_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCommandGroup_DeviceCommandGroup_Parent.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceCommandGroup_DeviceCommandGroup_Parent
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCommandGroup_DeviceCommandGroup_Parent(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCommandGroup_DeviceCommandGroup_Parent")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCommandGroup_DeviceCommandGroup_Parent();

private:		
	friend class Row_DeviceCommandGroup_DeviceCommandGroup_Parent;
	struct Key
	{
		friend class Row_DeviceCommandGroup_DeviceCommandGroup_Parent;
		long int pk_FK_DeviceCommandGroup;
long int pk_FK_DeviceCommandGroup_Parent;

		
		Key(long int in_FK_DeviceCommandGroup, long int in_FK_DeviceCommandGroup_Parent);
	
		Key(class Row_DeviceCommandGroup_DeviceCommandGroup_Parent *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key &key1, const Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key &key2) const;
	};	

	map<Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key, class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*, Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key_Less> cachedRows;
	map<Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key, class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*, Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> addedRows;
	vector<class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCommandGroup_DeviceCommandGroup_Parent*> *rows);
	class Row_DeviceCommandGroup_DeviceCommandGroup_Parent* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCommandGroup_DeviceCommandGroup_Parent* GetRow(long int in_FK_DeviceCommandGroup, long int in_FK_DeviceCommandGroup_Parent);
	

private:	
	
		
	class Row_DeviceCommandGroup_DeviceCommandGroup_Parent* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceCommandGroup_DeviceCommandGroup_Parent : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCommandGroup_DeviceCommandGroup_Parent::Key;
		friend class Table_DeviceCommandGroup_DeviceCommandGroup_Parent;
	private:
		Table_DeviceCommandGroup_DeviceCommandGroup_Parent *table;
		
		long int m_FK_DeviceCommandGroup;
long int m_FK_DeviceCommandGroup_Parent;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceCommandGroup_get();
long int FK_DeviceCommandGroup_Parent_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceCommandGroup_set(long int val);
void FK_DeviceCommandGroup_Parent_set(long int val);
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
	
		Row_DeviceCommandGroup_DeviceCommandGroup_Parent(Table_DeviceCommandGroup_DeviceCommandGroup_Parent *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCommandGroup_DeviceCommandGroup_Parent *Table_DeviceCommandGroup_DeviceCommandGroup_Parent_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCommandGroup* FK_DeviceCommandGroup_getrow();
class Row_DeviceCommandGroup* FK_DeviceCommandGroup_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceCommandGroup+ m_FK_DeviceCommandGroup_Parent+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceCommandGroup_asSQL();
string FK_DeviceCommandGroup_Parent_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

