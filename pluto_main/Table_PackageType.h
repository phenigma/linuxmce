#ifndef __Table_PackageType_H__
#define __Table_PackageType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PackageType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_PackageType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_PackageType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("PackageType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_PackageType();

private:		
	friend class Row_PackageType;
	struct Key
	{
		friend class Row_PackageType;
		long int pk_PK_PackageType;

		
		Key(long int in_PK_PackageType);
	
		Key(class Row_PackageType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PackageType::Key &key1, const Table_PackageType::Key &key2) const;
	};	

	map<Table_PackageType::Key, class Row_PackageType*, Table_PackageType::Key_Less> cachedRows;
	map<Table_PackageType::Key, class Row_PackageType*, Table_PackageType::Key_Less> deleted_cachedRows;
	vector<class Row_PackageType*> addedRows;
	vector<class Row_PackageType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_PackageType*> *rows);
	class Row_PackageType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PackageType* GetRow(long int in_PK_PackageType);
	

private:	
	
		
	class Row_PackageType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_PackageType : public TableRow, public SerializeClass
	{
		friend struct Table_PackageType::Key;
		friend class Table_PackageType;
	private:
		Table_PackageType *table;
		
		long int m_PK_PackageType;
string m_Description;
string m_Define;

		bool is_null[3];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_PackageType_get();
string Description_get();
string Define_get();

		
		void PK_PackageType_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PackageType(Table_PackageType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PackageType *Table_PackageType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Package_FK_PackageType_getrows(vector <class Row_Package*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_PackageType+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_PackageType_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

