#ifndef __Table_Command_H__
#define __Table_Command_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Command.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Command
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Command(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Command")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Command();

private:		
	friend class Row_Command;
	struct Key
	{
		friend class Row_Command;
		long int pk_PK_Command;

		
		Key(long int in_PK_Command);
	
		Key(class Row_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Command::Key &key1, const Table_Command::Key &key2) const;
	};	

	map<Table_Command::Key, class Row_Command*, Table_Command::Key_Less> cachedRows;
	map<Table_Command::Key, class Row_Command*, Table_Command::Key_Less> deleted_cachedRows;
	vector<class Row_Command*> addedRows;
	vector<class Row_Command*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Command*> *rows);
	class Row_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Command* GetRow(long int in_PK_Command);
	

private:	
	
		
	class Row_Command* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Command : public TableRow, public SerializeClass
	{
		friend struct Table_Command::Key;
		friend class Table_Command;
	private:
		Table_Command *table;
		
		long int m_PK_Command;
string m_Description;
string m_Define;
long int m_FK_CommandCategory;
short int m_AVCommand;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[9];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Command_get();
string Description_get();
string Define_get();
long int FK_CommandCategory_get();
short int AVCommand_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Command_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_CommandCategory_set(long int val);
void AVCommand_set(short int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Define_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Define_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Command(Table_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Command *Table_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandCategory* FK_CommandCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_Command_getrows(vector <class Row_CommandGroup_Command*> *rows);
void CommandGroup_D_Command_FK_Command_getrows(vector <class Row_CommandGroup_D_Command*> *rows);
void Command_CommandParameter_FK_Command_getrows(vector <class Row_Command_CommandParameter*> *rows);
void Command_Pipe_FK_Command_getrows(vector <class Row_Command_Pipe*> *rows);
void DSPMode_FK_Command_getrows(vector <class Row_DSPMode*> *rows);
void DeviceCommandGroup_Command_FK_Command_getrows(vector <class Row_DeviceCommandGroup_Command*> *rows);
void Device_Command_FK_Command_getrows(vector <class Row_Device_Command*> *rows);
void InfraredCode_FK_Command_getrows(vector <class Row_InfraredCode*> *rows);
void InfraredGroup_DSPMode_FK_Command_getrows(vector <class Row_InfraredGroup_DSPMode*> *rows);
void Input_FK_Command_getrows(vector <class Row_Input*> *rows);
void Output_FK_Command_getrows(vector <class Row_Output*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Command+ m_Description+ m_Define+ m_FK_CommandCategory+ m_AVCommand+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Command_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_CommandCategory_asSQL();
string AVCommand_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

