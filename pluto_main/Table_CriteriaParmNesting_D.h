#ifndef __Table_CriteriaParmNesting_D_H__
#define __Table_CriteriaParmNesting_D_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaParmNesting_D.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_CriteriaParmNesting_D
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CriteriaParmNesting_D(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CriteriaParmNesting_D")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CriteriaParmNesting_D();

private:		
	friend class Row_CriteriaParmNesting_D;
	struct Key
	{
		friend class Row_CriteriaParmNesting_D;
		long int pk_PK_CriteriaParmNesting_D;

		
		Key(long int in_PK_CriteriaParmNesting_D);
	
		Key(class Row_CriteriaParmNesting_D *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaParmNesting_D::Key &key1, const Table_CriteriaParmNesting_D::Key &key2) const;
	};	

	map<Table_CriteriaParmNesting_D::Key, class Row_CriteriaParmNesting_D*, Table_CriteriaParmNesting_D::Key_Less> cachedRows;
	map<Table_CriteriaParmNesting_D::Key, class Row_CriteriaParmNesting_D*, Table_CriteriaParmNesting_D::Key_Less> deleted_cachedRows;
	vector<class Row_CriteriaParmNesting_D*> addedRows;
	vector<class Row_CriteriaParmNesting_D*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CriteriaParmNesting_D*> *rows);
	class Row_CriteriaParmNesting_D* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaParmNesting_D* GetRow(long int in_PK_CriteriaParmNesting_D);
	

private:	
	
		
	class Row_CriteriaParmNesting_D* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_CriteriaParmNesting_D : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaParmNesting_D::Key;
		friend class Table_CriteriaParmNesting_D;
	private:
		Table_CriteriaParmNesting_D *table;
		
		long int m_PK_CriteriaParmNesting_D;
long int m_FK_CriteriaParmNesting_D_Parent;
short int m_IsAnd;
short int m_IsNot;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_CriteriaParmNesting_D_get();
long int FK_CriteriaParmNesting_D_Parent_get();
short int IsAnd_get();
short int IsNot_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_CriteriaParmNesting_D_set(long int val);
void FK_CriteriaParmNesting_D_Parent_set(long int val);
void IsAnd_set(short int val);
void IsNot_set(short int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_CriteriaParmNesting_D_Parent_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_CriteriaParmNesting_D_Parent_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CriteriaParmNesting_D(Table_CriteriaParmNesting_D *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaParmNesting_D *Table_CriteriaParmNesting_D_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaParmNesting_D* FK_CriteriaParmNesting_D_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CriteriaParmNesting_D_FK_CriteriaParmNesting_D_Parent_getrows(vector <class Row_CriteriaParmNesting_D*> *rows);
void CriteriaParm_D_FK_CriteriaParmNesting_D_getrows(vector <class Row_CriteriaParm_D*> *rows);
void Criteria_D_FK_CriteriaParmNesting_D_getrows(vector <class Row_Criteria_D*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CriteriaParmNesting_D+ m_FK_CriteriaParmNesting_D_Parent+ m_IsAnd+ m_IsNot+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_CriteriaParmNesting_D_asSQL();
string FK_CriteriaParmNesting_D_Parent_asSQL();
string IsAnd_asSQL();
string IsNot_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

