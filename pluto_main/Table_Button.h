#ifndef __Table_Button_H__
#define __Table_Button_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Button.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Button
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Button(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Button")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Button();

private:		
	friend class Row_Button;
	struct Key
	{
		friend class Row_Button;
		long int pk_PK_Button;

		
		Key(long int in_PK_Button);
	
		Key(class Row_Button *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Button::Key &key1, const Table_Button::Key &key2) const;
	};	

	map<Table_Button::Key, class Row_Button*, Table_Button::Key_Less> cachedRows;
	map<Table_Button::Key, class Row_Button*, Table_Button::Key_Less> deleted_cachedRows;
	vector<class Row_Button*> addedRows;
	vector<class Row_Button*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Button*> *rows);
	class Row_Button* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Button* GetRow(long int in_PK_Button);
	

private:	
	
		
	class Row_Button* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Button : public TableRow, public SerializeClass
	{
		friend struct Table_Button::Key;
		friend class Table_Button;
	private:
		Table_Button *table;
		
		long int m_PK_Button;
string m_Description;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Button_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Button_set(long int val);
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
	
		Row_Button(Table_Button *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Button *Table_Button_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_FK_Button_getrows(vector <class Row_DesignObjVariation*> *rows);
void DesignObjVariation_Zone_FK_Button_getrows(vector <class Row_DesignObjVariation_Zone*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Button+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Button_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

