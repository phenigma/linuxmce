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

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_InstallWizard : public TableBase , SingleLongKeyBase
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

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_InstallWizard*> *rows);
	class Row_InstallWizard* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InstallWizard* GetRow(long int in_PK_InstallWizard);
	

private:	
	
		
	class Row_InstallWizard* FetchRow(SingleLongKey &key);
		
			
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
short int m_Default;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[10];
	
	public:
		long int PK_InstallWizard_get();
long int FK_DeviceTemplate_get();
long int Step_get();
short int Default_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_InstallWizard_set(long int val);
void FK_DeviceTemplate_set(long int val);
void Step_set(long int val);
void Default_set(short int val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Default_isNull();
bool Comments_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Default_setNull(bool val);
void Comments_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InstallWizard(Table_InstallWizard *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InstallWizard *Table_InstallWizard_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void InstallWizard_Distro_FK_InstallWizard_getrows(vector <class Row_InstallWizard_Distro*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_InstallWizard+ m_FK_DeviceTemplate+ m_Step+ m_Default+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_InstallWizard_asSQL();
string FK_DeviceTemplate_asSQL();
string Step_asSQL();
string Default_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

