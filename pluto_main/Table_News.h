#ifndef __Table_News_H__
#define __Table_News_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_News.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_News
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_News(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("News")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_News();

private:		
	friend class Row_News;
	struct Key
	{
		friend class Row_News;
		long int pk_PK_News;

		
		Key(long int in_PK_News);
	
		Key(class Row_News *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_News::Key &key1, const Table_News::Key &key2) const;
	};	

	map<Table_News::Key, class Row_News*, Table_News::Key_Less> cachedRows;
	map<Table_News::Key, class Row_News*, Table_News::Key_Less> deleted_cachedRows;
	vector<class Row_News*> addedRows;
	vector<class Row_News*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_News*> *rows);
	class Row_News* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_News* GetRow(long int in_PK_News);
	

private:	
	
		
	class Row_News* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_News : public TableRow, public SerializeClass
	{
		friend struct Table_News::Key;
		friend class Table_News;
	private:
		Table_News *table;
		
		long int m_PK_News;
string m_Date;
long int m_FK_Package;
string m_Title;
string m_ShortSummary;
string m_FullText;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_News_get();
string Date_get();
long int FK_Package_get();
string Title_get();
string ShortSummary_get();
string FullText_get();

		
		void PK_News_set(long int val);
void Date_set(string val);
void FK_Package_set(long int val);
void Title_set(string val);
void ShortSummary_set(string val);
void FullText_set(string val);

		
		bool Date_isNull();
bool FK_Package_isNull();
bool Title_isNull();
bool ShortSummary_isNull();
bool FullText_isNull();

			
		void Date_setNull(bool val);
void FK_Package_setNull(bool val);
void Title_setNull(bool val);
void ShortSummary_setNull(bool val);
void FullText_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_News(Table_News *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_News *Table_News_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_News+ m_Date+ m_FK_Package+ m_Title+ m_ShortSummary+ m_FullText;
		}
	private:
		void SetDefaultValues();
		
		string PK_News_asSQL();
string Date_asSQL();
string FK_Package_asSQL();
string Title_asSQL();
string ShortSummary_asSQL();
string FullText_asSQL();

	};

#endif

