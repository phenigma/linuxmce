#ifndef __Table_Device_EntertainArea_H__
#define __Table_Device_EntertainArea_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_EntertainArea.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Device_EntertainArea
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Device_EntertainArea(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Device_EntertainArea")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Device_EntertainArea();

private:		
	friend class Row_Device_EntertainArea;
	struct Key
	{
		friend class Row_Device_EntertainArea;
		long int pk_FK_Device;
long int pk_FK_EntertainArea;

		
		Key(long int in_FK_Device, long int in_FK_EntertainArea);
	
		Key(class Row_Device_EntertainArea *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_EntertainArea::Key &key1, const Table_Device_EntertainArea::Key &key2) const;
	};	

	map<Table_Device_EntertainArea::Key, class Row_Device_EntertainArea*, Table_Device_EntertainArea::Key_Less> cachedRows;
	map<Table_Device_EntertainArea::Key, class Row_Device_EntertainArea*, Table_Device_EntertainArea::Key_Less> deleted_cachedRows;
	vector<class Row_Device_EntertainArea*> addedRows;
	vector<class Row_Device_EntertainArea*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Device_EntertainArea*> *rows);
	class Row_Device_EntertainArea* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_EntertainArea* GetRow(long int in_FK_Device, long int in_FK_EntertainArea);
	

private:	
	
		
	class Row_Device_EntertainArea* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Device_EntertainArea : public TableRow, public SerializeClass
	{
		friend struct Table_Device_EntertainArea::Key;
		friend class Table_Device_EntertainArea;
	private:
		Table_Device_EntertainArea *table;
		
		long int m_FK_Device;
long int m_FK_EntertainArea;
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
long int FK_EntertainArea_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Device_set(long int val);
void FK_EntertainArea_set(long int val);
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
	
		Row_Device_EntertainArea(Table_Device_EntertainArea *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_EntertainArea *Table_Device_EntertainArea_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_EntertainArea* FK_EntertainArea_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Device+ m_FK_EntertainArea+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Device_asSQL();
string FK_EntertainArea_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

