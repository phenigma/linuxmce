#ifndef __Table_File_H__
#define __Table_File_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_File.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_File
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_File(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("File")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_File();

private:		
	friend class Row_File;
	struct Key
	{
		friend class Row_File;
		long int pk_PK_File;

		
		Key(long int in_PK_File);
	
		Key(class Row_File *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_File::Key &key1, const Table_File::Key &key2) const;
	};	

	map<Table_File::Key, class Row_File*, Table_File::Key_Less> cachedRows;
	map<Table_File::Key, class Row_File*, Table_File::Key_Less> deleted_cachedRows;
	vector<class Row_File*> addedRows;
	vector<class Row_File*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_File*> *rows);
	class Row_File* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_File* GetRow(long int in_PK_File);
	

private:	
	
		
	class Row_File* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_File : public TableRow, public SerializeClass
	{
		friend struct Table_File::Key;
		friend class Table_File;
	private:
		Table_File *table;
		
		long int m_PK_File;
long int m_FK_Type;
string m_Path;
long int m_Missing;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_File_get();
long int FK_Type_get();
string Path_get();
long int Missing_get();

		
		void PK_File_set(long int val);
void FK_Type_set(long int val);
void Path_set(string val);
void Missing_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_File(Table_File *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_File *Table_File_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Type* FK_Type_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void File_Attribute_FK_File_getrows(vector <class Row_File_Attribute*> *rows);
void Picture_File_FK_File_getrows(vector <class Row_Picture_File*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_File+ m_FK_Type+ m_Path+ m_Missing;
		}
	private:
		void SetDefaultValues();
		
		string PK_File_asSQL();
string FK_Type_asSQL();
string Path_asSQL();
string Missing_asSQL();

	};

#endif

