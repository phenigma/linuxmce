#ifndef __Table_DeviceTemplate_DeviceTemplate_ControlledVia_H__
#define __Table_DeviceTemplate_DeviceTemplate_ControlledVia_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DeviceTemplate_ControlledVia.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceTemplate_DeviceTemplate_ControlledVia
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_DeviceTemplate_ControlledVia(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_DeviceTemplate_ControlledVia")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_DeviceTemplate_ControlledVia();

private:		
	friend class Row_DeviceTemplate_DeviceTemplate_ControlledVia;
	struct Key
	{
		friend class Row_DeviceTemplate_DeviceTemplate_ControlledVia;
		long int pk_PK_DeviceTemplate_DeviceTemplate_ControlledVia;

		
		Key(long int in_PK_DeviceTemplate_DeviceTemplate_ControlledVia);
	
		Key(class Row_DeviceTemplate_DeviceTemplate_ControlledVia *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key &key1, const Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key &key2) const;
	};	

	map<Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key, class Row_DeviceTemplate_DeviceTemplate_ControlledVia*, Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key_Less> cachedRows;
	map<Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key, class Row_DeviceTemplate_DeviceTemplate_ControlledVia*, Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> addedRows;
	vector<class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows);
	class Row_DeviceTemplate_DeviceTemplate_ControlledVia* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DeviceTemplate_ControlledVia* GetRow(long int in_PK_DeviceTemplate_DeviceTemplate_ControlledVia);
	

private:	
	
		
	class Row_DeviceTemplate_DeviceTemplate_ControlledVia* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_DeviceTemplate_ControlledVia : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DeviceTemplate_ControlledVia::Key;
		friend class Table_DeviceTemplate_DeviceTemplate_ControlledVia;
	private:
		Table_DeviceTemplate_DeviceTemplate_ControlledVia *table;
		
		long int m_PK_DeviceTemplate_DeviceTemplate_ControlledVia;
long int m_FK_DeviceTemplate;
long int m_FK_DeviceTemplate_ControlledVia;
string m_Description;
short int m_RerouteMessagesToParent;
short int m_AutoCreateChildren;
long int m_FK_Input_Audio;
long int m_FK_Input_Video;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[12];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DeviceTemplate_DeviceTemplate_ControlledVia_get();
long int FK_DeviceTemplate_get();
long int FK_DeviceTemplate_ControlledVia_get();
string Description_get();
short int RerouteMessagesToParent_get();
short int AutoCreateChildren_get();
long int FK_Input_Audio_get();
long int FK_Input_Video_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DeviceTemplate_DeviceTemplate_ControlledVia_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_DeviceTemplate_ControlledVia_set(long int val);
void Description_set(string val);
void RerouteMessagesToParent_set(short int val);
void AutoCreateChildren_set(short int val);
void FK_Input_Audio_set(long int val);
void FK_Input_Video_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Description_isNull();
bool FK_Input_Audio_isNull();
bool FK_Input_Video_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Description_setNull(bool val);
void FK_Input_Audio_setNull(bool val);
void FK_Input_Video_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_DeviceTemplate_ControlledVia(Table_DeviceTemplate_DeviceTemplate_ControlledVia *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DeviceTemplate_ControlledVia *Table_DeviceTemplate_DeviceTemplate_ControlledVia_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_ControlledVia_getrow();
class Row_Input* FK_Input_Audio_getrow();
class Row_Input* FK_Input_Video_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_DeviceTemplate_DeviceTemplate_ControlledVia_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DeviceTemplate_DeviceTemplate_ControlledVia+ m_FK_DeviceTemplate+ m_FK_DeviceTemplate_ControlledVia+ m_Description+ m_RerouteMessagesToParent+ m_AutoCreateChildren+ m_FK_Input_Audio+ m_FK_Input_Video+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceTemplate_DeviceTemplate_ControlledVia_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_DeviceTemplate_ControlledVia_asSQL();
string Description_asSQL();
string RerouteMessagesToParent_asSQL();
string AutoCreateChildren_asSQL();
string FK_Input_Audio_asSQL();
string FK_Input_Video_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

