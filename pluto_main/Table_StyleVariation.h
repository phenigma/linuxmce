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

class DLL_EXPORT Table_StyleVariation
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_StyleVariation(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("StyleVariation")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
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

	map<Table_StyleVariation::Key, class Row_StyleVariation*, Table_StyleVariation::Key_Less> cachedRows;
	map<Table_StyleVariation::Key, class Row_StyleVariation*, Table_StyleVariation::Key_Less> deleted_cachedRows;
	vector<class Row_StyleVariation*> addedRows;
	vector<class Row_StyleVariation*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_StyleVariation*> *rows);
	class Row_StyleVariation* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_StyleVariation* GetRow(long int in_PK_StyleVariation);
	

private:	
	
		
	class Row_StyleVariation* FetchRow(Key &key);
		
			
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
string m_Modification_RecordInfo;
short int m_IsNew_RecordInfo;
short int m_IsDeleted_RecordInfo;
long int m_FK_Users_RecordInfo;

		bool is_null[21];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
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
string Modification_RecordInfo_get();
short int IsNew_RecordInfo_get();
short int IsDeleted_RecordInfo_get();
long int FK_Users_RecordInfo_get();

		
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
void Modification_RecordInfo_set(string val);
void IsNew_RecordInfo_set(short int val);
void IsDeleted_RecordInfo_set(short int val);
void FK_Users_RecordInfo_set(long int val);

		
		bool FK_Skin_isNull();
bool FK_Criteria_D_isNull();
bool ForeColor_isNull();
bool ShadowX_isNull();
bool ShadowY_isNull();
bool ShadowColor_isNull();
bool BackColor_isNull();
bool IsNew_RecordInfo_isNull();
bool IsDeleted_RecordInfo_isNull();
bool FK_Users_RecordInfo_isNull();

			
		void FK_Skin_setNull(bool val);
void FK_Criteria_D_setNull(bool val);
void ForeColor_setNull(bool val);
void ShadowX_setNull(bool val);
void ShadowY_setNull(bool val);
void ShadowColor_setNull(bool val);
void BackColor_setNull(bool val);
void IsNew_RecordInfo_setNull(bool val);
void IsDeleted_RecordInfo_setNull(bool val);
void FK_Users_RecordInfo_setNull(bool val);
	
	
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
		void SetupSerialization() {
			StartSerializeList() + m_PK_StyleVariation+ m_FK_Style+ m_FK_Skin+ m_FK_Criteria_D+ m_Font+ m_ForeColor+ m_PixelHeight+ m_Bold+ m_Italic+ m_Underline+ m_ShadowX+ m_ShadowY+ m_ShadowColor+ m_BorderStyle+ m_FK_HorizAlignment+ m_FK_VertAlignment+ m_BackColor+ m_Modification_RecordInfo+ m_IsNew_RecordInfo+ m_IsDeleted_RecordInfo+ m_FK_Users_RecordInfo;
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
string Modification_RecordInfo_asSQL();
string IsNew_RecordInfo_asSQL();
string IsDeleted_RecordInfo_asSQL();
string FK_Users_RecordInfo_asSQL();

	};

#endif

