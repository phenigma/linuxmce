#ifndef __Table_DeviceTemplate_MediaType_DesignObj_H__
#define __Table_DeviceTemplate_MediaType_DesignObj_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_MediaType_DesignObj.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_MediaType_DesignObj
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_MediaType_DesignObj(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_MediaType_DesignObj")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_MediaType_DesignObj();

private:		
	friend class Row_DeviceTemplate_MediaType_DesignObj;
	struct Key
	{
		friend class Row_DeviceTemplate_MediaType_DesignObj;
		long int pk_FK_DeviceTemplate_MediaType;
long int pk_FK_DesignObj;

		
		Key(long int in_FK_DeviceTemplate_MediaType, long int in_FK_DesignObj);
	
		Key(class Row_DeviceTemplate_MediaType_DesignObj *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_MediaType_DesignObj::Key &key1, const Table_DeviceTemplate_MediaType_DesignObj::Key &key2) const;
	};	

	map<Table_DeviceTemplate_MediaType_DesignObj::Key, class Row_DeviceTemplate_MediaType_DesignObj*, Table_DeviceTemplate_MediaType_DesignObj::Key_Less> cachedRows;
	map<Table_DeviceTemplate_MediaType_DesignObj::Key, class Row_DeviceTemplate_MediaType_DesignObj*, Table_DeviceTemplate_MediaType_DesignObj::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_MediaType_DesignObj*> addedRows;
	vector<class Row_DeviceTemplate_MediaType_DesignObj*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_MediaType_DesignObj*> *rows);
	class Row_DeviceTemplate_MediaType_DesignObj* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_MediaType_DesignObj* GetRow(long int in_FK_DeviceTemplate_MediaType, long int in_FK_DesignObj);
	

private:	
	
		
	class Row_DeviceTemplate_MediaType_DesignObj* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_MediaType_DesignObj : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_MediaType_DesignObj::Key;
		friend class Table_DeviceTemplate_MediaType_DesignObj;
	private:
		Table_DeviceTemplate_MediaType_DesignObj *table;
		
		long int m_FK_DeviceTemplate_MediaType;
long int m_FK_DesignObj;
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
		long int FK_DeviceTemplate_MediaType_get();
long int FK_DesignObj_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceTemplate_MediaType_set(long int val);
void FK_DesignObj_set(long int val);
void Description_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Description_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Description_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_MediaType_DesignObj(Table_DeviceTemplate_MediaType_DesignObj *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_MediaType_DesignObj *Table_DeviceTemplate_MediaType_DesignObj_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate_MediaType* FK_DeviceTemplate_MediaType_getrow();
class Row_DesignObj* FK_DesignObj_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate_MediaType+ m_FK_DesignObj+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_MediaType_asSQL();
string FK_DesignObj_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

