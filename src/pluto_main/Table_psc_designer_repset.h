#ifndef __Table_psc_designer_repset_H__
#define __Table_psc_designer_repset_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_designer_repset.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_designer_repset : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_designer_repset(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("psc_designer_repset")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_designer_repset();

private:		
	friend class Row_psc_designer_repset;
	struct Key
	{
		friend class Row_psc_designer_repset;
		long int pk_PK_psc_designer_repset;

		
		Key(long int in_PK_psc_designer_repset);
	
		Key(class Row_psc_designer_repset *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_designer_repset::Key &key1, const Table_psc_designer_repset::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_psc_designer_repset*> *rows);
	class Row_psc_designer_repset* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_designer_repset* GetRow(long int in_PK_psc_designer_repset);
	

private:	
	
		
	class Row_psc_designer_repset* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_designer_repset : public TableRow, public SerializeClass
	{
		friend struct Table_psc_designer_repset::Key;
		friend class Table_psc_designer_repset;
	private:
		Table_psc_designer_repset *table;
		
		long int m_PK_psc_designer_repset;
string m_Setting;
string m_Value;

		bool is_null[3];
	
	public:
		long int PK_psc_designer_repset_get();
string Setting_get();
string Value_get();

		
		void PK_psc_designer_repset_set(long int val);
void Setting_set(string val);
void Value_set(string val);

		
		bool Value_isNull();

			
		void Value_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_designer_repset(Table_psc_designer_repset *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_designer_repset *Table_psc_designer_repset_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_designer_repset+ m_Setting+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_designer_repset_asSQL();
string Setting_asSQL();
string Value_asSQL();

	};

#endif

