#ifndef __Table_DesignObjVariation_DesignObjParameter_H__
#define __Table_DesignObjVariation_DesignObjParameter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_DesignObjParameter.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_DesignObjVariation_DesignObjParameter
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjVariation_DesignObjParameter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjVariation_DesignObjParameter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjVariation_DesignObjParameter();

private:		
	friend class Row_DesignObjVariation_DesignObjParameter;
	struct Key
	{
		friend class Row_DesignObjVariation_DesignObjParameter;
		long int pk_FK_DesignObjVariation;
long int pk_FK_DesignObjParameter;

		
		Key(long int in_FK_DesignObjVariation, long int in_FK_DesignObjParameter);
	
		Key(class Row_DesignObjVariation_DesignObjParameter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_DesignObjParameter::Key &key1, const Table_DesignObjVariation_DesignObjParameter::Key &key2) const;
	};	

	map<Table_DesignObjVariation_DesignObjParameter::Key, class Row_DesignObjVariation_DesignObjParameter*, Table_DesignObjVariation_DesignObjParameter::Key_Less> cachedRows;
	map<Table_DesignObjVariation_DesignObjParameter::Key, class Row_DesignObjVariation_DesignObjParameter*, Table_DesignObjVariation_DesignObjParameter::Key_Less> deleted_cachedRows;
	vector<class Row_DesignObjVariation_DesignObjParameter*> addedRows;
	vector<class Row_DesignObjVariation_DesignObjParameter*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_DesignObjParameter*> *rows);
	class Row_DesignObjVariation_DesignObjParameter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_DesignObjParameter* GetRow(long int in_FK_DesignObjVariation, long int in_FK_DesignObjParameter);
	

private:	
	
		
	class Row_DesignObjVariation_DesignObjParameter* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation_DesignObjParameter : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_DesignObjParameter::Key;
		friend class Table_DesignObjVariation_DesignObjParameter;
	private:
		Table_DesignObjVariation_DesignObjParameter *table;
		
		long int m_FK_DesignObjVariation;
long int m_FK_DesignObjParameter;
string m_Value;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_DesignObjVariation_get();
long int FK_DesignObjParameter_get();
string Value_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DesignObjVariation_set(long int val);
void FK_DesignObjParameter_set(long int val);
void Value_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Value_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Value_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_DesignObjParameter(Table_DesignObjVariation_DesignObjParameter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_DesignObjParameter *Table_DesignObjVariation_DesignObjParameter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_DesignObjParameter* FK_DesignObjParameter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DesignObjVariation+ m_FK_DesignObjParameter+ m_Value+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DesignObjVariation_asSQL();
string FK_DesignObjParameter_asSQL();
string Value_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

