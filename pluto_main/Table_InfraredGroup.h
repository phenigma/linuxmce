#ifndef __Table_InfraredGroup_H__
#define __Table_InfraredGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_InfraredGroup
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InfraredGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InfraredGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InfraredGroup();

private:		
	friend class Row_InfraredGroup;
	struct Key
	{
		friend class Row_InfraredGroup;
		long int pk_PK_InfraredGroup;

		
		Key(long int in_PK_InfraredGroup);
	
		Key(class Row_InfraredGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup::Key &key1, const Table_InfraredGroup::Key &key2) const;
	};	

	map<Table_InfraredGroup::Key, class Row_InfraredGroup*, Table_InfraredGroup::Key_Less> cachedRows;
	map<Table_InfraredGroup::Key, class Row_InfraredGroup*, Table_InfraredGroup::Key_Less> deleted_cachedRows;
	vector<class Row_InfraredGroup*> addedRows;
	vector<class Row_InfraredGroup*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredGroup*> *rows);
	class Row_InfraredGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup* GetRow(long int in_PK_InfraredGroup);
	

private:	
	
		
	class Row_InfraredGroup* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_InfraredGroup : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup::Key;
		friend class Table_InfraredGroup;
	private:
		Table_InfraredGroup *table;
		
		long int m_PK_InfraredGroup;
long int m_FK_DeviceCategory;
long int m_FK_Manufacturer;
string m_Description;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[8];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_InfraredGroup_get();
long int FK_DeviceCategory_get();
long int FK_Manufacturer_get();
string Description_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_InfraredGroup_set(long int val);
void FK_DeviceCategory_set(long int val);
void FK_Manufacturer_set(long int val);
void Description_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Description_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Description_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InfraredGroup(Table_InfraredGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup *Table_InfraredGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_InfraredGroup_FK_InfraredGroup_getrows(vector <class Row_DeviceTemplate_InfraredGroup*> *rows);
void InfraredCode_FK_InfraredGroup_getrows(vector <class Row_InfraredCode*> *rows);
void InfraredGroup_DSPMode_FK_InfraredGroup_getrows(vector <class Row_InfraredGroup_DSPMode*> *rows);
void InfraredGroup_Input_FK_InfraredGroup_getrows(vector <class Row_InfraredGroup_Input*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_InfraredGroup+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Description+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_InfraredGroup_asSQL();
string FK_DeviceCategory_asSQL();
string FK_Manufacturer_asSQL();
string Description_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

