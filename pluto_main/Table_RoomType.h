#ifndef __Table_RoomType_H__
#define __Table_RoomType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RoomType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_RoomType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_RoomType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("RoomType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_RoomType();

private:		
	friend class Row_RoomType;
	struct Key
	{
		friend class Row_RoomType;
		long int pk_PK_RoomType;

		
		Key(long int in_PK_RoomType);
	
		Key(class Row_RoomType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RoomType::Key &key1, const Table_RoomType::Key &key2) const;
	};	

	map<Table_RoomType::Key, class Row_RoomType*, Table_RoomType::Key_Less> cachedRows;
	map<Table_RoomType::Key, class Row_RoomType*, Table_RoomType::Key_Less> deleted_cachedRows;
	vector<class Row_RoomType*> addedRows;
	vector<class Row_RoomType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_RoomType*> *rows);
	class Row_RoomType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RoomType* GetRow(long int in_PK_RoomType);
	

private:	
	
		
	class Row_RoomType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_RoomType : public TableRow, public SerializeClass
	{
		friend struct Table_RoomType::Key;
		friend class Table_RoomType;
	private:
		Table_RoomType *table;
		
		long int m_PK_RoomType;
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
		long int PK_RoomType_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_RoomType_set(long int val);
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
	
		Row_RoomType(Table_RoomType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RoomType *Table_RoomType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Room_FK_RoomType_getrows(vector <class Row_Room*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_RoomType+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_RoomType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

