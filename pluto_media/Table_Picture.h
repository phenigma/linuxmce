#ifndef __Table_Picture_H__
#define __Table_Picture_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Picture.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Picture
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Picture(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Picture")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Picture();

private:		
	friend class Row_Picture;
	struct Key
	{
		friend class Row_Picture;
		long int pk_PK_Picture;

		
		Key(long int in_PK_Picture);
	
		Key(class Row_Picture *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Picture::Key &key1, const Table_Picture::Key &key2) const;
	};	

	map<Table_Picture::Key, class Row_Picture*, Table_Picture::Key_Less> cachedRows;
	map<Table_Picture::Key, class Row_Picture*, Table_Picture::Key_Less> deleted_cachedRows;
	vector<class Row_Picture*> addedRows;
	vector<class Row_Picture*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Picture*> *rows);
	class Row_Picture* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Picture* GetRow(long int in_PK_Picture);
	

private:	
	
		
	class Row_Picture* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Picture : public TableRow, public SerializeClass
	{
		friend struct Table_Picture::Key;
		friend class Table_Picture;
	private:
		Table_Picture *table;
		
		long int m_PK_Picture;
string m_Extension;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Picture_get();
string Extension_get();

		
		void PK_Picture_set(long int val);
void Extension_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Picture(Table_Picture *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Picture *Table_Picture_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Picture_Attribute_FK_Picture_getrows(vector <class Row_Picture_Attribute*> *rows);
void Picture_File_FK_Picture_getrows(vector <class Row_Picture_File*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Picture+ m_Extension;
		}
	private:
		void SetDefaultValues();
		
		string PK_Picture_asSQL();
string Extension_asSQL();

	};

#endif

