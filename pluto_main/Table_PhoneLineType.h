#ifndef __Table_PhoneLineType_H__
#define __Table_PhoneLineType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PhoneLineType.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_PhoneLineType
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_PhoneLineType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("PhoneLineType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_PhoneLineType();

private:		
	friend class Row_PhoneLineType;
	struct Key
	{
		friend class Row_PhoneLineType;
		long int pk_PK_PhoneLineType;

		
		Key(long int in_PK_PhoneLineType);
	
		Key(class Row_PhoneLineType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PhoneLineType::Key &key1, const Table_PhoneLineType::Key &key2) const;
	};	

	map<Table_PhoneLineType::Key, class Row_PhoneLineType*, Table_PhoneLineType::Key_Less> cachedRows;
	map<Table_PhoneLineType::Key, class Row_PhoneLineType*, Table_PhoneLineType::Key_Less> deleted_cachedRows;
	vector<class Row_PhoneLineType*> addedRows;
	vector<class Row_PhoneLineType*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_PhoneLineType*> *rows);
	class Row_PhoneLineType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PhoneLineType* GetRow(long int in_PK_PhoneLineType);
	

private:	
	
		
	class Row_PhoneLineType* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_PhoneLineType : public TableRow, public SerializeClass
	{
		friend struct Table_PhoneLineType::Key;
		friend class Table_PhoneLineType;
	private:
		Table_PhoneLineType *table;
		
		long int m_PK_PhoneLineType;
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
		long int PK_PhoneLineType_get();
string Description_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_PhoneLineType_set(long int val);
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
	
		Row_PhoneLineType(Table_PhoneLineType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PhoneLineType *Table_PhoneLineType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_PhoneLineType+ m_Description+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_PhoneLineType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

