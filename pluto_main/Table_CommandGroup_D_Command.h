#ifndef __Table_CommandGroup_D_Command_H__
#define __Table_CommandGroup_D_Command_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_D_Command.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CommandGroup_D_Command
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CommandGroup_D_Command(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CommandGroup_D_Command")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CommandGroup_D_Command();

private:		
	friend class Row_CommandGroup_D_Command;
	struct Key
	{
		friend class Row_CommandGroup_D_Command;
		long int pk_PK_CommandGroup_D_Command;

		
		Key(long int in_PK_CommandGroup_D_Command);
	
		Key(class Row_CommandGroup_D_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_D_Command::Key &key1, const Table_CommandGroup_D_Command::Key &key2) const;
	};	

	map<Table_CommandGroup_D_Command::Key, class Row_CommandGroup_D_Command*, Table_CommandGroup_D_Command::Key_Less> cachedRows;
	map<Table_CommandGroup_D_Command::Key, class Row_CommandGroup_D_Command*, Table_CommandGroup_D_Command::Key_Less> deleted_cachedRows;
	vector<class Row_CommandGroup_D_Command*> addedRows;
	vector<class Row_CommandGroup_D_Command*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CommandGroup_D_Command*> *rows);
	class Row_CommandGroup_D_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_D_Command* GetRow(long int in_PK_CommandGroup_D_Command);
	

private:	
	
		
	class Row_CommandGroup_D_Command* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CommandGroup_D_Command : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_D_Command::Key;
		friend class Table_CommandGroup_D_Command;
	private:
		Table_CommandGroup_D_Command *table;
		
		long int m_PK_CommandGroup_D_Command;
long int m_FK_CommandGroup_D;
long int m_FK_Command;
long int m_FK_DeviceTemplate;
long int m_FK_DeviceCategory;
short int m_BroadcastLevel;
short int m_RelativeToSender;
long int m_OrderNum;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[12];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_CommandGroup_D_Command_get();
long int FK_CommandGroup_D_get();
long int FK_Command_get();
long int FK_DeviceTemplate_get();
long int FK_DeviceCategory_get();
short int BroadcastLevel_get();
short int RelativeToSender_get();
long int OrderNum_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_CommandGroup_D_Command_set(long int val);
void FK_CommandGroup_D_set(long int val);
void FK_Command_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_DeviceCategory_set(long int val);
void BroadcastLevel_set(short int val);
void RelativeToSender_set(short int val);
void OrderNum_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_DeviceTemplate_isNull();
bool FK_DeviceCategory_isNull();
bool BroadcastLevel_isNull();
bool RelativeToSender_isNull();
bool OrderNum_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_DeviceTemplate_setNull(bool val);
void FK_DeviceCategory_setNull(bool val);
void BroadcastLevel_setNull(bool val);
void RelativeToSender_setNull(bool val);
void OrderNum_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CommandGroup_D_Command(Table_CommandGroup_D_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_D_Command *Table_CommandGroup_D_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandGroup_D* FK_CommandGroup_D_getrow();
class Row_Command* FK_Command_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceCategory* FK_DeviceCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_D_Command_CommandParameter_FK_CommandGroup_D_Command_getrows(vector <class Row_CommandGroup_D_Command_CommandParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CommandGroup_D_Command+ m_FK_CommandGroup_D+ m_FK_Command+ m_FK_DeviceTemplate+ m_FK_DeviceCategory+ m_BroadcastLevel+ m_RelativeToSender+ m_OrderNum+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_CommandGroup_D_Command_asSQL();
string FK_CommandGroup_D_asSQL();
string FK_Command_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_DeviceCategory_asSQL();
string BroadcastLevel_asSQL();
string RelativeToSender_asSQL();
string OrderNum_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

