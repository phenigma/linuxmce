#ifndef __Table_Language_H__
#define __Table_Language_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Language.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Language
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Language(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Language")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Language();

private:		
	friend class Row_Language;
	struct Key
	{
		friend class Row_Language;
		long int pk_PK_Language;

		
		Key(long int in_PK_Language);
	
		Key(class Row_Language *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Language::Key &key1, const Table_Language::Key &key2) const;
	};	

	map<Table_Language::Key, class Row_Language*, Table_Language::Key_Less> cachedRows;
	map<Table_Language::Key, class Row_Language*, Table_Language::Key_Less> deleted_cachedRows;
	vector<class Row_Language*> addedRows;
	vector<class Row_Language*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Language*> *rows);
	class Row_Language* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Language* GetRow(long int in_PK_Language);
	

private:	
	
		
	class Row_Language* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Language : public TableRow, public SerializeClass
	{
		friend struct Table_Language::Key;
		friend class Table_Language;
	private:
		Table_Language *table;
		
		long int m_PK_Language;
string m_Description;
long int m_FK_Language_TextPlacement;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Language_get();
string Description_get();
long int FK_Language_TextPlacement_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Language_set(long int val);
void Description_set(string val);
void FK_Language_TextPlacement_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Language_TextPlacement_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Language_TextPlacement_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Language(Table_Language *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Language *Table_Language_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Language* FK_Language_TextPlacement_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_Language_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);
void Language_FK_Language_TextPlacement_getrows(vector <class Row_Language*> *rows);
void Orbiter_FK_Language_getrows(vector <class Row_Orbiter*> *rows);
void Text_LS_FK_Language_getrows(vector <class Row_Text_LS*> *rows);
void Text_LS_AltVersions_FK_Language_getrows(vector <class Row_Text_LS_AltVersions*> *rows);
void Users_FK_Language_getrows(vector <class Row_Users*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Language+ m_Description+ m_FK_Language_TextPlacement+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Language_asSQL();
string Description_asSQL();
string FK_Language_TextPlacement_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

