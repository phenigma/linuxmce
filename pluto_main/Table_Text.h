#ifndef __Table_Text_H__
#define __Table_Text_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Text.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Text
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Text(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Text")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Text();

private:		
	friend class Row_Text;
	struct Key
	{
		friend class Row_Text;
		long int pk_PK_Text;

		
		Key(long int in_PK_Text);
	
		Key(class Row_Text *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Text::Key &key1, const Table_Text::Key &key2) const;
	};	

	map<Table_Text::Key, class Row_Text*, Table_Text::Key_Less> cachedRows;
	map<Table_Text::Key, class Row_Text*, Table_Text::Key_Less> deleted_cachedRows;
	vector<class Row_Text*> addedRows;
	vector<class Row_Text*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Text*> *rows);
	class Row_Text* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Text* GetRow(long int in_PK_Text);
	

private:	
	
		
	class Row_Text* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Text : public TableRow, public SerializeClass
	{
		friend struct Table_Text::Key;
		friend class Table_Text;
	private:
		Table_Text *table;
		
		long int m_PK_Text;
long int m_FK_TextCategory;
string m_Description;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Text_get();
long int FK_TextCategory_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Text_set(long int val);
void FK_TextCategory_set(long int val);
void Description_set(string val);
void Define_set(string val);
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
	
		Row_Text(Table_Text *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Text *Table_Text_get() { return table; };

		// Return the rows for foreign keys 
		class Row_TextCategory* FK_TextCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_FK_Text_getrows(vector <class Row_DesignObjVariation_Text*> *rows);
void DesignObjVariation_Text_FK_Text_OverrideFromHeader_getrows(vector <class Row_DesignObjVariation_Text*> *rows);
void Text_LS_FK_Text_getrows(vector <class Row_Text_LS*> *rows);
void Text_LS_AltVersions_FK_Text_getrows(vector <class Row_Text_LS_AltVersions*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Text+ m_FK_TextCategory+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Text_asSQL();
string FK_TextCategory_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

