#ifndef __Table_Pipe_H__
#define __Table_Pipe_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Pipe.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Pipe
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Pipe(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Pipe")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Pipe();

private:		
	friend class Row_Pipe;
	struct Key
	{
		friend class Row_Pipe;
		long int pk_PK_Pipe;

		
		Key(long int in_PK_Pipe);
	
		Key(class Row_Pipe *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Pipe::Key &key1, const Table_Pipe::Key &key2) const;
	};	

	map<Table_Pipe::Key, class Row_Pipe*, Table_Pipe::Key_Less> cachedRows;
	map<Table_Pipe::Key, class Row_Pipe*, Table_Pipe::Key_Less> deleted_cachedRows;
	vector<class Row_Pipe*> addedRows;
	vector<class Row_Pipe*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Pipe*> *rows);
	class Row_Pipe* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Pipe* GetRow(long int in_PK_Pipe);
	

private:	
	
		
	class Row_Pipe* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Pipe : public TableRow, public SerializeClass
	{
		friend struct Table_Pipe::Key;
		friend class Table_Pipe;
	private:
		Table_Pipe *table;
		
		long int m_PK_Pipe;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Pipe_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Pipe_set(long int val);
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
	
		Row_Pipe(Table_Pipe *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Pipe *Table_Pipe_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Command_Pipe_FK_Pipe_getrows(vector <class Row_Command_Pipe*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Pipe_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Pipe_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);
void Device_Device_Pipe_FK_Pipe_getrows(vector <class Row_Device_Device_Pipe*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Pipe+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Pipe_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

