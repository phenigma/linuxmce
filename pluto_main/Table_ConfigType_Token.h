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

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_ConfigType_Token : public TableBase 
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

	map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less> cachedRows;
	map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less> deleted_cachedRows;

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_ConfigType_Token*> *rows);
	class Row_ConfigType_Token* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ConfigType_Token* GetRow(long int in_FK_ConfigType_Setting, long int in_FK_ConfigType_File, string in_FindStr);
	

private:	
	
		
	class Row_ConfigType_Token* FetchRow(Table_ConfigType_Token::Key &key);
		
			
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
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int FK_ConfigType_Setting_get();
long int FK_ConfigType_File_get();
string FindStr_get();
string ReplaceStr_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_ConfigType_Setting_set(long int val);
void FK_ConfigType_File_set(long int val);
void FindStr_set(string val);
void ReplaceStr_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
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
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_ConfigType_Setting+ m_FK_ConfigType_File+ m_FindStr+ m_ReplaceStr+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_ConfigType_Setting_asSQL();
string FK_ConfigType_File_asSQL();
string FindStr_asSQL();
string ReplaceStr_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

