#ifndef __Table_DeviceCommandGroup_Command_H__
#define __Table_DeviceCommandGroup_Command_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceCommandGroup_Command.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DeviceCommandGroup_Command
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceCommandGroup_Command(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceCommandGroup_Command")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceCommandGroup_Command();

private:		
	friend class Row_DeviceCommandGroup_Command;
	struct Key
	{
		friend class Row_DeviceCommandGroup_Command;
		long int pk_FK_DeviceCommandGroup;
long int pk_FK_Command;

		
		Key(long int in_FK_DeviceCommandGroup, long int in_FK_Command);
	
		Key(class Row_DeviceCommandGroup_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceCommandGroup_Command::Key &key1, const Table_DeviceCommandGroup_Command::Key &key2) const;
	};	

	map<Table_DeviceCommandGroup_Command::Key, class Row_DeviceCommandGroup_Command*, Table_DeviceCommandGroup_Command::Key_Less> cachedRows;
	map<Table_DeviceCommandGroup_Command::Key, class Row_DeviceCommandGroup_Command*, Table_DeviceCommandGroup_Command::Key_Less> deleted_cachedRows;
	vector<class Row_DeviceCommandGroup_Command*> addedRows;
	vector<class Row_DeviceCommandGroup_Command*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceCommandGroup_Command*> *rows);
	class Row_DeviceCommandGroup_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceCommandGroup_Command* GetRow(long int in_FK_DeviceCommandGroup, long int in_FK_Command);
	

private:	
	
		
	class Row_DeviceCommandGroup_Command* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DeviceCommandGroup_Command : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceCommandGroup_Command::Key;
		friend class Table_DeviceCommandGroup_Command;
	private:
		Table_DeviceCommandGroup_Command *table;
		
		long int m_FK_DeviceCommandGroup;
long int m_FK_Command;
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
		long int FK_DeviceCommandGroup_get();
long int FK_Command_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceCommandGroup_set(long int val);
void FK_Command_set(long int val);
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
	
		Row_DeviceCommandGroup_Command(Table_DeviceCommandGroup_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceCommandGroup_Command *Table_DeviceCommandGroup_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCommandGroup* FK_DeviceCommandGroup_getrow();
class Row_Command* FK_Command_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceCommandGroup+ m_FK_Command+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceCommandGroup_asSQL();
string FK_Command_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

