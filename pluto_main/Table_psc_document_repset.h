#ifndef __Table_psc_document_repset_H__
#define __Table_psc_document_repset_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_document_repset.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_document_repset : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_document_repset(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("psc_document_repset")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_document_repset();

private:		
	friend class Row_psc_document_repset;
	struct Key
	{
		friend class Row_psc_document_repset;
		long int pk_PK_psc_document_repset;

		
		Key(long int in_PK_psc_document_repset);
	
		Key(class Row_psc_document_repset *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_document_repset::Key &key1, const Table_psc_document_repset::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_psc_document_repset*> *rows);
	class Row_psc_document_repset* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_document_repset* GetRow(long int in_PK_psc_document_repset);
	

private:	
	
		
	class Row_psc_document_repset* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_document_repset : public TableRow, public SerializeClass
	{
		friend struct Table_psc_document_repset::Key;
		friend class Table_psc_document_repset;
	private:
		Table_psc_document_repset *table;
		
		long int m_PK_psc_document_repset;
string m_Setting;
string m_Value;

		bool is_null[3];
	
	public:
		long int PK_psc_document_repset_get();
string Setting_get();
string Value_get();

		
		void PK_psc_document_repset_set(long int val);
void Setting_set(string val);
void Value_set(string val);

		
		bool Value_isNull();

			
		void Value_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_document_repset(Table_psc_document_repset *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_document_repset *Table_psc_document_repset_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_psc_document_repset+ m_Setting+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_document_repset_asSQL();
string Setting_asSQL();
string Value_asSQL();

	};

#endif

