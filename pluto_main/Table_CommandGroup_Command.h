#ifndef __Table_CommandGroup_Command_H__
#define __Table_CommandGroup_Command_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_Command.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CommandGroup_Command
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CommandGroup_Command(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CommandGroup_Command")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CommandGroup_Command();

private:		
	friend class Row_CommandGroup_Command;
	struct Key
	{
		friend class Row_CommandGroup_Command;
		long int pk_PK_CommandGroup_Command;

		
		Key(long int in_PK_CommandGroup_Command);
	
		Key(class Row_CommandGroup_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_Command::Key &key1, const Table_CommandGroup_Command::Key &key2) const;
	};	

	map<Table_CommandGroup_Command::Key, class Row_CommandGroup_Command*, Table_CommandGroup_Command::Key_Less> cachedRows;
	map<Table_CommandGroup_Command::Key, class Row_CommandGroup_Command*, Table_CommandGroup_Command::Key_Less> deleted_cachedRows;
	vector<class Row_CommandGroup_Command*> addedRows;
	vector<class Row_CommandGroup_Command*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CommandGroup_Command*> *rows);
	class Row_CommandGroup_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_Command* GetRow(long int in_PK_CommandGroup_Command);
	

private:	
	
		
	class Row_CommandGroup_Command* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CommandGroup_Command : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_Command::Key;
		friend class Table_CommandGroup_Command;
	private:
		Table_CommandGroup_Command *table;
		
		long int m_PK_CommandGroup_Command;
long int m_FK_CommandGroup;
long int m_FK_Command;
long int m_FK_Device;
long int m_FK_DeviceGroup;
short int m_TurnOff;
long int m_OrderNum;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[11];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_CommandGroup_Command_get();
long int FK_CommandGroup_get();
long int FK_Command_get();
long int FK_Device_get();
long int FK_DeviceGroup_get();
short int TurnOff_get();
long int OrderNum_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_CommandGroup_Command_set(long int val);
void FK_CommandGroup_set(long int val);
void FK_Command_set(long int val);
void FK_Device_set(long int val);
void FK_DeviceGroup_set(long int val);
void TurnOff_set(short int val);
void OrderNum_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Device_isNull();
bool FK_DeviceGroup_isNull();
bool OrderNum_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Device_setNull(bool val);
void FK_DeviceGroup_setNull(bool val);
void OrderNum_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CommandGroup_Command(Table_CommandGroup_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_Command *Table_CommandGroup_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandGroup* FK_CommandGroup_getrow();
class Row_Command* FK_Command_getrow();
class Row_Device* FK_Device_getrow();
class Row_DeviceGroup* FK_DeviceGroup_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_CommandParameter_FK_CommandGroup_Command_getrows(vector <class Row_CommandGroup_Command_CommandParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CommandGroup_Command+ m_FK_CommandGroup+ m_FK_Command+ m_FK_Device+ m_FK_DeviceGroup+ m_TurnOff+ m_OrderNum+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_CommandGroup_Command_asSQL();
string FK_CommandGroup_asSQL();
string FK_Command_asSQL();
string FK_Device_asSQL();
string FK_DeviceGroup_asSQL();
string TurnOff_asSQL();
string OrderNum_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

