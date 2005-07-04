#ifndef __Table_Type_Extension_H__
#define __Table_Type_Extension_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Type_Extension.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Type_Extension : public TableBase 
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Type_Extension(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Type_Extension();

private:		
	friend class Row_Type_Extension;
	struct Key
	{
		friend class Row_Type_Extension;
		long int pk_FK_Type;
string pk_Extension;

		
		Key(long int in_FK_Type, string in_Extension);
	
		Key(class Row_Type_Extension *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Type_Extension::Key &key1, const Table_Type_Extension::Key &key2) const;
	};	

	map<Table_Type_Extension::Key, class TableRow*, Table_Type_Extension::Key_Less> cachedRows;
	map<Table_Type_Extension::Key, class TableRow*, Table_Type_Extension::Key_Less> deleted_cachedRows;

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Type_Extension*> *rows);
	class Row_Type_Extension* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Type_Extension* GetRow(long int in_FK_Type, string in_Extension);
	

private:	
	
		
	class Row_Type_Extension* FetchRow(Table_Type_Extension::Key &key);
		
			
};

class DECLSPECIFIER Row_Type_Extension : public TableRow, public SerializeClass
	{
		friend struct Table_Type_Extension::Key;
		friend class Table_Type_Extension;
	private:
		Table_Type_Extension *table;
		
		long int m_FK_Type;
string m_Extension;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[8];
	
	public:
		long int FK_Type_get();
string Extension_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Type_set(long int val);
void Extension_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Type_Extension(Table_Type_Extension *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Type_Extension *Table_Type_Extension_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Type* FK_Type_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Type+ m_Extension+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Type_asSQL();
string Extension_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

