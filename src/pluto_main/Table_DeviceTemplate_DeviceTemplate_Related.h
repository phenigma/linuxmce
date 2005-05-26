#ifndef __Table_DeviceTemplate_DeviceTemplate_Related_H__
#define __Table_DeviceTemplate_DeviceTemplate_Related_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DeviceTemplate_Related.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_DeviceTemplate_Related : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_DeviceTemplate_Related(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_DeviceTemplate_Related();

private:		
	friend class Row_DeviceTemplate_DeviceTemplate_Related;
	struct Key
	{
		friend class Row_DeviceTemplate_DeviceTemplate_Related;
		long int pk_FK_DeviceTemplate;
long int pk_FK_DeviceTemplate_Related;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_DeviceTemplate_Related);
	
		Key(class Row_DeviceTemplate_DeviceTemplate_Related *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DeviceTemplate_Related::Key &key1, const Table_DeviceTemplate_DeviceTemplate_Related::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceTemplate_Related*> *rows);
	class Row_DeviceTemplate_DeviceTemplate_Related* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DeviceTemplate_Related* GetRow(long int in_FK_DeviceTemplate, long int in_FK_DeviceTemplate_Related);
	

private:	
	
		
	class Row_DeviceTemplate_DeviceTemplate_Related* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_DeviceTemplate_Related : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DeviceTemplate_Related::Key;
		friend class Table_DeviceTemplate_DeviceTemplate_Related;
	private:
		Table_DeviceTemplate_DeviceTemplate_Related *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_DeviceTemplate_Related;
string m_Value;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[8];
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_DeviceTemplate_Related_get();
string Value_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_DeviceTemplate_Related_set(long int val);
void Value_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Value_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Value_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_DeviceTemplate_Related(Table_DeviceTemplate_DeviceTemplate_Related *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DeviceTemplate_Related *Table_DeviceTemplate_DeviceTemplate_Related_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_Related_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_DeviceTemplate_Related+ m_Value+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_DeviceTemplate_Related_asSQL();
string Value_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

