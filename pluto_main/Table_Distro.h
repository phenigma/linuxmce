#ifndef __Table_Distro_H__
#define __Table_Distro_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Distro.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Distro : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Distro(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Distro")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Distro();

private:		
	friend class Row_Distro;
	struct Key
	{
		friend class Row_Distro;
		long int pk_PK_Distro;

		
		Key(long int in_PK_Distro);
	
		Key(class Row_Distro *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Distro::Key &key1, const Table_Distro::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Distro*> *rows);
	class Row_Distro* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Distro* GetRow(long int in_PK_Distro);
	

private:	
	
		
	class Row_Distro* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Distro : public TableRow, public SerializeClass
	{
		friend struct Table_Distro::Key;
		friend class Table_Distro;
	private:
		Table_Distro *table;
		
		long int m_PK_Distro;
string m_Description;
string m_Define;
long int m_FK_OperatingSystem;
string m_Installer;
string m_KickStartCD;
string m_Binaries;
string m_SourceCode;
short int m_Confirmed;

		bool is_null[9];
	
	public:
		long int PK_Distro_get();
string Description_get();
string Define_get();
long int FK_OperatingSystem_get();
string Installer_get();
string KickStartCD_get();
string Binaries_get();
string SourceCode_get();
short int Confirmed_get();

		
		void PK_Distro_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_OperatingSystem_set(long int val);
void Installer_set(string val);
void KickStartCD_set(string val);
void Binaries_set(string val);
void SourceCode_set(string val);
void Confirmed_set(short int val);

		
		bool Define_isNull();
bool Installer_isNull();

			
		void Define_setNull(bool val);
void Installer_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Distro(Table_Distro *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Distro *Table_Distro_get() { return table; };

		// Return the rows for foreign keys 
		class Row_OperatingSystem* FK_OperatingSystem_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void InstallWizard_Distro_FK_Distro_getrows(vector <class Row_InstallWizard_Distro*> *rows);
void Package_Directory_FK_Distro_getrows(vector <class Row_Package_Directory*> *rows);
void Package_Directory_File_FK_Distro_getrows(vector <class Row_Package_Directory_File*> *rows);
void Package_Source_Compat_FK_Distro_getrows(vector <class Row_Package_Source_Compat*> *rows);
void RepositorySource_FK_Distro_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Distro+ m_Description+ m_Define+ m_FK_OperatingSystem+ m_Installer+ m_KickStartCD+ m_Binaries+ m_SourceCode+ m_Confirmed;
		}
	private:
		void SetDefaultValues();
		
		string PK_Distro_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_OperatingSystem_asSQL();
string Installer_asSQL();
string KickStartCD_asSQL();
string Binaries_asSQL();
string SourceCode_asSQL();
string Confirmed_asSQL();

	};

#endif

