#ifndef __Table_SetupStep_H__
#define __Table_SetupStep_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_SetupStep.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_SetupStep
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_SetupStep(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("SetupStep")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_SetupStep();

private:		
	friend class Row_SetupStep;
	struct Key
	{
		friend class Row_SetupStep;
		long int pk_PK_SetupStep;

		
		Key(long int in_PK_SetupStep);
	
		Key(class Row_SetupStep *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_SetupStep::Key &key1, const Table_SetupStep::Key &key2) const;
	};	

	map<Table_SetupStep::Key, class Row_SetupStep*, Table_SetupStep::Key_Less> cachedRows;
	map<Table_SetupStep::Key, class Row_SetupStep*, Table_SetupStep::Key_Less> deleted_cachedRows;
	vector<class Row_SetupStep*> addedRows;
	vector<class Row_SetupStep*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_SetupStep*> *rows);
	class Row_SetupStep* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_SetupStep* GetRow(long int in_PK_SetupStep);
	

private:	
	
		
	class Row_SetupStep* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_SetupStep : public TableRow, public SerializeClass
	{
		friend struct Table_SetupStep::Key;
		friend class Table_SetupStep;
	private:
		Table_SetupStep *table;
		
		long int m_PK_SetupStep;
long int m_FK_SetupStep_Next;
long int m_FK_SetupStep_Parent;
string m_Address;
string m_Name;
string m_Description;
short int m_FirstStep;
string m_Define;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[12];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_SetupStep_get();
long int FK_SetupStep_Next_get();
long int FK_SetupStep_Parent_get();
string Address_get();
string Name_get();
string Description_get();
short int FirstStep_get();
string Define_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_SetupStep_set(long int val);
void FK_SetupStep_Next_set(long int val);
void FK_SetupStep_Parent_set(long int val);
void Address_set(string val);
void Name_set(string val);
void Description_set(string val);
void FirstStep_set(short int val);
void Define_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_SetupStep_Next_isNull();
bool FK_SetupStep_Parent_isNull();
bool Description_isNull();
bool Define_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_SetupStep_Next_setNull(bool val);
void FK_SetupStep_Parent_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_SetupStep(Table_SetupStep *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_SetupStep *Table_SetupStep_get() { return table; };

		// Return the rows for foreign keys 
		class Row_SetupStep* FK_SetupStep_Next_getrow();
class Row_SetupStep* FK_SetupStep_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void SetupStep_FK_SetupStep_Next_getrows(vector <class Row_SetupStep*> *rows);
void SetupStep_FK_SetupStep_Parent_getrows(vector <class Row_SetupStep*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_SetupStep+ m_FK_SetupStep_Next+ m_FK_SetupStep_Parent+ m_Address+ m_Name+ m_Description+ m_FirstStep+ m_Define+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_SetupStep_asSQL();
string FK_SetupStep_Next_asSQL();
string FK_SetupStep_Parent_asSQL();
string Address_asSQL();
string Name_asSQL();
string Description_asSQL();
string FirstStep_asSQL();
string Define_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

