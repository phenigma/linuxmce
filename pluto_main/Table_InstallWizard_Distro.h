#ifndef __Table_InstallWizard_Distro_H__
#define __Table_InstallWizard_Distro_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InstallWizard_Distro.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_InstallWizard_Distro : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_InstallWizard_Distro(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("InstallWizard_Distro")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_InstallWizard_Distro();

private:		
	friend class Row_InstallWizard_Distro;
	struct Key
	{
		friend class Row_InstallWizard_Distro;
		long int pk_PK_InstallWizard_Distro;

		
		Key(long int in_PK_InstallWizard_Distro);
	
		Key(class Row_InstallWizard_Distro *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InstallWizard_Distro::Key &key1, const Table_InstallWizard_Distro::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_InstallWizard_Distro*> *rows);
	class Row_InstallWizard_Distro* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InstallWizard_Distro* GetRow(long int in_PK_InstallWizard_Distro);
	

private:	
	
		
	class Row_InstallWizard_Distro* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_InstallWizard_Distro : public TableRow, public SerializeClass
	{
		friend struct Table_InstallWizard_Distro::Key;
		friend class Table_InstallWizard_Distro;
	private:
		Table_InstallWizard_Distro *table;
		
		long int m_PK_InstallWizard_Distro;
long int m_FK_InstallWizard;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
string m_Comments;

		bool is_null[5];
	
	public:
		long int PK_InstallWizard_Distro_get();
long int FK_InstallWizard_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
string Comments_get();

		
		void PK_InstallWizard_Distro_set(long int val);
void FK_InstallWizard_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void Comments_set(string val);

		
		bool FK_InstallWizard_isNull();
bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();

			
		void FK_InstallWizard_setNull(bool val);
void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InstallWizard_Distro(Table_InstallWizard_Distro *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InstallWizard_Distro *Table_InstallWizard_Distro_get() { return table; };

		// Return the rows for foreign keys 
		class Row_InstallWizard* FK_InstallWizard_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_InstallWizard_Distro+ m_FK_InstallWizard+ m_FK_OperatingSystem+ m_FK_Distro+ m_Comments;
		}
	private:
		void SetDefaultValues();
		
		string PK_InstallWizard_Distro_asSQL();
string FK_InstallWizard_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string Comments_asSQL();

	};

#endif

