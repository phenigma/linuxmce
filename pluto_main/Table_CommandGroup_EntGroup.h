#ifndef __Table_CommandGroup_EntGroup_H__
#define __Table_CommandGroup_EntGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_EntGroup.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CommandGroup_EntGroup
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CommandGroup_EntGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CommandGroup_EntGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CommandGroup_EntGroup();

private:		
	friend class Row_CommandGroup_EntGroup;
	struct Key
	{
		friend class Row_CommandGroup_EntGroup;
		long int pk_FK_CommandGroup;
long int pk_FK_Device;

		
		Key(long int in_FK_CommandGroup, long int in_FK_Device);
	
		Key(class Row_CommandGroup_EntGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_EntGroup::Key &key1, const Table_CommandGroup_EntGroup::Key &key2) const;
	};	

	map<Table_CommandGroup_EntGroup::Key, class Row_CommandGroup_EntGroup*, Table_CommandGroup_EntGroup::Key_Less> cachedRows;
	map<Table_CommandGroup_EntGroup::Key, class Row_CommandGroup_EntGroup*, Table_CommandGroup_EntGroup::Key_Less> deleted_cachedRows;
	vector<class Row_CommandGroup_EntGroup*> addedRows;
	vector<class Row_CommandGroup_EntGroup*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CommandGroup_EntGroup*> *rows);
	class Row_CommandGroup_EntGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_EntGroup* GetRow(long int in_FK_CommandGroup, long int in_FK_Device);
	

private:	
	
		
	class Row_CommandGroup_EntGroup* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CommandGroup_EntGroup : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_EntGroup::Key;
		friend class Table_CommandGroup_EntGroup;
	private:
		Table_CommandGroup_EntGroup *table;
		
		long int m_FK_CommandGroup;
long int m_FK_Device;
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
		long int FK_CommandGroup_get();
long int FK_Device_get();
long int Sort_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_CommandGroup_set(long int val);
void FK_Device_set(long int val);
void Sort_set(long int val);
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
	
		Row_CommandGroup_EntGroup(Table_CommandGroup_EntGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_EntGroup *Table_CommandGroup_EntGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandGroup* FK_CommandGroup_getrow();
class Row_Device* FK_Device_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_CommandGroup+ m_FK_Device+ m_Sort+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_CommandGroup_asSQL();
string FK_Device_asSQL();
string Sort_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

