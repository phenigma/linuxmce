#ifndef __Table_Event_EventParameter_H__
#define __Table_Event_EventParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Event_EventParameter.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Event_EventParameter
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Event_EventParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Event_EventParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Event_EventParameter();

private:		
	friend class Row_Event_EventParameter;
	struct Key
	{
		friend class Row_Event_EventParameter;
		long int pk_FK_Event;
long int pk_FK_EventParameter;

		
		Key(long int in_FK_Event, long int in_FK_EventParameter);
	
		Key(class Row_Event_EventParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Event_EventParameter::Key &key1, const Table_Event_EventParameter::Key &key2) const;
	};	

	map<Table_Event_EventParameter::Key, class Row_Event_EventParameter*, Table_Event_EventParameter::Key_Less> cachedRows;
	map<Table_Event_EventParameter::Key, class Row_Event_EventParameter*, Table_Event_EventParameter::Key_Less> deleted_cachedRows;
	vector<class Row_Event_EventParameter*> addedRows;
	vector<class Row_Event_EventParameter*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Event_EventParameter*> *rows);
	class Row_Event_EventParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Event_EventParameter* GetRow(long int in_FK_Event, long int in_FK_EventParameter);
	

private:	
	
		
	class Row_Event_EventParameter* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Event_EventParameter : public TableRow, public SerializeClass
	{
		friend struct Table_Event_EventParameter::Key;
		friend class Table_Event_EventParameter;
	private:
		Table_Event_EventParameter *table;
		
		long int m_FK_Event;
long int m_FK_EventParameter;
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
		long int FK_Event_get();
long int FK_EventParameter_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Event_set(long int val);
void FK_EventParameter_set(long int val);
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
	
		Row_Event_EventParameter(Table_Event_EventParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Event_EventParameter *Table_Event_EventParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Event* FK_Event_getrow();
class Row_EventParameter* FK_EventParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Event+ m_FK_EventParameter+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Event_asSQL();
string FK_EventParameter_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

