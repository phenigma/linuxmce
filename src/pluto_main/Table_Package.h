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

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Package : public TableBase , SingleLongKeyBase
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

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Package*> *rows);
	class Row_Package* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package* GetRow(long int in_PK_Package);
	

private:	
	
		
	class Row_Package* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Package : public TableRow, public SerializeClass
	{
		friend struct Table_Package::Key;
		friend class Table_Package;
	private:
		Table_Package *table;
		
		long int m_PK_Package;
string m_Description;
long int m_FK_PackageType;
long int m_FK_Package_Sourcecode;
short int m_IsSource;
short int m_NonExecutable;
string m_HomePage;
long int m_FK_License;
long int m_FK_Manufacturer;
long int m_FK_Document;
long int m_FK_Document_UsersManual;
long int m_FK_Document_ProgrammersGuide;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[18];
	
	public:
		long int PK_Package_get();
string Description_get();
long int FK_PackageType_get();
long int FK_Package_Sourcecode_get();
short int IsSource_get();
short int NonExecutable_get();
string HomePage_get();
long int FK_License_get();
long int FK_Manufacturer_get();
long int FK_Document_get();
long int FK_Document_UsersManual_get();
long int FK_Document_ProgrammersGuide_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Package_set(long int val);
void Description_set(string val);
void FK_PackageType_set(long int val);
void FK_Package_Sourcecode_set(long int val);
void IsSource_set(short int val);
void NonExecutable_set(short int val);
void HomePage_set(string val);
void FK_License_set(long int val);
void FK_Manufacturer_set(long int val);
void FK_Document_set(long int val);
void FK_Document_UsersManual_set(long int val);
void FK_Document_ProgrammersGuide_set(long int val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_PackageType_isNull();
bool FK_Package_Sourcecode_isNull();
bool NonExecutable_isNull();
bool HomePage_isNull();
bool FK_License_isNull();
bool FK_Manufacturer_isNull();
bool FK_Document_isNull();
bool FK_Document_UsersManual_isNull();
bool FK_Document_ProgrammersGuide_isNull();
bool Comments_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_PackageType_setNull(bool val);
void FK_Package_Sourcecode_setNull(bool val);
void NonExecutable_setNull(bool val);
void HomePage_setNull(bool val);
void FK_License_setNull(bool val);
void FK_Manufacturer_setNull(bool val);
void FK_Document_setNull(bool val);
void FK_Document_UsersManual_setNull(bool val);
void FK_Document_ProgrammersGuide_setNull(bool val);
void Comments_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package(Table_Package *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package *Table_Package_get() { return table; };

		// Return the rows for foreign keys 
		class Row_PackageType* FK_PackageType_getrow();
class Row_Package* FK_Package_Sourcecode_getrow();
class Row_License* FK_License_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();
class Row_Document* FK_Document_getrow();
class Row_Document* FK_Document_UsersManual_getrow();
class Row_Document* FK_Document_ProgrammersGuide_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_Package_getrows(vector <class Row_DeviceTemplate*> *rows);
void FAQ_FK_Package_getrows(vector <class Row_FAQ*> *rows);
void News_FK_Package_getrows(vector <class Row_News*> *rows);
void Package_FK_Package_Sourcecode_getrows(vector <class Row_Package*> *rows);
void Package_Compat_FK_Package_getrows(vector <class Row_Package_Compat*> *rows);
void Package_Directory_FK_Package_getrows(vector <class Row_Package_Directory*> *rows);
void Package_Package_FK_Package_getrows(vector <class Row_Package_Package*> *rows);
void Package_Package_FK_Package_DependsOn_getrows(vector <class Row_Package_Package*> *rows);
void Package_Source_FK_Package_getrows(vector <class Row_Package_Source*> *rows);
void Package_Users_FK_Package_getrows(vector <class Row_Package_Users*> *rows);
void Package_Version_FK_Package_getrows(vector <class Row_Package_Version*> *rows);
void PageSetup_FK_Package_getrows(vector <class Row_PageSetup*> *rows);
void PaidLicense_FK_Package_getrows(vector <class Row_PaidLicense*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Package+ m_Description+ m_FK_PackageType+ m_FK_Package_Sourcecode+ m_IsSource+ m_NonExecutable+ m_HomePage+ m_FK_License+ m_FK_Manufacturer+ m_FK_Document+ m_FK_Document_UsersManual+ m_FK_Document_ProgrammersGuide+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_asSQL();
string Description_asSQL();
string FK_PackageType_asSQL();
string FK_Package_Sourcecode_asSQL();
string IsSource_asSQL();
string NonExecutable_asSQL();
string HomePage_asSQL();
string FK_License_asSQL();
string FK_Manufacturer_asSQL();
string FK_Document_asSQL();
string FK_Document_UsersManual_asSQL();
string FK_Document_ProgrammersGuide_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

