#ifndef __Table_SearchToken_H__
#define __Table_SearchToken_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_SearchToken.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_SearchToken
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_SearchToken(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("SearchToken")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_SearchToken();

private:		
	friend class Row_SearchToken;
	struct Key
	{
		friend class Row_SearchToken;
		long int pk_PK_SearchToken;

		
		Key(long int in_PK_SearchToken);
	
		Key(class Row_SearchToken *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_SearchToken::Key &key1, const Table_SearchToken::Key &key2) const;
	};	

	map<Table_SearchToken::Key, class Row_SearchToken*, Table_SearchToken::Key_Less> cachedRows;
	map<Table_SearchToken::Key, class Row_SearchToken*, Table_SearchToken::Key_Less> deleted_cachedRows;
	vector<class Row_SearchToken*> addedRows;
	vector<class Row_SearchToken*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_SearchToken*> *rows);
	class Row_SearchToken* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_SearchToken* GetRow(long int in_PK_SearchToken);
	

private:	
	
		
	class Row_SearchToken* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_SearchToken : public TableRow, public SerializeClass
	{
		friend struct Table_SearchToken::Key;
		friend class Table_SearchToken;
	private:
		Table_SearchToken *table;
		
		long int m_PK_SearchToken;
string m_Token;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_SearchToken_get();
string Token_get();

		
		void PK_SearchToken_set(long int val);
void Token_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_SearchToken(Table_SearchToken *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_SearchToken *Table_SearchToken_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void SearchToken_Attribute_FK_SearchToken_getrows(vector <class Row_SearchToken_Attribute*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_SearchToken+ m_Token;
		}
	private:
		void SetDefaultValues();
		
		string PK_SearchToken_asSQL();
string Token_asSQL();

	};

#endif

