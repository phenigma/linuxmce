#ifndef __Table_InfraredGroup_DSPMode_H__
#define __Table_InfraredGroup_DSPMode_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup_DSPMode.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_InfraredGroup_DSPMode
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InfraredGroup_DSPMode(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InfraredGroup_DSPMode")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InfraredGroup_DSPMode();

private:		
	friend class Row_InfraredGroup_DSPMode;
	struct Key
	{
		friend class Row_InfraredGroup_DSPMode;
		long int pk_FK_InfraredGroup;
long int pk_FK_DSPMode;

		
		Key(long int in_FK_InfraredGroup, long int in_FK_DSPMode);
	
		Key(class Row_InfraredGroup_DSPMode *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup_DSPMode::Key &key1, const Table_InfraredGroup_DSPMode::Key &key2) const;
	};	

	map<Table_InfraredGroup_DSPMode::Key, class Row_InfraredGroup_DSPMode*, Table_InfraredGroup_DSPMode::Key_Less> cachedRows;
	map<Table_InfraredGroup_DSPMode::Key, class Row_InfraredGroup_DSPMode*, Table_InfraredGroup_DSPMode::Key_Less> deleted_cachedRows;
	vector<class Row_InfraredGroup_DSPMode*> addedRows;
	vector<class Row_InfraredGroup_DSPMode*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredGroup_DSPMode*> *rows);
	class Row_InfraredGroup_DSPMode* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup_DSPMode* GetRow(long int in_FK_InfraredGroup, long int in_FK_DSPMode);
	

private:	
	
		
	class Row_InfraredGroup_DSPMode* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_InfraredGroup_DSPMode : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup_DSPMode::Key;
		friend class Table_InfraredGroup_DSPMode;
	private:
		Table_InfraredGroup_DSPMode *table;
		
		long int m_FK_InfraredGroup;
long int m_FK_DSPMode;
string m_Description;
long int m_FK_Command;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_InfraredGroup_get();
long int FK_DSPMode_get();
string Description_get();
long int FK_Command_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_InfraredGroup_set(long int val);
void FK_DSPMode_set(long int val);
void Description_set(string val);
void FK_Command_set(long int val);
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
	
		Row_InfraredGroup_DSPMode(Table_InfraredGroup_DSPMode *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup_DSPMode *Table_InfraredGroup_DSPMode_get() { return table; };

		// Return the rows for foreign keys 
		class Row_InfraredGroup* FK_InfraredGroup_getrow();
class Row_DSPMode* FK_DSPMode_getrow();
class Row_Command* FK_Command_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_InfraredGroup+ m_FK_DSPMode+ m_Description+ m_FK_Command+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_InfraredGroup_asSQL();
string FK_DSPMode_asSQL();
string Description_asSQL();
string FK_Command_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

