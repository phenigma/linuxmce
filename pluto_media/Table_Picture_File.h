#ifndef __Table_Picture_File_H__
#define __Table_Picture_File_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Picture_File.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Picture_File
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Picture_File(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Picture_File")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Picture_File();

private:		
	friend class Row_Picture_File;
	struct Key
	{
		friend class Row_Picture_File;
		long int pk_FK_Picture;
long int pk_FK_File;

		
		Key(long int in_FK_Picture, long int in_FK_File);
	
		Key(class Row_Picture_File *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Picture_File::Key &key1, const Table_Picture_File::Key &key2) const;
	};	

	map<Table_Picture_File::Key, class Row_Picture_File*, Table_Picture_File::Key_Less> cachedRows;
	map<Table_Picture_File::Key, class Row_Picture_File*, Table_Picture_File::Key_Less> deleted_cachedRows;
	vector<class Row_Picture_File*> addedRows;
	vector<class Row_Picture_File*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Picture_File*> *rows);
	class Row_Picture_File* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Picture_File* GetRow(long int in_FK_Picture, long int in_FK_File);
	

private:	
	
		
	class Row_Picture_File* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Picture_File : public TableRow, public SerializeClass
	{
		friend struct Table_Picture_File::Key;
		friend class Table_Picture_File;
	private:
		Table_Picture_File *table;
		
		long int m_FK_Picture;
long int m_FK_File;

		bool is_null[2];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Picture_get();
long int FK_File_get();

		
		void FK_Picture_set(long int val);
void FK_File_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Picture_File(Table_Picture_File *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Picture_File *Table_Picture_File_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Picture* FK_Picture_getrow();
class Row_File* FK_File_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Picture+ m_FK_File;
		}
	private:
		void SetDefaultValues();
		
		string FK_Picture_asSQL();
string FK_File_asSQL();

	};

#endif

