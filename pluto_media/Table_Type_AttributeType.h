#ifndef __Table_Type_AttributeType_H__
#define __Table_Type_AttributeType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Type_AttributeType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Type_AttributeType
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Type_AttributeType(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Type_AttributeType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Type_AttributeType();

private:		
	friend class Row_Type_AttributeType;
	struct Key
	{
		friend class Row_Type_AttributeType;
		long int pk_FK_Type;
long int pk_FK_AttributeType;

		
		Key(long int in_FK_Type, long int in_FK_AttributeType);
	
		Key(class Row_Type_AttributeType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Type_AttributeType::Key &key1, const Table_Type_AttributeType::Key &key2) const;
	};	

	map<Table_Type_AttributeType::Key, class Row_Type_AttributeType*, Table_Type_AttributeType::Key_Less> cachedRows;
	map<Table_Type_AttributeType::Key, class Row_Type_AttributeType*, Table_Type_AttributeType::Key_Less> deleted_cachedRows;
	vector<class Row_Type_AttributeType*> addedRows;
	vector<class Row_Type_AttributeType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Type_AttributeType*> *rows);
	class Row_Type_AttributeType* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Type_AttributeType* GetRow(long int in_FK_Type, long int in_FK_AttributeType);
	

private:	
	
		
	class Row_Type_AttributeType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Type_AttributeType : public TableRow, public SerializeClass
	{
		friend struct Table_Type_AttributeType::Key;
		friend class Table_Type_AttributeType;
	private:
		Table_Type_AttributeType *table;
		
		long int m_FK_Type;
long int m_FK_AttributeType;
short int m_Identifier;
short int m_CombineAsOne;

		bool is_null[4];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Type_get();
long int FK_AttributeType_get();
short int Identifier_get();
short int CombineAsOne_get();

		
		void FK_Type_set(long int val);
void FK_AttributeType_set(long int val);
void Identifier_set(short int val);
void CombineAsOne_set(short int val);

		
		bool CombineAsOne_isNull();

			
		void CombineAsOne_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Type_AttributeType(Table_Type_AttributeType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Type_AttributeType *Table_Type_AttributeType_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Type* FK_Type_getrow();
class Row_AttributeType* FK_AttributeType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Type+ m_FK_AttributeType+ m_Identifier+ m_CombineAsOne;
		}
	private:
		void SetDefaultValues();
		
		string FK_Type_asSQL();
string FK_AttributeType_asSQL();
string Identifier_asSQL();
string CombineAsOne_asSQL();

	};

#endif

