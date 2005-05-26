#ifndef __Table_InfraredGroup_Command_Preferred_H__
#define __Table_InfraredGroup_Command_Preferred_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup_Command_Preferred.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_InfraredGroup_Command_Preferred : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_InfraredGroup_Command_Preferred(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_InfraredGroup_Command_Preferred();

private:		
	friend class Row_InfraredGroup_Command_Preferred;
	struct Key
	{
		friend class Row_InfraredGroup_Command_Preferred;
		long int pk_FK_InfraredGroup_Command;
long int pk_FK_Installation;

		
		Key(long int in_FK_InfraredGroup_Command, long int in_FK_Installation);
	
		Key(class Row_InfraredGroup_Command_Preferred *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup_Command_Preferred::Key &key1, const Table_InfraredGroup_Command_Preferred::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_InfraredGroup_Command_Preferred*> *rows);
	class Row_InfraredGroup_Command_Preferred* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup_Command_Preferred* GetRow(long int in_FK_InfraredGroup_Command, long int in_FK_Installation);
	

private:	
	
		
	class Row_InfraredGroup_Command_Preferred* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_InfraredGroup_Command_Preferred : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup_Command_Preferred::Key;
		friend class Table_InfraredGroup_Command_Preferred;
	private:
		Table_InfraredGroup_Command_Preferred *table;
		
		long int m_FK_InfraredGroup_Command;
long int m_FK_Installation;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int FK_InfraredGroup_Command_get();
long int FK_Installation_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_InfraredGroup_Command_set(long int val);
void FK_Installation_set(long int val);
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
	
		Row_InfraredGroup_Command_Preferred(Table_InfraredGroup_Command_Preferred *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup_Command_Preferred *Table_InfraredGroup_Command_Preferred_get() { return table; };

		// Return the rows for foreign keys 
		class Row_InfraredGroup_Command* FK_InfraredGroup_Command_getrow();
class Row_Installation* FK_Installation_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_InfraredGroup_Command+ m_FK_Installation+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_InfraredGroup_Command_asSQL();
string FK_Installation_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

