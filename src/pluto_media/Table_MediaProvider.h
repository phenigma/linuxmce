#ifndef __Table_MediaProvider_H__
#define __Table_MediaProvider_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_MediaProvider.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_MediaProvider : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_MediaProvider(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_MediaProvider();

private:		
	friend class Row_MediaProvider;
	struct Key
	{
		friend class Row_MediaProvider;
		long int pk_PK_MediaProvider;

		
		Key(long int in_PK_MediaProvider);
	
		Key(class Row_MediaProvider *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_MediaProvider::Key &key1, const Table_MediaProvider::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_MediaProvider*> *rows);
	class Row_MediaProvider* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_MediaProvider* GetRow(long int in_PK_MediaProvider);
	

private:	
	
		
	class Row_MediaProvider* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_MediaProvider : public TableRow, public SerializeClass
	{
		friend struct Table_MediaProvider::Key;
		friend class Table_MediaProvider;
	private:
		Table_MediaProvider *table;
		
		long int m_PK_MediaProvider;
string m_Description;
long int m_EK_MediaType;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int PK_MediaProvider_get();
string Description_get();
long int EK_MediaType_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_MediaProvider_set(long int val);
void Description_set(string val);
void EK_MediaType_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool EK_MediaType_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void EK_MediaType_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_MediaProvider(Table_MediaProvider *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_MediaProvider *Table_MediaProvider_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Bookmark_FK_MediaProvider_getrows(vector <class Row_Bookmark*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_MediaProvider+ m_Description+ m_EK_MediaType+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_MediaProvider_asSQL();
string Description_asSQL();
string EK_MediaType_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

