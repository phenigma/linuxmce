#ifndef __Table_ModeChange_H__
#define __Table_ModeChange_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ModeChange.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_ModeChange : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_ModeChange(Database_pluto_security *pDatabase):database(pDatabase), m_Mutex("ModeChange")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_ModeChange();

private:		
	friend class Row_ModeChange;
	struct Key
	{
		friend class Row_ModeChange;
		long int pk_PK_ModeChange;

		
		Key(long int in_PK_ModeChange);
	
		Key(class Row_ModeChange *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ModeChange::Key &key1, const Table_ModeChange::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_ModeChange*> *rows);
	class Row_ModeChange* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_ModeChange* GetRow(long int in_PK_ModeChange);
	

private:	
	
		
	class Row_ModeChange* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_ModeChange : public TableRow, public SerializeClass
	{
		friend struct Table_ModeChange::Key;
		friend class Table_ModeChange;
	private:
		Table_ModeChange *table;
		
		long int m_PK_ModeChange;
long int m_EK_HouseMode;
string m_ChangeTime;
long int m_EK_Users;

		bool is_null[4];
	
	public:
		long int PK_ModeChange_get();
long int EK_HouseMode_get();
string ChangeTime_get();
long int EK_Users_get();

		
		void PK_ModeChange_set(long int val);
void EK_HouseMode_set(long int val);
void ChangeTime_set(string val);
void EK_Users_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_ModeChange(Table_ModeChange *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ModeChange *Table_ModeChange_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_ModeChange+ m_EK_HouseMode+ m_ChangeTime+ m_EK_Users;
		}
	private:
		void SetDefaultValues();
		
		string PK_ModeChange_asSQL();
string EK_HouseMode_asSQL();
string ChangeTime_asSQL();
string EK_Users_asSQL();

	};

#endif

