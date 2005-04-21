#ifndef __Table_PaidLicense_H__
#define __Table_PaidLicense_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PaidLicense.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_PaidLicense : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_PaidLicense(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_PaidLicense();

private:		
	friend class Row_PaidLicense;
	struct Key
	{
		friend class Row_PaidLicense;
		long int pk_PK_PaidLicense;

		
		Key(long int in_PK_PaidLicense);
	
		Key(class Row_PaidLicense *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PaidLicense::Key &key1, const Table_PaidLicense::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_PaidLicense*> *rows);
	class Row_PaidLicense* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PaidLicense* GetRow(long int in_PK_PaidLicense);
	

private:	
	
		
	class Row_PaidLicense* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_PaidLicense : public TableRow, public SerializeClass
	{
		friend struct Table_PaidLicense::Key;
		friend class Table_PaidLicense;
	private:
		Table_PaidLicense *table;
		
		long int m_PK_PaidLicense;
long int m_FK_Package;
long int m_FK_License;
string m_ReferenceNumber;
string m_Username;
string m_Password;
string m_Key;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[12];
	
	public:
		long int PK_PaidLicense_get();
long int FK_Package_get();
long int FK_License_get();
string ReferenceNumber_get();
string Username_get();
string Password_get();
string Key_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_PaidLicense_set(long int val);
void FK_Package_set(long int val);
void FK_License_set(long int val);
void ReferenceNumber_set(string val);
void Username_set(string val);
void Password_set(string val);
void Key_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Package_isNull();
bool FK_License_isNull();
bool ReferenceNumber_isNull();
bool Username_isNull();
bool Password_isNull();
bool Key_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Package_setNull(bool val);
void FK_License_setNull(bool val);
void ReferenceNumber_setNull(bool val);
void Username_setNull(bool val);
void Password_setNull(bool val);
void Key_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PaidLicense(Table_PaidLicense *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PaidLicense *Table_PaidLicense_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();
class Row_License* FK_License_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PaidLicense+ m_FK_Package+ m_FK_License+ m_ReferenceNumber+ m_Username+ m_Password+ m_Key+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_PaidLicense_asSQL();
string FK_Package_asSQL();
string FK_License_asSQL();
string ReferenceNumber_asSQL();
string Username_asSQL();
string Password_asSQL();
string Key_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

