#ifndef __Table_DeviceCategory_DeviceObj_H__
#define __Table_DeviceCategory_DeviceObj_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCategory_DeviceObj.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceCategory_DeviceObj
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCategory_DeviceObj(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCategory_DeviceObj")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCategory_DeviceObj();

private:		
	friend class Row_DeviceCategory_DeviceObj;
	struct Key
	{
		friend class Row_DeviceCategory_DeviceObj;
		long int pk_FK_DeviceCategory;
long int pk_FK_DesignObj;

		
		Key(long int in_FK_DeviceCategory, long int in_FK_DesignObj);
	
		Key(class Row_DeviceCategory_DeviceObj *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCategory_DeviceObj::Key &key1, const Table_DeviceCategory_DeviceObj::Key &key2) const;
	};	

	map<Table_DeviceCategory_DeviceObj::Key, class Row_DeviceCategory_DeviceObj*, Table_DeviceCategory_DeviceObj::Key_Less> cachedRows;
	map<Table_DeviceCategory_DeviceObj::Key, class Row_DeviceCategory_DeviceObj*, Table_DeviceCategory_DeviceObj::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceCategory_DeviceObj*> addedRows;
	vector<class Row_DeviceCategory_DeviceObj*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCategory_DeviceObj*> *rows);
	class Row_DeviceCategory_DeviceObj* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCategory_DeviceObj* GetRow(long int in_FK_DeviceCategory, long int in_FK_DesignObj);
	

private:	
	
		
	class Row_DeviceCategory_DeviceObj* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceCategory_DeviceObj : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCategory_DeviceObj::Key;
		friend class Table_DeviceCategory_DeviceObj;
	private:
		Table_DeviceCategory_DeviceObj *table;
		
		long int m_FK_DeviceCategory;
long int m_FK_DesignObj;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceCategory_get();
long int FK_DesignObj_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceCategory_set(long int val);
void FK_DesignObj_set(long int val);
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
	
		Row_DeviceCategory_DeviceObj(Table_DeviceCategory_DeviceObj *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCategory_DeviceObj *Table_DeviceCategory_DeviceObj_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_DesignObj* FK_DesignObj_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceCategory+ m_FK_DesignObj+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceCategory_asSQL();
string FK_DesignObj_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

