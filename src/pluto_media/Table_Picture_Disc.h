#ifndef __Table_Picture_Disc_H__
#define __Table_Picture_Disc_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Picture_Disc.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Picture_Disc : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Picture_Disc(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Picture_Disc();

private:		
	friend class Row_Picture_Disc;
	struct Key
	{
		friend class Row_Picture_Disc;
		long int pk_FK_Picture;
long int pk_FK_Disc;

		
		Key(long int in_FK_Picture, long int in_FK_Disc);
	
		Key(class Row_Picture_Disc *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Picture_Disc::Key &key1, const Table_Picture_Disc::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Picture_Disc*> *rows);
	class Row_Picture_Disc* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Picture_Disc* GetRow(long int in_FK_Picture, long int in_FK_Disc);
	

private:	
	
		
	class Row_Picture_Disc* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Picture_Disc : public TableRow, public SerializeClass
	{
		friend struct Table_Picture_Disc::Key;
		friend class Table_Picture_Disc;
	private:
		Table_Picture_Disc *table;
		
		long int m_FK_Picture;
long int m_FK_Disc;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int FK_Picture_get();
long int FK_Disc_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_Picture_set(long int val);
void FK_Disc_set(long int val);
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
	
		Row_Picture_Disc(Table_Picture_Disc *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Picture_Disc *Table_Picture_Disc_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Picture* FK_Picture_getrow();
class Row_Disc* FK_Disc_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Picture+ m_FK_Disc+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_Picture_asSQL();
string FK_Disc_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

