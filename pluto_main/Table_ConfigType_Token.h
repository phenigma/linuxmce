#ifndef __Table_ConfigType_Token_H__
#define __Table_ConfigType_Token_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ConfigType_Token.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_ConfigType_Token
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_ConfigType_Token(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("ConfigType_Token")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_ConfigType_Token();

private:		
	friend class Row_ConfigType_Token;
	struct Key
	{
		friend class Row_ConfigType_Token;
		long int pk_FK_ConfigType_Setting;
long int pk_FK_ConfigType_File;
string pk_FindStr;

		
		Key(long int in_FK_ConfigType_Setting, long int in_FK_ConfigType_File, string in_FindStr);
	
		Key(class Row_ConfigType_Token *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ConfigType_Token::Key &key1, const Table_ConfigType_Token::Key &key2) const;
	};	

	map<Table_ConfigType_Token::Key, class Row_ConfigType_Token*, Table_ConfigType_Token::Key_Less> cachedRows;
	map<Table_ConfigType_Token::Key, class Row_ConfigType_Token*, Table_ConfigType_Token::Key_Less> deleted_cachedRows;
	vector<class Row_ConfigType_Token*> addedRows;
	vector<class Row_ConfigType_Token*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_ConfigType_Token*> *rows);
	class Row_ConfigType_Token* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ConfigType_Token* GetRow(long int in_FK_ConfigType_Setting, long int in_FK_ConfigType_File, string in_FindStr);
	

private:	
	
		
	class Row_ConfigType_Token* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_ConfigType_Token : public TableRow, public SerializeClass
	{
		friend struct Table_ConfigType_Token::Key;
		friend class Table_ConfigType_Token;
	private:
		Table_ConfigType_Token *table;
		
		long int m_FK_ConfigType_Setting;
long int m_FK_ConfigType_File;
string m_FindStr;
string m_ReplaceStr;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_ConfigType_Setting_get();
long int FK_ConfigType_File_get();
string FindStr_get();
string ReplaceStr_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_ConfigType_Setting_set(long int val);
void FK_ConfigType_File_set(long int val);
void FindStr_set(string val);
void ReplaceStr_set(string val);
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
	
		Row_ConfigType_Token(Table_ConfigType_Token *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ConfigType_Token *Table_ConfigType_Token_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ConfigType_Setting* FK_ConfigType_Setting_getrow();
class Row_ConfigType_File* FK_ConfigType_File_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_ConfigType_Setting+ m_FK_ConfigType_File+ m_FindStr+ m_ReplaceStr+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_ConfigType_Setting_asSQL();
string FK_ConfigType_File_asSQL();
string FindStr_asSQL();
string ReplaceStr_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

