#ifndef __Table_DeviceTemplate_PageSetup_H__
#define __Table_DeviceTemplate_PageSetup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_PageSetup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DeviceTemplate_PageSetup : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DeviceTemplate_PageSetup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DeviceTemplate_PageSetup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DeviceTemplate_PageSetup();

private:		
	friend class Row_DeviceTemplate_PageSetup;
	struct Key
	{
		friend class Row_DeviceTemplate_PageSetup;
		long int pk_FK_DeviceTemplate;
long int pk_FK_PageSetup;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_PageSetup);
	
		Key(class Row_DeviceTemplate_PageSetup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_PageSetup::Key &key1, const Table_DeviceTemplate_PageSetup::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_PageSetup*> *rows);
	class Row_DeviceTemplate_PageSetup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_PageSetup* GetRow(long int in_FK_DeviceTemplate, long int in_FK_PageSetup);
	

private:	
	
		
	class Row_DeviceTemplate_PageSetup* FetchRow(DoubleLongKey &key);
		
			
};

class DLL_EXPORT Row_DeviceTemplate_PageSetup : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_PageSetup::Key;
		friend class Table_DeviceTemplate_PageSetup;
	private:
		Table_DeviceTemplate_PageSetup *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_PageSetup;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[6];
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_PageSetup_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_PageSetup_set(long int val);
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
	
		Row_DeviceTemplate_PageSetup(Table_DeviceTemplate_PageSetup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_PageSetup *Table_DeviceTemplate_PageSetup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_PageSetup* FK_PageSetup_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_PageSetup+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_PageSetup_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

