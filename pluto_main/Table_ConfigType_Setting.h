#ifndef __Table_ConfigType_Setting_H__
#define __Table_ConfigType_Setting_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ConfigType_Setting.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_ConfigType_Setting
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_ConfigType_Setting(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("ConfigType_Setting")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_ConfigType_Setting();

private:		
	friend class Row_ConfigType_Setting;
	struct Key
	{
		friend class Row_ConfigType_Setting;
		long int pk_PK_ConfigType_Setting;

		
		Key(long int in_PK_ConfigType_Setting);
	
		Key(class Row_ConfigType_Setting *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ConfigType_Setting::Key &key1, const Table_ConfigType_Setting::Key &key2) const;
	};	

	map<Table_ConfigType_Setting::Key, class Row_ConfigType_Setting*, Table_ConfigType_Setting::Key_Less> cachedRows;
	map<Table_ConfigType_Setting::Key, class Row_ConfigType_Setting*, Table_ConfigType_Setting::Key_Less> deleted_cachedRows;
	vector<class Row_ConfigType_Setting*> addedRows;
	vector<class Row_ConfigType_Setting*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_ConfigType_Setting*> *rows);
	class Row_ConfigType_Setting* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ConfigType_Setting* GetRow(long int in_PK_ConfigType_Setting);
	

private:	
	
		
	class Row_ConfigType_Setting* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_ConfigType_Setting : public TableRow, public SerializeClass
	{
		friend struct Table_ConfigType_Setting::Key;
		friend class Table_ConfigType_Setting;
	private:
		Table_ConfigType_Setting *table;
		
		long int m_PK_ConfigType_Setting;
long int m_FK_ConfigType;
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
		long int PK_ConfigType_Setting_get();
long int FK_ConfigType_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_ConfigType_Setting_set(long int val);
void FK_ConfigType_set(long int val);
void Description_set(string val);
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
	
		Row_ConfigType_Setting(Table_ConfigType_Setting *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ConfigType_Setting *Table_ConfigType_Setting_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ConfigType* FK_ConfigType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void ConfigType_Token_FK_ConfigType_Setting_getrows(vector <class Row_ConfigType_Token*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_ConfigType_Setting+ m_FK_ConfigType+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_ConfigType_Setting_asSQL();
string FK_ConfigType_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

