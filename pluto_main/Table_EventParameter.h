#ifndef __Table_EventParameter_H__
#define __Table_EventParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EventParameter.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_EventParameter
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_EventParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("EventParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_EventParameter();

private:		
	friend class Row_EventParameter;
	struct Key
	{
		friend class Row_EventParameter;
		long int pk_PK_EventParameter;

		
		Key(long int in_PK_EventParameter);
	
		Key(class Row_EventParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EventParameter::Key &key1, const Table_EventParameter::Key &key2) const;
	};	

	map<Table_EventParameter::Key, class Row_EventParameter*, Table_EventParameter::Key_Less> cachedRows;
	map<Table_EventParameter::Key, class Row_EventParameter*, Table_EventParameter::Key_Less> deleted_cachedRows;
	vector<class Row_EventParameter*> addedRows;
	vector<class Row_EventParameter*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_EventParameter*> *rows);
	class Row_EventParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventParameter* GetRow(long int in_PK_EventParameter);
	

private:	
	
		
	class Row_EventParameter* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_EventParameter : public TableRow, public SerializeClass
	{
		friend struct Table_EventParameter::Key;
		friend class Table_EventParameter;
	private:
		Table_EventParameter *table;
		
		long int m_PK_EventParameter;
string m_Description;
string m_Define;
string m_Comments;
long int m_FK_ParameterType;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_EventParameter_get();
string Description_get();
string Define_get();
string Comments_get();
long int FK_ParameterType_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_EventParameter_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Comments_set(string val);
void FK_ParameterType_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Comments_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Comments_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EventParameter(Table_EventParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EventParameter *Table_EventParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ParameterType* FK_ParameterType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Event_EventParameter_FK_EventParameter_getrows(vector <class Row_Event_EventParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_EventParameter+ m_Description+ m_Define+ m_Comments+ m_FK_ParameterType+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_EventParameter_asSQL();
string Description_asSQL();
string Define_asSQL();
string Comments_asSQL();
string FK_ParameterType_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

