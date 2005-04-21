#ifndef __Table_News_H__
#define __Table_News_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_News.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_News : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_News(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_News();

private:		
	friend class Row_News;
	struct Key
	{
		friend class Row_News;
		long int pk_PK_News;

		
		Key(long int in_PK_News);
	
		Key(class Row_News *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_News::Key &key1, const Table_News::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_News*> *rows);
	class Row_News* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_News* GetRow(long int in_PK_News);
	

private:	
	
		
	class Row_News* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_News : public TableRow, public SerializeClass
	{
		friend struct Table_News::Key;
		friend class Table_News;
	private:
		Table_News *table;
		
		long int m_PK_News;
string m_Date;
long int m_FK_Package;
string m_Title;
string m_ShortSummary;
string m_FullText;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[11];
	
	public:
		long int PK_News_get();
string Date_get();
long int FK_Package_get();
string Title_get();
string ShortSummary_get();
string FullText_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_News_set(long int val);
void Date_set(string val);
void FK_Package_set(long int val);
void Title_set(string val);
void ShortSummary_set(string val);
void FullText_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Date_isNull();
bool FK_Package_isNull();
bool Title_isNull();
bool ShortSummary_isNull();
bool FullText_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Date_setNull(bool val);
void FK_Package_setNull(bool val);
void Title_setNull(bool val);
void ShortSummary_setNull(bool val);
void FullText_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_News(Table_News *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_News *Table_News_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_News+ m_Date+ m_FK_Package+ m_Title+ m_ShortSummary+ m_FullText+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_News_asSQL();
string Date_asSQL();
string FK_Package_asSQL();
string Title_asSQL();
string ShortSummary_asSQL();
string FullText_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

