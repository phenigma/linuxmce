#ifndef __Table_Command_CommandParameter_H__
#define __Table_Command_CommandParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Command_CommandParameter.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Command_CommandParameter
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Command_CommandParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Command_CommandParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Command_CommandParameter();

private:		
	friend class Row_Command_CommandParameter;
	struct Key
	{
		friend class Row_Command_CommandParameter;
		long int pk_FK_Command;
long int pk_FK_CommandParameter;

		
		Key(long int in_FK_Command, long int in_FK_CommandParameter);
	
		Key(class Row_Command_CommandParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Command_CommandParameter::Key &key1, const Table_Command_CommandParameter::Key &key2) const;
	};	

	map<Table_Command_CommandParameter::Key, class Row_Command_CommandParameter*, Table_Command_CommandParameter::Key_Less> cachedRows;
	map<Table_Command_CommandParameter::Key, class Row_Command_CommandParameter*, Table_Command_CommandParameter::Key_Less> deleted_cachedRows;
	vector<class Row_Command_CommandParameter*> addedRows;
	vector<class Row_Command_CommandParameter*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Command_CommandParameter*> *rows);
	class Row_Command_CommandParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Command_CommandParameter* GetRow(long int in_FK_Command, long int in_FK_CommandParameter);
	

private:	
	
		
	class Row_Command_CommandParameter* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Command_CommandParameter : public TableRow, public SerializeClass
	{
		friend struct Table_Command_CommandParameter::Key;
		friend class Table_Command_CommandParameter;
	private:
		Table_Command_CommandParameter *table;
		
		long int m_FK_Command;
long int m_FK_CommandParameter;
string m_Description;
short int m_IsOut;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Command_get();
long int FK_CommandParameter_get();
string Description_get();
short int IsOut_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Command_set(long int val);
void FK_CommandParameter_set(long int val);
void Description_set(string val);
void IsOut_set(short int val);
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
	
		Row_Command_CommandParameter(Table_Command_CommandParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Command_CommandParameter *Table_Command_CommandParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Command* FK_Command_getrow();
class Row_CommandParameter* FK_CommandParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Command+ m_FK_CommandParameter+ m_Description+ m_IsOut+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Command_asSQL();
string FK_CommandParameter_asSQL();
string Description_asSQL();
string IsOut_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

