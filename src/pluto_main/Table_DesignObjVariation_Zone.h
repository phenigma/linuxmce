#ifndef __Table_DesignObjVariation_Zone_H__
#define __Table_DesignObjVariation_Zone_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_Zone.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DesignObjVariation_Zone : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DesignObjVariation_Zone(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DesignObjVariation_Zone();

private:		
	friend class Row_DesignObjVariation_Zone;
	struct Key
	{
		friend class Row_DesignObjVariation_Zone;
		long int pk_PK_DesignObjVariation_Zone;

		
		Key(long int in_PK_DesignObjVariation_Zone);
	
		Key(class Row_DesignObjVariation_Zone *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_Zone::Key &key1, const Table_DesignObjVariation_Zone::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_Zone*> *rows);
	class Row_DesignObjVariation_Zone* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_Zone* GetRow(long int in_PK_DesignObjVariation_Zone);
	

private:	
	
		
	class Row_DesignObjVariation_Zone* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation_Zone : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_Zone::Key;
		friend class Table_DesignObjVariation_Zone;
	private:
		Table_DesignObjVariation_Zone *table;
		
		long int m_PK_DesignObjVariation_Zone;
long int m_FK_DesignObjVariation;
long int m_FK_Button;
long int m_FK_CommandGroup_D;
long int m_FK_DesignObj_Goto;
string m_Description;
long int m_X;
long int m_Y;
long int m_Width;
long int m_Height;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[15];
	
	public:
		long int PK_DesignObjVariation_Zone_get();
long int FK_DesignObjVariation_get();
long int FK_Button_get();
long int FK_CommandGroup_D_get();
long int FK_DesignObj_Goto_get();
string Description_get();
long int X_get();
long int Y_get();
long int Width_get();
long int Height_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DesignObjVariation_Zone_set(long int val);
void FK_DesignObjVariation_set(long int val);
void FK_Button_set(long int val);
void FK_CommandGroup_D_set(long int val);
void FK_DesignObj_Goto_set(long int val);
void Description_set(string val);
void X_set(long int val);
void Y_set(long int val);
void Width_set(long int val);
void Height_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DesignObjVariation_isNull();
bool FK_Button_isNull();
bool FK_DesignObj_Goto_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DesignObjVariation_setNull(bool val);
void FK_Button_setNull(bool val);
void FK_DesignObj_Goto_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_Zone(Table_DesignObjVariation_Zone *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_Zone *Table_DesignObjVariation_Zone_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_Button* FK_Button_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_getrow();
class Row_DesignObj* FK_DesignObj_Goto_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObjVariation_Zone+ m_FK_DesignObjVariation+ m_FK_Button+ m_FK_CommandGroup_D+ m_FK_DesignObj_Goto+ m_Description+ m_X+ m_Y+ m_Width+ m_Height+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_Zone_asSQL();
string FK_DesignObjVariation_asSQL();
string FK_Button_asSQL();
string FK_CommandGroup_D_asSQL();
string FK_DesignObj_Goto_asSQL();
string Description_asSQL();
string X_asSQL();
string Y_asSQL();
string Width_asSQL();
string Height_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

