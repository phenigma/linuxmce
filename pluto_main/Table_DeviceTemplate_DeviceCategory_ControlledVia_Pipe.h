#ifndef __Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_H__
#define __Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DeviceCategory_ControlledVia_Pipe.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_DeviceCategory_ControlledVia_Pipe")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();

private:		
	friend class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe;
	struct Key
	{
		friend class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe;
		long int pk_FK_DeviceTemplate_DeviceCategory_ControlledVia;
long int pk_FK_Pipe;

		
		Key(long int in_FK_DeviceTemplate_DeviceCategory_ControlledVia, long int in_FK_Pipe);
	
		Key(class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key &key1, const Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key &key2) const;
	};	

	map<Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key, class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key_Less> cachedRows;
	map<Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key, class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*, Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> addedRows;
	vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
	class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* GetRow(long int in_FK_DeviceTemplate_DeviceCategory_ControlledVia, long int in_FK_Pipe);
	

private:	
	
		
	class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe::Key;
		friend class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe;
	private:
		Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *table;
		
		long int m_FK_DeviceTemplate_DeviceCategory_ControlledVia;
long int m_FK_Pipe;
long int m_FK_Input;
long int m_FK_Output;
short int m_ToChild;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DeviceTemplate_DeviceCategory_ControlledVia_get();
long int FK_Pipe_get();
long int FK_Input_get();
long int FK_Output_get();
short int ToChild_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceTemplate_DeviceCategory_ControlledVia_set(long int val);
void FK_Pipe_set(long int val);
void FK_Input_set(long int val);
void FK_Output_set(long int val);
void ToChild_set(short int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Input_isNull();
bool FK_Output_isNull();
bool ToChild_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Input_setNull(bool val);
void FK_Output_setNull(bool val);
void ToChild_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe *Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate_DeviceCategory_ControlledVia* FK_DeviceTemplate_DeviceCategory_ControlledVia_getrow();
class Row_Pipe* FK_Pipe_getrow();
class Row_Input* FK_Input_getrow();
class Row_Output* FK_Output_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate_DeviceCategory_ControlledVia+ m_FK_Pipe+ m_FK_Input+ m_FK_Output+ m_ToChild+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_DeviceCategory_ControlledVia_asSQL();
string FK_Pipe_asSQL();
string FK_Input_asSQL();
string FK_Output_asSQL();
string ToChild_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

