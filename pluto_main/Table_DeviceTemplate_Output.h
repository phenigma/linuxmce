#ifndef __Table_DeviceTemplate_Output_H__
#define __Table_DeviceTemplate_Output_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_Output.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_Output
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_Output(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_Output")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_Output();

private:		
	friend class Row_DeviceTemplate_Output;
	struct Key
	{
		friend class Row_DeviceTemplate_Output;
		long int pk_FK_DeviceTemplate;
long int pk_FK_Output;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_Output);
	
		Key(class Row_DeviceTemplate_Output *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_Output::Key &key1, const Table_DeviceTemplate_Output::Key &key2) const;
	};	

	map<Table_DeviceTemplate_Output::Key, class Row_DeviceTemplate_Output*, Table_DeviceTemplate_Output::Key_Less> cachedRows;
	map<Table_DeviceTemplate_Output::Key, class Row_DeviceTemplate_Output*, Table_DeviceTemplate_Output::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_Output*> addedRows;
	vector<class Row_DeviceTemplate_Output*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_Output*> *rows);
	class Row_DeviceTemplate_Output* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_Output* GetRow(long int in_FK_DeviceTemplate, long int in_FK_Output);
	

private:	
	
		
	class Row_DeviceTemplate_Output* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_Output : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_Output::Key;
		friend class Table_DeviceTemplate_Output;
	private:
		Table_DeviceTemplate_Output *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_Output;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_Output_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_Output_set(long int val);
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
	
		Row_DeviceTemplate_Output(Table_DeviceTemplate_Output *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_Output *Table_DeviceTemplate_Output_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Output* FK_Output_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_Output+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_Output_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

