#ifndef __Table_Orbiter_H__
#define __Table_Orbiter_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Orbiter
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Orbiter(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Orbiter")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Orbiter();

private:		
	friend class Row_Orbiter;
	struct Key
	{
		friend class Row_Orbiter;
		long int pk_PK_Orbiter;

		
		Key(long int in_PK_Orbiter);
	
		Key(class Row_Orbiter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter::Key &key1, const Table_Orbiter::Key &key2) const;
	};	

	map<Table_Orbiter::Key, class Row_Orbiter*, Table_Orbiter::Key_Less> cachedRows;
	map<Table_Orbiter::Key, class Row_Orbiter*, Table_Orbiter::Key_Less> deleted_cachedRows;
	vector<class Row_Orbiter*> addedRows;
	vector<class Row_Orbiter*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Orbiter*> *rows);
	class Row_Orbiter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter* GetRow(long int in_PK_Orbiter);
	

private:	
	
		
	class Row_Orbiter* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Orbiter : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter::Key;
		friend class Table_Orbiter;
	private:
		Table_Orbiter *table;
		
		long int m_PK_Orbiter;
long int m_FK_DesignObj_MainMenu;
long int m_FK_DesignObj_Sleeping;
long int m_FK_DesignObj_ScreenSaver;
long int m_FK_EntertainArea;
long int m_FK_System;
long int m_FK_Skin;
long int m_FK_Size;
long int m_FK_Version;
long int m_FK_Language;
short int m_NoEffects;
string m_Modification_LastGen;
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[16];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Orbiter_get();
long int FK_DesignObj_MainMenu_get();
long int FK_DesignObj_Sleeping_get();
long int FK_DesignObj_ScreenSaver_get();
long int FK_EntertainArea_get();
long int FK_System_get();
long int FK_Skin_get();
long int FK_Size_get();
long int FK_Version_get();
long int FK_Language_get();
short int NoEffects_get();
string Modification_LastGen_get();
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
		void PK_Orbiter_set(long int val);
void FK_DesignObj_MainMenu_set(long int val);
void FK_DesignObj_Sleeping_set(long int val);
void FK_DesignObj_ScreenSaver_set(long int val);
void FK_EntertainArea_set(long int val);
void FK_System_set(long int val);
void FK_Skin_set(long int val);
void FK_Size_set(long int val);
void FK_Version_set(long int val);
void FK_Language_set(long int val);
void NoEffects_set(short int val);
void Modification_LastGen_set(string val);
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_DesignObj_Sleeping_isNull();
bool FK_DesignObj_ScreenSaver_isNull();
bool FK_EntertainArea_isNull();
bool Modification_LastGen_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_DesignObj_Sleeping_setNull(bool val);
void FK_DesignObj_ScreenSaver_setNull(bool val);
void FK_EntertainArea_setNull(bool val);
void Modification_LastGen_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Orbiter(Table_Orbiter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter *Table_Orbiter_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObj* FK_DesignObj_MainMenu_getrow();
class Row_DesignObj* FK_DesignObj_Sleeping_getrow();
class Row_DesignObj* FK_DesignObj_ScreenSaver_getrow();
class Row_EntertainArea* FK_EntertainArea_getrow();
class Row_System* FK_System_getrow();
class Row_Skin* FK_Skin_getrow();
class Row_Size* FK_Size_getrow();
class Row_Version* FK_Version_getrow();
class Row_Language* FK_Language_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CachedScreens_FK_Orbiter_getrows(vector <class Row_CachedScreens*> *rows);
void Device_Orbiter_FK_Orbiter_getrows(vector <class Row_Device_Orbiter*> *rows);
void Orbiter_Users_PasswordReq_FK_Orbiter_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows);
void Orbiter_Variable_FK_Orbiter_getrows(vector <class Row_Orbiter_Variable*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Orbiter+ m_FK_DesignObj_MainMenu+ m_FK_DesignObj_Sleeping+ m_FK_DesignObj_ScreenSaver+ m_FK_EntertainArea+ m_FK_System+ m_FK_Skin+ m_FK_Size+ m_FK_Version+ m_FK_Language+ m_NoEffects+ m_Modification_LastGen+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
		}
	private:
		void SetDefaultValues();
		
		string PK_Orbiter_asSQL();
string FK_DesignObj_MainMenu_asSQL();
string FK_DesignObj_Sleeping_asSQL();
string FK_DesignObj_ScreenSaver_asSQL();
string FK_EntertainArea_asSQL();
string FK_System_asSQL();
string FK_Skin_asSQL();
string FK_Size_asSQL();
string FK_Version_asSQL();
string FK_Language_asSQL();
string NoEffects_asSQL();
string Modification_LastGen_asSQL();
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

