#ifndef __Table_PageSetup_H__
#define __Table_PageSetup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PageSetup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_PageSetup : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_PageSetup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("PageSetup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_PageSetup();

private:		
	friend class Row_PageSetup;
	struct Key
	{
		friend class Row_PageSetup;
		long int pk_PK_PageSetup;

		
		Key(long int in_PK_PageSetup);
	
		Key(class Row_PageSetup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PageSetup::Key &key1, const Table_PageSetup::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_PageSetup*> *rows);
	class Row_PageSetup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PageSetup* GetRow(long int in_PK_PageSetup);
	

private:	
	
		
	class Row_PageSetup* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_PageSetup : public TableRow, public SerializeClass
	{
		friend struct Table_PageSetup::Key;
		friend class Table_PageSetup;
	private:
		Table_PageSetup *table;
		
		long int m_PK_PageSetup;
long int m_FK_PageSetup_Parent;
short int m_Website;
long int m_OrderNum;
long int m_FK_Package;
string m_Description;
string m_pageURL;
short int m_showInTopMenu;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[12];
	
	public:
		long int PK_PageSetup_get();
long int FK_PageSetup_Parent_get();
short int Website_get();
long int OrderNum_get();
long int FK_Package_get();
string Description_get();
string pageURL_get();
short int showInTopMenu_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_PageSetup_set(long int val);
void FK_PageSetup_Parent_set(long int val);
void Website_set(short int val);
void OrderNum_set(long int val);
void FK_Package_set(long int val);
void Description_set(string val);
void pageURL_set(string val);
void showInTopMenu_set(short int val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_PageSetup_Parent_isNull();
bool OrderNum_isNull();
bool FK_Package_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_PageSetup_Parent_setNull(bool val);
void OrderNum_setNull(bool val);
void FK_Package_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PageSetup(Table_PageSetup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PageSetup *Table_PageSetup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_PageSetup* FK_PageSetup_Parent_getrow();
class Row_Package* FK_Package_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_PageSetup_FK_PageSetup_getrows(vector <class Row_DeviceTemplate_PageSetup*> *rows);
void PageSetup_FK_PageSetup_Parent_getrows(vector <class Row_PageSetup*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_PageSetup+ m_FK_PageSetup_Parent+ m_Website+ m_OrderNum+ m_FK_Package+ m_Description+ m_pageURL+ m_showInTopMenu+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_PageSetup_asSQL();
string FK_PageSetup_Parent_asSQL();
string Website_asSQL();
string OrderNum_asSQL();
string FK_Package_asSQL();
string Description_asSQL();
string pageURL_asSQL();
string showInTopMenu_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

