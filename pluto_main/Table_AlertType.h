#ifndef __Table_AlertType_H__
#define __Table_AlertType_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_AlertType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_AlertType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_AlertType(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("AlertType")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_AlertType();

private:		
	friend class Row_AlertType;
	struct Key
	{
		friend class Row_AlertType;
		long int pk_PK_AlertType;

		
		Key(long int in_PK_AlertType);
	
		Key(class Row_AlertType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_AlertType::Key &key1, const Table_AlertType::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_AlertType*> *rows);
	class Row_AlertType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_AlertType* GetRow(long int in_PK_AlertType);
	

private:	
	
		
	class Row_AlertType* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_AlertType : public TableRow, public SerializeClass
	{
		friend struct Table_AlertType::Key;
		friend class Table_AlertType;
	private:
		Table_AlertType *table;
		
		long int m_PK_AlertType;
string m_Description;
string m_Define;

		bool is_null[3];
	
	public:
		long int PK_AlertType_get();
string Description_get();
string Define_get();

		
		void PK_AlertType_set(long int val);
void Description_set(string val);
void Define_set(string val);

		
		bool Description_isNull();
bool Define_isNull();

			
		void Description_setNull(bool val);
void Define_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_AlertType(Table_AlertType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_AlertType *Table_AlertType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_AlertType+ m_Description+ m_Define;
		}
	private:
		void SetDefaultValues();
		
		string PK_AlertType_asSQL();
string Description_asSQL();
string Define_asSQL();

	};

#endif

