#ifndef __Table_DesignObjVariation_Text_H__
#define __Table_DesignObjVariation_Text_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_Text.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DesignObjVariation_Text
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjVariation_Text(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjVariation_Text")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjVariation_Text();

private:		
	friend class Row_DesignObjVariation_Text;
	struct Key
	{
		friend class Row_DesignObjVariation_Text;
		long int pk_PK_DesignObjVariation_Text;

		
		Key(long int in_PK_DesignObjVariation_Text);
	
		Key(class Row_DesignObjVariation_Text *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_Text::Key &key1, const Table_DesignObjVariation_Text::Key &key2) const;
	};	

	map<Table_DesignObjVariation_Text::Key, class Row_DesignObjVariation_Text*, Table_DesignObjVariation_Text::Key_Less> cachedRows;
	map<Table_DesignObjVariation_Text::Key, class Row_DesignObjVariation_Text*, Table_DesignObjVariation_Text::Key_Less> deleted_cachedRows;
	vector<class Row_DesignObjVariation_Text*> addedRows;
	vector<class Row_DesignObjVariation_Text*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_Text*> *rows);
	class Row_DesignObjVariation_Text* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_Text* GetRow(long int in_PK_DesignObjVariation_Text);
	

private:	
	
		
	class Row_DesignObjVariation_Text* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation_Text : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_Text::Key;
		friend class Table_DesignObjVariation_Text;
	private:
		Table_DesignObjVariation_Text *table;
		
		long int m_PK_DesignObjVariation_Text;
long int m_FK_DesignObjVariation;
long int m_FK_Text;
long int m_FK_Text_OverrideFromHeader;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_DesignObjVariation_Text_get();
long int FK_DesignObjVariation_get();
long int FK_Text_get();
long int FK_Text_OverrideFromHeader_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_DesignObjVariation_Text_set(long int val);
void FK_DesignObjVariation_set(long int val);
void FK_Text_set(long int val);
void FK_Text_OverrideFromHeader_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Text_OverrideFromHeader_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Text_OverrideFromHeader_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_Text(Table_DesignObjVariation_Text *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_Text *Table_DesignObjVariation_Text_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_Text* FK_Text_getrow();
class Row_Text* FK_Text_OverrideFromHeader_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DesignObjVariation_Text+ m_FK_DesignObjVariation+ m_FK_Text+ m_FK_Text_OverrideFromHeader+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_Text_asSQL();
string FK_DesignObjVariation_asSQL();
string FK_Text_asSQL();
string FK_Text_OverrideFromHeader_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

