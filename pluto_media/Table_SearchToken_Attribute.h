#ifndef __Table_SearchToken_Attribute_H__
#define __Table_SearchToken_Attribute_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_SearchToken_Attribute.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_SearchToken_Attribute
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_SearchToken_Attribute(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("SearchToken_Attribute")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_SearchToken_Attribute();

private:		
	friend class Row_SearchToken_Attribute;
	struct Key
	{
		friend class Row_SearchToken_Attribute;
		long int pk_FK_SearchToken;
long int pk_FK_Attribute;

		
		Key(long int in_FK_SearchToken, long int in_FK_Attribute);
	
		Key(class Row_SearchToken_Attribute *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_SearchToken_Attribute::Key &key1, const Table_SearchToken_Attribute::Key &key2) const;
	};	

	map<Table_SearchToken_Attribute::Key, class Row_SearchToken_Attribute*, Table_SearchToken_Attribute::Key_Less> cachedRows;
	map<Table_SearchToken_Attribute::Key, class Row_SearchToken_Attribute*, Table_SearchToken_Attribute::Key_Less> deleted_cachedRows;
	vector<class Row_SearchToken_Attribute*> addedRows;
	vector<class Row_SearchToken_Attribute*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_SearchToken_Attribute*> *rows);
	class Row_SearchToken_Attribute* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_SearchToken_Attribute* GetRow(long int in_FK_SearchToken, long int in_FK_Attribute);
	

private:	
	
		
	class Row_SearchToken_Attribute* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_SearchToken_Attribute : public TableRow, public SerializeClass
	{
		friend struct Table_SearchToken_Attribute::Key;
		friend class Table_SearchToken_Attribute;
	private:
		Table_SearchToken_Attribute *table;
		
		long int m_FK_SearchToken;
long int m_FK_Attribute;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_SearchToken_get();
long int FK_Attribute_get();

		
		void FK_SearchToken_set(long int val);
void FK_Attribute_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_SearchToken_Attribute(Table_SearchToken_Attribute *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_SearchToken_Attribute *Table_SearchToken_Attribute_get() { return table; };

		// Return the rows for foreign keys 
		class Row_SearchToken* FK_SearchToken_getrow();
class Row_Attribute* FK_Attribute_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_SearchToken+ m_FK_Attribute;
		}
	private:
		void SetDefaultValues();
		
		string FK_SearchToken_asSQL();
string FK_Attribute_asSQL();

	};

#endif

