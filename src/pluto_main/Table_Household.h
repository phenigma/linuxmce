#ifndef __Table_Household_H__
#define __Table_Household_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Household.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Household : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Household(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Household();

private:		
	friend class Row_Household;
	struct Key
	{
		friend class Row_Household;
		long int pk_PK_Household;

		
		Key(long int in_PK_Household);
	
		Key(class Row_Household *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Household::Key &key1, const Table_Household::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Household*> *rows);
	class Row_Household* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Household* GetRow(long int in_PK_Household);
	

private:	
	
		
	class Row_Household* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Household : public TableRow, public SerializeClass
	{
		friend struct Table_Household::Key;
		friend class Table_Household;
	private:
		Table_Household *table;
		
		long int m_PK_Household;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[6];
	
	public:
		long int PK_Household_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Household_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Household(Table_Household *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Household *Table_Household_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Household_Installation_FK_Household_getrows(vector <class Row_Household_Installation*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Household+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Household_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

