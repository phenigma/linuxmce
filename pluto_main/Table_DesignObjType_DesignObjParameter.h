#ifndef __Table_DesignObjType_DesignObjParameter_H__
#define __Table_DesignObjType_DesignObjParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjType_DesignObjParameter.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DesignObjType_DesignObjParameter
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjType_DesignObjParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjType_DesignObjParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjType_DesignObjParameter();

private:		
	friend class Row_DesignObjType_DesignObjParameter;
	struct Key
	{
		friend class Row_DesignObjType_DesignObjParameter;
		long int pk_FK_DesignObjType;
long int pk_FK_DesignObjParameter;

		
		Key(long int in_FK_DesignObjType, long int in_FK_DesignObjParameter);
	
		Key(class Row_DesignObjType_DesignObjParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjType_DesignObjParameter::Key &key1, const Table_DesignObjType_DesignObjParameter::Key &key2) const;
	};	

	map<Table_DesignObjType_DesignObjParameter::Key, class Row_DesignObjType_DesignObjParameter*, Table_DesignObjType_DesignObjParameter::Key_Less> cachedRows;
	map<Table_DesignObjType_DesignObjParameter::Key, class Row_DesignObjType_DesignObjParameter*, Table_DesignObjType_DesignObjParameter::Key_Less> deleted_cachedRows;
	vector<class Row_DesignObjType_DesignObjParameter*> addedRows;
	vector<class Row_DesignObjType_DesignObjParameter*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjType_DesignObjParameter*> *rows);
	class Row_DesignObjType_DesignObjParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjType_DesignObjParameter* GetRow(long int in_FK_DesignObjType, long int in_FK_DesignObjParameter);
	

private:	
	
		
	class Row_DesignObjType_DesignObjParameter* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DesignObjType_DesignObjParameter : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjType_DesignObjParameter::Key;
		friend class Table_DesignObjType_DesignObjParameter;
	private:
		Table_DesignObjType_DesignObjParameter *table;
		
		long int m_FK_DesignObjType;
long int m_FK_DesignObjParameter;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DesignObjType_get();
long int FK_DesignObjParameter_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DesignObjType_set(long int val);
void FK_DesignObjParameter_set(long int val);
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
	
		Row_DesignObjType_DesignObjParameter(Table_DesignObjType_DesignObjParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjType_DesignObjParameter *Table_DesignObjType_DesignObjParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjType* FK_DesignObjType_getrow();
class Row_DesignObjParameter* FK_DesignObjParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DesignObjType+ m_FK_DesignObjParameter+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DesignObjType_asSQL();
string FK_DesignObjParameter_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

