#ifndef __Table_psc_media_bathdr_H__
#define __Table_psc_media_bathdr_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_media_bathdr.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_psc_media_bathdr : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_psc_media_bathdr(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("psc_media_bathdr")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_psc_media_bathdr();

private:		
	friend class Row_psc_media_bathdr;
	struct Key
	{
		friend class Row_psc_media_bathdr;
		long int pk_PK_psc_media_bathdr;

		
		Key(long int in_PK_psc_media_bathdr);
	
		Key(class Row_psc_media_bathdr *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_media_bathdr::Key &key1, const Table_psc_media_bathdr::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_psc_media_bathdr*> *rows);
	class Row_psc_media_bathdr* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_psc_media_bathdr* GetRow(long int in_PK_psc_media_bathdr);
	

private:	
	
		
	class Row_psc_media_bathdr* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_psc_media_bathdr : public TableRow, public SerializeClass
	{
		friend struct Table_psc_media_bathdr::Key;
		friend class Table_psc_media_bathdr;
	private:
		Table_psc_media_bathdr *table;
		
		long int m_PK_psc_media_bathdr;
string m_Value;

		bool is_null[2];
	
	public:
		long int PK_psc_media_bathdr_get();
string Value_get();

		
		void PK_psc_media_bathdr_set(long int val);
void Value_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_media_bathdr(Table_psc_media_bathdr *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_media_bathdr *Table_psc_media_bathdr_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_media_bathdr+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_media_bathdr_asSQL();
string Value_asSQL();

	};

#endif

