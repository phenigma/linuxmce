#ifndef __Table_Text_LS_AltVersions_H__
#define __Table_Text_LS_AltVersions_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Text_LS_AltVersions.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Text_LS_AltVersions
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Text_LS_AltVersions(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Text_LS_AltVersions")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Text_LS_AltVersions();

private:		
	friend class Row_Text_LS_AltVersions;
	struct Key
	{
		friend class Row_Text_LS_AltVersions;
		long int pk_FK_Text;
long int pk_FK_Language;
long int pk_Version;

		
		Key(long int in_FK_Text, long int in_FK_Language, long int in_Version);
	
		Key(class Row_Text_LS_AltVersions *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Text_LS_AltVersions::Key &key1, const Table_Text_LS_AltVersions::Key &key2) const;
	};	

	map<Table_Text_LS_AltVersions::Key, class Row_Text_LS_AltVersions*, Table_Text_LS_AltVersions::Key_Less> cachedRows;
	map<Table_Text_LS_AltVersions::Key, class Row_Text_LS_AltVersions*, Table_Text_LS_AltVersions::Key_Less> deleted_cachedRows;
	vector<class Row_Text_LS_AltVersions*> addedRows;
	vector<class Row_Text_LS_AltVersions*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Text_LS_AltVersions*> *rows);
	class Row_Text_LS_AltVersions* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Text_LS_AltVersions* GetRow(long int in_FK_Text, long int in_FK_Language, long int in_Version);
	

private:	
	
		
	class Row_Text_LS_AltVersions* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Text_LS_AltVersions : public TableRow, public SerializeClass
	{
		friend struct Table_Text_LS_AltVersions::Key;
		friend class Table_Text_LS_AltVersions;
	private:
		Table_Text_LS_AltVersions *table;
		
		long int m_FK_Text;
long int m_FK_Language;
long int m_Version;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Text_get();
long int FK_Language_get();
long int Version_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Text_set(long int val);
void FK_Language_set(long int val);
void Version_set(long int val);
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
	
		Row_Text_LS_AltVersions(Table_Text_LS_AltVersions *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Text_LS_AltVersions *Table_Text_LS_AltVersions_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Text* FK_Text_getrow();
class Row_Language* FK_Language_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Text+ m_FK_Language+ m_Version+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Text_asSQL();
string FK_Language_asSQL();
string Version_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

