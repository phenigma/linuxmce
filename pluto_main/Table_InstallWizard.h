#ifndef __Table_InstallWizard_H__
#define __Table_InstallWizard_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InstallWizard.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_InstallWizard
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InstallWizard(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InstallWizard")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InstallWizard();

private:		
	friend class Row_InstallWizard;
	struct Key
	{
		friend class Row_InstallWizard;
		long int pk_PK_InstallWizard;

		
		Key(long int in_PK_InstallWizard);
	
		Key(class Row_InstallWizard *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InstallWizard::Key &key1, const Table_InstallWizard::Key &key2) const;
	};	

	map<Table_InstallWizard::Key, class Row_InstallWizard*, Table_InstallWizard::Key_Less> cachedRows;
	map<Table_InstallWizard::Key, class Row_InstallWizard*, Table_InstallWizard::Key_Less> deleted_cachedRows;
	vector<class Row_InstallWizard*> addedRows;
	vector<class Row_InstallWizard*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InstallWizard*> *rows);
	class Row_InstallWizard* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InstallWizard* GetRow(long int in_PK_InstallWizard);
	

private:	
	
		
	class Row_InstallWizard* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_InstallWizard : public TableRow, public SerializeClass
	{
		friend struct Table_InstallWizard::Key;
		friend class Table_InstallWizard;
	private:
		Table_InstallWizard *table;
		
		long int m_PK_InstallWizard;
long int m_FK_DeviceTemplate;
long int m_Step;

		bool is_null[3];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_InstallWizard_get();
long int FK_DeviceTemplate_get();
long int Step_get();

		
		void PK_InstallWizard_set(long int val);
void FK_DeviceTemplate_set(long int val);
void Step_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_InstallWizard(Table_InstallWizard *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InstallWizard *Table_InstallWizard_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_InstallWizard+ m_FK_DeviceTemplate+ m_Step;
		}
	private:
		void SetDefaultValues();
		
		string PK_InstallWizard_asSQL();
string FK_DeviceTemplate_asSQL();
string Step_asSQL();

	};

#endif

