#ifndef __Table_CommandGroup_D_H__
#define __Table_CommandGroup_D_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_D.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_CommandGroup_D : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CommandGroup_D(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_CommandGroup_D();

private:		
	friend class Row_CommandGroup_D;
	struct Key
	{
		friend class Row_CommandGroup_D;
		long int pk_PK_CommandGroup_D;

		
		Key(long int in_PK_CommandGroup_D);
	
		Key(class Row_CommandGroup_D *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_D::Key &key1, const Table_CommandGroup_D::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_CommandGroup_D*> *rows);
	class Row_CommandGroup_D* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_D* GetRow(long int in_PK_CommandGroup_D);
	

private:	
	
		
	class Row_CommandGroup_D* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_CommandGroup_D : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_D::Key;
		friend class Table_CommandGroup_D;
	private:
		Table_CommandGroup_D *table;
		
		long int m_PK_CommandGroup_D;
long int m_FK_Criteria_Orbiter;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[7];
	
	public:
		long int PK_CommandGroup_D_get();
long int FK_Criteria_Orbiter_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_CommandGroup_D_set(long int val);
void FK_Criteria_Orbiter_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Criteria_Orbiter_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Criteria_Orbiter_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CommandGroup_D(Table_CommandGroup_D *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_D *Table_CommandGroup_D_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Criteria* FK_Criteria_Orbiter_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_D_Command_FK_CommandGroup_D_getrows(vector <class Row_CommandGroup_D_Command*> *rows);
void DesignObjVariation_FK_CommandGroup_D_OnActivate_getrows(vector <class Row_DesignObjVariation*> *rows);
void DesignObjVariation_FK_CommandGroup_D_OnLoad_getrows(vector <class Row_DesignObjVariation*> *rows);
void DesignObjVariation_FK_CommandGroup_D_OnUnload_getrows(vector <class Row_DesignObjVariation*> *rows);
void DesignObjVariation_FK_CommandGroup_D_OnTimeout_getrows(vector <class Row_DesignObjVariation*> *rows);
void DesignObjVariation_FK_CommandGroup_D_OnStartup_getrows(vector <class Row_DesignObjVariation*> *rows);
void DesignObjVariation_Zone_FK_CommandGroup_D_getrows(vector <class Row_DesignObjVariation_Zone*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CommandGroup_D+ m_FK_Criteria_Orbiter+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_CommandGroup_D_asSQL();
string FK_Criteria_Orbiter_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

