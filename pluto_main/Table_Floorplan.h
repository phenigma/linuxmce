#ifndef __Table_Floorplan_H__
#define __Table_Floorplan_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Floorplan.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Floorplan
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Floorplan(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Floorplan")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Floorplan();

private:		
	friend class Row_Floorplan;
	struct Key
	{
		friend class Row_Floorplan;
		long int pk_FK_Installation;
long int pk_Page;

		
		Key(long int in_FK_Installation, long int in_Page);
	
		Key(class Row_Floorplan *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Floorplan::Key &key1, const Table_Floorplan::Key &key2) const;
	};	

	map<Table_Floorplan::Key, class Row_Floorplan*, Table_Floorplan::Key_Less> cachedRows;
	map<Table_Floorplan::Key, class Row_Floorplan*, Table_Floorplan::Key_Less> deleted_cachedRows;
	vector<class Row_Floorplan*> addedRows;
	vector<class Row_Floorplan*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Floorplan*> *rows);
	class Row_Floorplan* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Floorplan* GetRow(long int in_FK_Installation, long int in_Page);
	

private:	
	
		
	class Row_Floorplan* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Floorplan : public TableRow, public SerializeClass
	{
		friend struct Table_Floorplan::Key;
		friend class Table_Floorplan;
	private:
		Table_Floorplan *table;
		
		long int m_FK_Installation;
long int m_Page;
string m_Description;
long int m_FK_Icon;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int FK_Installation_get();
long int Page_get();
string Description_get();
long int FK_Icon_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_Installation_set(long int val);
void Page_set(long int val);
void Description_set(string val);
void FK_Icon_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Icon_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Icon_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Floorplan(Table_Floorplan *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Floorplan *Table_Floorplan_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Installation* FK_Installation_getrow();
class Row_Icon* FK_Icon_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_Installation+ m_Page+ m_Description+ m_FK_Icon+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_Installation_asSQL();
string Page_asSQL();
string Description_asSQL();
string FK_Icon_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

