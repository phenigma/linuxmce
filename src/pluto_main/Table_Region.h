#ifndef __Table_Region_H__
#define __Table_Region_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Region.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Region : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Region(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Region();

private:		
	friend class Row_Region;
	struct Key
	{
		friend class Row_Region;
		long int pk_PK_Region;

		
		Key(long int in_PK_Region);
	
		Key(class Row_Region *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Region::Key &key1, const Table_Region::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Region*> *rows);
	class Row_Region* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Region* GetRow(long int in_PK_Region);
	

private:	
	
		
	class Row_Region* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Region : public TableRow, public SerializeClass
	{
		friend struct Table_Region::Key;
		friend class Table_Region;
	private:
		Table_Region *table;
		
		long int m_PK_Region;
long int m_FK_Country;
string m_Region;
string m_Code;
string m_ADM1Code;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_Region_get();
long int FK_Country_get();
string Region_get();
string Code_get();
string ADM1Code_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Region_set(long int val);
void FK_Country_set(long int val);
void Region_set(string val);
void Code_set(string val);
void ADM1Code_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Region_isNull();
bool Code_isNull();
bool ADM1Code_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Region_setNull(bool val);
void Code_setNull(bool val);
void ADM1Code_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Region(Table_Region *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Region *Table_Region_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Country* FK_Country_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void City_FK_Region_getrows(vector <class Row_City*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Region+ m_FK_Country+ m_Region+ m_Code+ m_ADM1Code+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Region_asSQL();
string FK_Country_asSQL();
string Region_asSQL();
string Code_asSQL();
string ADM1Code_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

