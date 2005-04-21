#ifndef __Table_Image_H__
#define __Table_Image_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Image.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Image : public TableBase 
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Image(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Image();

private:		
	friend class Row_Image;
	struct Key
	{
		friend class Row_Image;
		string pk_PK_Image;

		
		Key(string in_PK_Image);
	
		Key(class Row_Image *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Image::Key &key1, const Table_Image::Key &key2) const;
	};	

	map<Table_Image::Key, class TableRow*, Table_Image::Key_Less> cachedRows;
	map<Table_Image::Key, class TableRow*, Table_Image::Key_Less> deleted_cachedRows;

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Image*> *rows);
	class Row_Image* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Image* GetRow(string in_PK_Image);
	

private:	
	
		
	class Row_Image* FetchRow(Table_Image::Key &key);
		
			
};

class DLL_EXPORT Row_Image : public TableRow, public SerializeClass
	{
		friend struct Table_Image::Key;
		friend class Table_Image;
	private:
		Table_Image *table;
		
		string m_PK_Image;
string m_Date;
long int m_Width;
long int m_Height;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[9];
	
	public:
		string PK_Image_get();
string Date_get();
long int Width_get();
long int Height_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Image_set(string val);
void Date_set(string val);
void Width_set(long int val);
void Height_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Date_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Date_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Image(Table_Image *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Image *Table_Image_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Image+ m_Date+ m_Width+ m_Height+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Image_asSQL();
string Date_asSQL();
string Width_asSQL();
string Height_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

