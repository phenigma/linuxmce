#ifndef __Table_PackageType_H__
#define __Table_PackageType_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PackageType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PackageType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_PackageType(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_PackageType();

private:		
	friend class Row_PackageType;
	struct Key
	{
		friend class Row_PackageType;
		long int pk_PK_PackageType;

		
		Key(long int in_PK_PackageType);
	
		Key(class Row_PackageType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PackageType::Key &key1, const Table_PackageType::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_PackageType*> *rows);
	class Row_PackageType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PackageType* GetRow(long int in_PK_PackageType);
	

private:	
	
		
	class Row_PackageType* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PackageType : public TableRow, public SerializeClass
	{
		friend struct Table_PackageType::Key;
		friend class Table_PackageType;
	private:
		Table_PackageType *table;
		
		long int m_PK_PackageType;
string m_Description;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int PK_PackageType_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_PackageType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Define_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Define_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PackageType(Table_PackageType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PackageType *Table_PackageType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Package_FK_PackageType_getrows(vector <class Row_Package*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PackageType+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_PackageType_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

