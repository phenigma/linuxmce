#ifndef __Table_MobilePhoneVersion_H__
#define __Table_MobilePhoneVersion_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_MobilePhoneVersion.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_MobilePhoneVersion : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_MobilePhoneVersion(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("MobilePhoneVersion")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_MobilePhoneVersion();

private:		
	friend class Row_MobilePhoneVersion;
	struct Key
	{
		friend class Row_MobilePhoneVersion;
		long int pk_PK_MobilePhoneVersion;

		
		Key(long int in_PK_MobilePhoneVersion);
	
		Key(class Row_MobilePhoneVersion *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_MobilePhoneVersion::Key &key1, const Table_MobilePhoneVersion::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_MobilePhoneVersion*> *rows);
	class Row_MobilePhoneVersion* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_MobilePhoneVersion* GetRow(long int in_PK_MobilePhoneVersion);
	

private:	
	
		
	class Row_MobilePhoneVersion* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_MobilePhoneVersion : public TableRow, public SerializeClass
	{
		friend struct Table_MobilePhoneVersion::Key;
		friend class Table_MobilePhoneVersion;
	private:
		Table_MobilePhoneVersion *table;
		
		long int m_PK_MobilePhoneVersion;
string m_Description;
string m_Define;
string m_Models;
long int m_Height;
long int m_Width;

		bool is_null[6];
	
	public:
		long int PK_MobilePhoneVersion_get();
string Description_get();
string Define_get();
string Models_get();
long int Height_get();
long int Width_get();

		
		void PK_MobilePhoneVersion_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Models_set(string val);
void Height_set(long int val);
void Width_set(long int val);

		
		bool Define_isNull();
bool Models_isNull();
bool Height_isNull();
bool Width_isNull();

			
		void Define_setNull(bool val);
void Models_setNull(bool val);
void Height_setNull(bool val);
void Width_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_MobilePhoneVersion(Table_MobilePhoneVersion *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_MobilePhoneVersion *Table_MobilePhoneVersion_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_MobilePhoneVersion+ m_Description+ m_Define+ m_Models+ m_Height+ m_Width;
		}
	private:
		void SetDefaultValues();
		
		string PK_MobilePhoneVersion_asSQL();
string Description_asSQL();
string Define_asSQL();
string Models_asSQL();
string Height_asSQL();
string Width_asSQL();

	};

#endif

