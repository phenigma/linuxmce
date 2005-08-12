#ifndef __Table_City_H__
#define __Table_City_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_City.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_City : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_City(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_City();

private:		
	friend class Row_City;
	struct Key
	{
		friend class Row_City;
		long int pk_PK_City;

		
		Key(long int in_PK_City);
	
		Key(class Row_City *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_City::Key &key1, const Table_City::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_City*> *rows);
	class Row_City* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_City* GetRow(long int in_PK_City);
	

private:	
	
		
	class Row_City* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_City : public TableRow, public SerializeClass
	{
		friend struct Table_City::Key;
		friend class Table_City;
	private:
		Table_City *table;
		
		long int m_PK_City;
long int m_FK_Country;
long int m_FK_Region;
string m_City;
float m_Latitude;
float m_Longitude;
string m_TimeZone;
short int m_DmaId;
string m_County;
string m_Code;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		long int PK_City_get();
long int FK_Country_get();
long int FK_Region_get();
string City_get();
float Latitude_get();
float Longitude_get();
string TimeZone_get();
short int DmaId_get();
string County_get();
string Code_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_City_set(long int val);
void FK_Country_set(long int val);
void FK_Region_set(long int val);
void City_set(string val);
void Latitude_set(float val);
void Longitude_set(float val);
void TimeZone_set(string val);
void DmaId_set(short int val);
void County_set(string val);
void Code_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Region_isNull();
bool City_isNull();
bool Latitude_isNull();
bool Longitude_isNull();
bool TimeZone_isNull();
bool DmaId_isNull();
bool County_isNull();
bool Code_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Region_setNull(bool val);
void City_setNull(bool val);
void Latitude_setNull(bool val);
void Longitude_setNull(bool val);
void TimeZone_setNull(bool val);
void DmaId_setNull(bool val);
void County_setNull(bool val);
void Code_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_City(Table_City *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_City *Table_City_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Country* FK_Country_getrow();
class Row_Region* FK_Region_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_City+ m_FK_Country+ m_FK_Region+ m_City+ m_Latitude+ m_Longitude+ m_TimeZone+ m_DmaId+ m_County+ m_Code+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_City_asSQL();
string FK_Country_asSQL();
string FK_Region_asSQL();
string City_asSQL();
string Latitude_asSQL();
string Longitude_asSQL();
string TimeZone_asSQL();
string DmaId_asSQL();
string County_asSQL();
string Code_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

