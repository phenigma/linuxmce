#ifndef __Table_Document_Comment_H__
#define __Table_Document_Comment_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Document_Comment.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Document_Comment
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Document_Comment(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Document_Comment")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Document_Comment();

private:		
	friend class Row_Document_Comment;
	struct Key
	{
		friend class Row_Document_Comment;
		long int pk_PK_Document_Comment;

		
		Key(long int in_PK_Document_Comment);
	
		Key(class Row_Document_Comment *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Document_Comment::Key &key1, const Table_Document_Comment::Key &key2) const;
	};	

	map<Table_Document_Comment::Key, class Row_Document_Comment*, Table_Document_Comment::Key_Less> cachedRows;
	map<Table_Document_Comment::Key, class Row_Document_Comment*, Table_Document_Comment::Key_Less> deleted_cachedRows;
	vector<class Row_Document_Comment*> addedRows;
	vector<class Row_Document_Comment*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Document_Comment*> *rows);
	class Row_Document_Comment* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Document_Comment* GetRow(long int in_PK_Document_Comment);
	

private:	
	
		
	class Row_Document_Comment* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Document_Comment : public TableRow, public SerializeClass
	{
		friend struct Table_Document_Comment::Key;
		friend class Table_Document_Comment;
	private:
		Table_Document_Comment *table;
		
		long int m_PK_Document_Comment;
long int m_FK_Document;
long int m_FK_MasterUsers;
string m_Comment;
string m_Date;

		bool is_null[5];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Document_Comment_get();
long int FK_Document_get();
long int FK_MasterUsers_get();
string Comment_get();
string Date_get();

		
		void PK_Document_Comment_set(long int val);
void FK_Document_set(long int val);
void FK_MasterUsers_set(long int val);
void Comment_set(string val);
void Date_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Document_Comment(Table_Document_Comment *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Document_Comment *Table_Document_Comment_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Document* FK_Document_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Document_Comment+ m_FK_Document+ m_FK_MasterUsers+ m_Comment+ m_Date;
		}
	private:
		void SetDefaultValues();
		
		string PK_Document_Comment_asSQL();
string FK_Document_asSQL();
string FK_MasterUsers_asSQL();
string Comment_asSQL();
string Date_asSQL();

	};

#endif

