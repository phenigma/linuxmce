#ifndef __Table_Package_Compat_H__
#define __Table_Package_Compat_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Compat.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Package_Compat : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Package_Compat(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Package_Compat")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Package_Compat();

private:		
	friend class Row_Package_Compat;
	struct Key
	{
		friend class Row_Package_Compat;
		long int pk_PK_Package_Compat;

		
		Key(long int in_PK_Package_Compat);
	
		Key(class Row_Package_Compat *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Compat::Key &key1, const Table_Package_Compat::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Package_Compat*> *rows);
	class Row_Package_Compat* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Compat* GetRow(long int in_PK_Package_Compat);
	

private:	
	
		
	class Row_Package_Compat* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Package_Compat : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Compat::Key;
		friend class Table_Package_Compat;
	private:
		Table_Package_Compat *table;
		
		long int m_PK_Package_Compat;
long int m_FK_Package;
long int m_FK_Distro;
long int m_FK_OperatingSystem;

		bool is_null[4];
	
	public:
		long int PK_Package_Compat_get();
long int FK_Package_get();
long int FK_Distro_get();
long int FK_OperatingSystem_get();

		
		void PK_Package_Compat_set(long int val);
void FK_Package_set(long int val);
void FK_Distro_set(long int val);
void FK_OperatingSystem_set(long int val);

		
		bool FK_Distro_isNull();
bool FK_OperatingSystem_isNull();

			
		void FK_Distro_setNull(bool val);
void FK_OperatingSystem_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_Compat(Table_Package_Compat *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Compat *Table_Package_Compat_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();
class Row_Distro* FK_Distro_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Package_Compat+ m_FK_Package+ m_FK_Distro+ m_FK_OperatingSystem;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_Compat_asSQL();
string FK_Package_asSQL();
string FK_Distro_asSQL();
string FK_OperatingSystem_asSQL();

	};

#endif

