#ifndef __Table_Manufacturer_H__
#define __Table_Manufacturer_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Manufacturer.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Manufacturer
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Manufacturer(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Manufacturer")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Manufacturer();

private:		
	friend class Row_Manufacturer;
	struct Key
	{
		friend class Row_Manufacturer;
		long int pk_PK_Manufacturer;

		
		Key(long int in_PK_Manufacturer);
	
		Key(class Row_Manufacturer *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Manufacturer::Key &key1, const Table_Manufacturer::Key &key2) const;
	};	

	map<Table_Manufacturer::Key, class Row_Manufacturer*, Table_Manufacturer::Key_Less> cachedRows;
	map<Table_Manufacturer::Key, class Row_Manufacturer*, Table_Manufacturer::Key_Less> deleted_cachedRows;
	vector<class Row_Manufacturer*> addedRows;
	vector<class Row_Manufacturer*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Manufacturer*> *rows);
	class Row_Manufacturer* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Manufacturer* GetRow(long int in_PK_Manufacturer);
	

private:	
	
		
	class Row_Manufacturer* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Manufacturer : public TableRow, public SerializeClass
	{
		friend struct Table_Manufacturer::Key;
		friend class Table_Manufacturer;
	private:
		Table_Manufacturer *table;
		
		long int m_PK_Manufacturer;
string m_Description;
long int m_IRFrequency;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[7];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Manufacturer_get();
string Description_get();
long int IRFrequency_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Manufacturer_set(long int val);
void Description_set(string val);
void IRFrequency_set(long int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool IRFrequency_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void IRFrequency_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Manufacturer(Table_Manufacturer *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Manufacturer *Table_Manufacturer_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_Manufacturer_getrows(vector <class Row_DeviceTemplate*> *rows);
void InfraredGroup_FK_Manufacturer_getrows(vector <class Row_InfraredGroup*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Manufacturer+ m_Description+ m_IRFrequency+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Manufacturer_asSQL();
string Description_asSQL();
string IRFrequency_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

