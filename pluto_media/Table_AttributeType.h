#ifndef __Table_AttributeType_H__
#define __Table_AttributeType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_AttributeType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_AttributeType
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_AttributeType(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("AttributeType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_AttributeType();

private:		
	friend class Row_AttributeType;
	struct Key
	{
		friend class Row_AttributeType;
		long int pk_PK_AttributeType;

		
		Key(long int in_PK_AttributeType);
	
		Key(class Row_AttributeType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_AttributeType::Key &key1, const Table_AttributeType::Key &key2) const;
	};	

	map<Table_AttributeType::Key, class Row_AttributeType*, Table_AttributeType::Key_Less> cachedRows;
	map<Table_AttributeType::Key, class Row_AttributeType*, Table_AttributeType::Key_Less> deleted_cachedRows;
	vector<class Row_AttributeType*> addedRows;
	vector<class Row_AttributeType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_AttributeType*> *rows);
	class Row_AttributeType* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_AttributeType* GetRow(long int in_PK_AttributeType);
	

private:	
	
		
	class Row_AttributeType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_AttributeType : public TableRow, public SerializeClass
	{
		friend struct Table_AttributeType::Key;
		friend class Table_AttributeType;
	private:
		Table_AttributeType *table;
		
		long int m_PK_AttributeType;
string m_Description;
string m_Define;

		bool is_null[3];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_AttributeType_get();
string Description_get();
string Define_get();

		
		void PK_AttributeType_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Define_isNull();

			
		void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_AttributeType(Table_AttributeType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_AttributeType *Table_AttributeType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Attribute_FK_AttributeType_getrows(vector <class Row_Attribute*> *rows);
void Type_AttributeType_FK_AttributeType_getrows(vector <class Row_Type_AttributeType*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_AttributeType+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_AttributeType_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

