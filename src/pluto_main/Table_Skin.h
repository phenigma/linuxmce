#ifndef __Table_Skin_H__
#define __Table_Skin_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Skin.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Skin : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Skin(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Skin();

private:		
	friend class Row_Skin;
	struct Key
	{
		friend class Row_Skin;
		long int pk_PK_Skin;

		
		Key(long int in_PK_Skin);
	
		Key(class Row_Skin *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Skin::Key &key1, const Table_Skin::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Skin*> *rows);
	class Row_Skin* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Skin* GetRow(long int in_PK_Skin);
	

private:	
	
		
	class Row_Skin* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Skin : public TableRow, public SerializeClass
	{
		friend struct Table_Skin::Key;
		friend class Table_Skin;
	private:
		Table_Skin *table;
		
		long int m_PK_Skin;
string m_Description;
long int m_FK_Criteria_D;
short int m_MergeStandardVariation;
string m_DataSubdirectory;
long int m_FK_Style;
long int m_FK_Skin_TextPlacement;
short int m_DrawTextBeforeChildren;
long int m_FK_StabilityStatus;
long int m_FK_DesignObj_MainMenu;
long int m_FK_DesignObj_Sleeping;
long int m_FK_DesignObj_ScreenSaver;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[17];
	
	public:
		long int PK_Skin_get();
string Description_get();
long int FK_Criteria_D_get();
short int MergeStandardVariation_get();
string DataSubdirectory_get();
long int FK_Style_get();
long int FK_Skin_TextPlacement_get();
short int DrawTextBeforeChildren_get();
long int FK_StabilityStatus_get();
long int FK_DesignObj_MainMenu_get();
long int FK_DesignObj_Sleeping_get();
long int FK_DesignObj_ScreenSaver_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Skin_set(long int val);
void Description_set(string val);
void FK_Criteria_D_set(long int val);
void MergeStandardVariation_set(short int val);
void DataSubdirectory_set(string val);
void FK_Style_set(long int val);
void FK_Skin_TextPlacement_set(long int val);
void DrawTextBeforeChildren_set(short int val);
void FK_StabilityStatus_set(long int val);
void FK_DesignObj_MainMenu_set(long int val);
void FK_DesignObj_Sleeping_set(long int val);
void FK_DesignObj_ScreenSaver_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Criteria_D_isNull();
bool DataSubdirectory_isNull();
bool FK_Style_isNull();
bool FK_Skin_TextPlacement_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Criteria_D_setNull(bool val);
void DataSubdirectory_setNull(bool val);
void FK_Style_setNull(bool val);
void FK_Skin_TextPlacement_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Skin(Table_Skin *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Skin *Table_Skin_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Criteria_D* FK_Criteria_D_getrow();
class Row_Style* FK_Style_getrow();
class Row_Skin* FK_Skin_TextPlacement_getrow();
class Row_StabilityStatus* FK_StabilityStatus_getrow();
class Row_DesignObj* FK_DesignObj_MainMenu_getrow();
class Row_DesignObj* FK_DesignObj_Sleeping_getrow();
class Row_DesignObj* FK_DesignObj_ScreenSaver_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_Skin_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);
void Skin_FK_Skin_TextPlacement_getrows(vector <class Row_Skin*> *rows);
void StyleVariation_FK_Skin_getrows(vector <class Row_StyleVariation*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Skin+ m_Description+ m_FK_Criteria_D+ m_MergeStandardVariation+ m_DataSubdirectory+ m_FK_Style+ m_FK_Skin_TextPlacement+ m_DrawTextBeforeChildren+ m_FK_StabilityStatus+ m_FK_DesignObj_MainMenu+ m_FK_DesignObj_Sleeping+ m_FK_DesignObj_ScreenSaver+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Skin_asSQL();
string Description_asSQL();
string FK_Criteria_D_asSQL();
string MergeStandardVariation_asSQL();
string DataSubdirectory_asSQL();
string FK_Style_asSQL();
string FK_Skin_TextPlacement_asSQL();
string DrawTextBeforeChildren_asSQL();
string FK_StabilityStatus_asSQL();
string FK_DesignObj_MainMenu_asSQL();
string FK_DesignObj_Sleeping_asSQL();
string FK_DesignObj_ScreenSaver_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

