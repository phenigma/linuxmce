#ifndef __Table_Device_CommandGroup_H__
#define __Table_Device_CommandGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_CommandGroup.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Device_CommandGroup
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Device_CommandGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Device_CommandGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Device_CommandGroup();

private:		
	friend class Row_Device_CommandGroup;
	struct Key
	{
		friend class Row_Device_CommandGroup;
		long int pk_FK_Device;
long int pk_FK_CommandGroup;

		
		Key(long int in_FK_Device, long int in_FK_CommandGroup);
	
		Key(class Row_Device_CommandGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_CommandGroup::Key &key1, const Table_Device_CommandGroup::Key &key2) const;
	};	

	map<Table_Device_CommandGroup::Key, class Row_Device_CommandGroup*, Table_Device_CommandGroup::Key_Less> cachedRows;
	map<Table_Device_CommandGroup::Key, class Row_Device_CommandGroup*, Table_Device_CommandGroup::Key_Less> deleted_cachedRows;
	vector<class Row_Device_CommandGroup*> addedRows;
	vector<class Row_Device_CommandGroup*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Device_CommandGroup*> *rows);
	class Row_Device_CommandGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_CommandGroup* GetRow(long int in_FK_Device, long int in_FK_CommandGroup);
	

private:	
	
		
	class Row_Device_CommandGroup* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Device_CommandGroup : public TableRow, public SerializeClass
	{
		friend struct Table_Device_CommandGroup::Key;
		friend class Table_Device_CommandGroup;
	private:
		Table_Device_CommandGroup *table;
		
		long int m_FK_Device;
long int m_FK_CommandGroup;
long int m_Sort;
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
long int FK_CommandGroup_get();
long int Sort_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Device_set(long int val);
void FK_CommandGroup_set(long int val);
void Sort_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Sort_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Sort_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Device_CommandGroup(Table_Device_CommandGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_CommandGroup *Table_Device_CommandGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_CommandGroup* FK_CommandGroup_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Device+ m_FK_CommandGroup+ m_Sort+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Device_asSQL();
string FK_CommandGroup_asSQL();
string Sort_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

