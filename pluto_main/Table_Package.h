#ifndef __Table_Package_H__
#define __Table_Package_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Package
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Package(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Package")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Package();

private:		
	friend class Row_Package;
	struct Key
	{
		friend class Row_Package;
		long int pk_PK_Package;

		
		Key(long int in_PK_Package);
	
		Key(class Row_Package *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package::Key &key1, const Table_Package::Key &key2) const;
	};	

	map<Table_Package::Key, class Row_Package*, Table_Package::Key_Less> cachedRows;
	map<Table_Package::Key, class Row_Package*, Table_Package::Key_Less> deleted_cachedRows;
	vector<class Row_Package*> addedRows;
	vector<class Row_Package*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Package*> *rows);
	class Row_Package* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package* GetRow(long int in_PK_Package);
	

private:	
	
		
	class Row_Package* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Package : public TableRow, public SerializeClass
	{
		friend struct Table_Package::Key;
		friend class Table_Package;
	private:
		Table_Package *table;
		
		long int m_PK_Package;
string m_Description;
string m_PackageName;
string m_Source;
string m_Repository;
string m_Version;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[10];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Package_get();
string Description_get();
string PackageName_get();
string Source_get();
string Repository_get();
string Version_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Package_set(long int val);
void Description_set(string val);
void PackageName_set(string val);
void Source_set(string val);
void Repository_set(string val);
void Version_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool Source_isNull();
bool Repository_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void Source_setNull(bool val);
void Repository_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package(Table_Package *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package *Table_Package_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_Package_FK_Package_getrows(vector <class Row_DeviceTemplate_Package*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Package+ m_Description+ m_PackageName+ m_Source+ m_Repository+ m_Version+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_asSQL();
string Description_asSQL();
string PackageName_asSQL();
string Source_asSQL();
string Repository_asSQL();
string Version_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

