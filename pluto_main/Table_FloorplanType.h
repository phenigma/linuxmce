#ifndef __Table_FloorplanType_H__
#define __Table_FloorplanType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_FloorplanType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_FloorplanType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_FloorplanType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("FloorplanType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_FloorplanType();

private:		
	friend class Row_FloorplanType;
	struct Key
	{
		friend class Row_FloorplanType;
		long int pk_PK_FloorplanType;

		
		Key(long int in_PK_FloorplanType);
	
		Key(class Row_FloorplanType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_FloorplanType::Key &key1, const Table_FloorplanType::Key &key2) const;
	};	

	map<Table_FloorplanType::Key, class Row_FloorplanType*, Table_FloorplanType::Key_Less> cachedRows;
	map<Table_FloorplanType::Key, class Row_FloorplanType*, Table_FloorplanType::Key_Less> deleted_cachedRows;
	vector<class Row_FloorplanType*> addedRows;
	vector<class Row_FloorplanType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_FloorplanType*> *rows);
	class Row_FloorplanType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_FloorplanType* GetRow(long int in_PK_FloorplanType);
	

private:	
	
		
	class Row_FloorplanType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_FloorplanType : public TableRow, public SerializeClass
	{
		friend struct Table_FloorplanType::Key;
		friend class Table_FloorplanType;
	private:
		Table_FloorplanType *table;
		
		long int m_PK_FloorplanType;
string m_Description;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_FloorplanType_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_FloorplanType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_FloorplanType(Table_FloorplanType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FloorplanType *Table_FloorplanType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void FloorplanObjectType_FK_FloorplanType_getrows(vector <class Row_FloorplanObjectType*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_FloorplanType+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_FloorplanType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

