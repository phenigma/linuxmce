#ifndef __Table_Picture_H__
#define __Table_Picture_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Picture.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Picture : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
	Table_Picture(Database_pluto_security *pDatabase):database(pDatabase)
	{
	};
	~Table_Picture();

private:		
	friend class Row_Picture;
	struct Key
	{
		friend class Row_Picture;
		long int pk_PK_Picture;

		
		Key(long int in_PK_Picture);
	
		Key(class Row_Picture *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Picture::Key &key1, const Table_Picture::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Picture*> *rows);
	class Row_Picture* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_Picture* GetRow(long int in_PK_Picture);
	

private:	
	
		
	class Row_Picture* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Picture : public TableRow, public SerializeClass
	{
		friend struct Table_Picture::Key;
		friend class Table_Picture;
	private:
		Table_Picture *table;
		
		long int m_PK_Picture;
long int m_FK_Alert;
long int m_EK_Device;
string m_Filename;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		long int PK_Picture_get();
long int FK_Alert_get();
long int EK_Device_get();
string Filename_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Picture_set(long int val);
void FK_Alert_set(long int val);
void EK_Device_set(long int val);
void Filename_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Filename_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Filename_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Picture(Table_Picture *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Picture *Table_Picture_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Alert* FK_Alert_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Picture+ m_FK_Alert+ m_EK_Device+ m_Filename+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Picture_asSQL();
string FK_Alert_asSQL();
string EK_Device_asSQL();
string Filename_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

