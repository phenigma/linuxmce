#ifndef __Table_StyleVariation_H__
#define __Table_StyleVariation_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_StyleVariation.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_StyleVariation : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_StyleVariation(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_StyleVariation();

private:		
	friend class Row_StyleVariation;
	struct Key
	{
		friend class Row_StyleVariation;
		long int pk_PK_StyleVariation;

		
		Key(long int in_PK_StyleVariation);
	
		Key(class Row_StyleVariation *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_StyleVariation::Key &key1, const Table_StyleVariation::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_StyleVariation*> *rows);
	class Row_StyleVariation* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_StyleVariation* GetRow(long int in_PK_StyleVariation);
	

private:	
	
		
	class Row_StyleVariation* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_StyleVariation : public TableRow, public SerializeClass
	{
		friend struct Table_StyleVariation::Key;
		friend class Table_StyleVariation;
	private:
		Table_StyleVariation *table;
		
		long int m_PK_StyleVariation;
long int m_FK_Style;
long int m_FK_Skin;
long int m_FK_Criteria_D;
string m_Font;
long int m_ForeColor;
long int m_PixelHeight;
short int m_Bold;
short int m_Italic;
short int m_Underline;
long int m_ShadowX;
long int m_ShadowY;
long int m_ShadowColor;
long int m_BorderStyle;
long int m_FK_HorizAlignment;
long int m_FK_VertAlignment;
long int m_BackColor;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[22];
	
	public:
		long int PK_StyleVariation_get();
long int FK_Style_get();
long int FK_Skin_get();
long int FK_Criteria_D_get();
string Font_get();
long int ForeColor_get();
long int PixelHeight_get();
short int Bold_get();
short int Italic_get();
short int Underline_get();
long int ShadowX_get();
long int ShadowY_get();
long int ShadowColor_get();
long int BorderStyle_get();
long int FK_HorizAlignment_get();
long int FK_VertAlignment_get();
long int BackColor_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_StyleVariation_set(long int val);
void FK_Style_set(long int val);
void FK_Skin_set(long int val);
void FK_Criteria_D_set(long int val);
void Font_set(string val);
void ForeColor_set(long int val);
void PixelHeight_set(long int val);
void Bold_set(short int val);
void Italic_set(short int val);
void Underline_set(short int val);
void ShadowX_set(long int val);
void ShadowY_set(long int val);
void ShadowColor_set(long int val);
void BorderStyle_set(long int val);
void FK_HorizAlignment_set(long int val);
void FK_VertAlignment_set(long int val);
void BackColor_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Skin_isNull();
bool FK_Criteria_D_isNull();
bool ForeColor_isNull();
bool ShadowX_isNull();
bool ShadowY_isNull();
bool ShadowColor_isNull();
bool BackColor_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Skin_setNull(bool val);
void FK_Criteria_D_setNull(bool val);
void ForeColor_setNull(bool val);
void ShadowX_setNull(bool val);
void ShadowY_setNull(bool val);
void ShadowColor_setNull(bool val);
void BackColor_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_StyleVariation(Table_StyleVariation *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_StyleVariation *Table_StyleVariation_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Style* FK_Style_getrow();
class Row_Skin* FK_Skin_getrow();
class Row_Criteria_D* FK_Criteria_D_getrow();
class Row_HorizAlignment* FK_HorizAlignment_getrow();
class Row_VertAlignment* FK_VertAlignment_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_StyleVariation+ m_FK_Style+ m_FK_Skin+ m_FK_Criteria_D+ m_Font+ m_ForeColor+ m_PixelHeight+ m_Bold+ m_Italic+ m_Underline+ m_ShadowX+ m_ShadowY+ m_ShadowColor+ m_BorderStyle+ m_FK_HorizAlignment+ m_FK_VertAlignment+ m_BackColor+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_StyleVariation_asSQL();
string FK_Style_asSQL();
string FK_Skin_asSQL();
string FK_Criteria_D_asSQL();
string Font_asSQL();
string ForeColor_asSQL();
string PixelHeight_asSQL();
string Bold_asSQL();
string Italic_asSQL();
string Underline_asSQL();
string ShadowX_asSQL();
string ShadowY_asSQL();
string ShadowColor_asSQL();
string BorderStyle_asSQL();
string FK_HorizAlignment_asSQL();
string FK_VertAlignment_asSQL();
string BackColor_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

