#ifndef __Table_Type_H__
#define __Table_Type_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Type.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Type
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Type(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Type")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Type();

private:		
	friend class Row_Type;
	struct Key
	{
		friend class Row_Type;
		long int pk_PK_Type;

		
		Key(long int in_PK_Type);
	
		Key(class Row_Type *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Type::Key &key1, const Table_Type::Key &key2) const;
	};	

	map<Table_Type::Key, class Row_Type*, Table_Type::Key_Less> cachedRows;
	map<Table_Type::Key, class Row_Type*, Table_Type::Key_Less> deleted_cachedRows;
	vector<class Row_Type*> addedRows;
	vector<class Row_Type*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Type*> *rows);
	class Row_Type* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Type* GetRow(long int in_PK_Type);
	

private:	
	
		
	class Row_Type* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Type : public TableRow, public SerializeClass
	{
		friend struct Table_Type::Key;
		friend class Table_Type;
	private:
		Table_Type *table;
		
		long int m_PK_Type;
string m_Description;
string m_Define;

		bool is_null[3];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Type_get();
string Description_get();
string Define_get();

		
		void PK_Type_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Type(Table_Type *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Type *Table_Type_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void File_FK_Type_getrows(vector <class Row_File*> *rows);
void Type_AttributeType_FK_Type_getrows(vector <class Row_Type_AttributeType*> *rows);
void Type_Extension_FK_Type_getrows(vector <class Row_Type_Extension*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Type+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_Type_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

