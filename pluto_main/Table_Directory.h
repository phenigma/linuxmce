#ifndef __Table_Directory_H__
#define __Table_Directory_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Directory.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Directory : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Directory(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Directory")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Directory();

private:		
	friend class Row_Directory;
	struct Key
	{
		friend class Row_Directory;
		long int pk_PK_Directory;

		
		Key(long int in_PK_Directory);
	
		Key(class Row_Directory *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Directory::Key &key1, const Table_Directory::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Directory*> *rows);
	class Row_Directory* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Directory* GetRow(long int in_PK_Directory);
	

private:	
	
		
	class Row_Directory* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Directory : public TableRow, public SerializeClass
	{
		friend struct Table_Directory::Key;
		friend class Table_Directory;
	private:
		Table_Directory *table;
		
		long int m_PK_Directory;
string m_Description;
string m_Define;

		bool is_null[3];
	
	public:
		long int PK_Directory_get();
string Description_get();
string Define_get();

		
		void PK_Directory_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Directory(Table_Directory *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Directory *Table_Directory_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Package_Directory_FK_Directory_getrows(vector <class Row_Package_Directory*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Directory+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_Directory_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

